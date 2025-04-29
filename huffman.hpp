#ifndef DATACOMPRESSION_HUFFMAN_HPP
#define DATACOMPRESSION_HUFFMAN_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

class huffman {

    struct Node {
        char character;
        unsigned int frequency;
        string code;
        Node* left;
        Node* right;

        Node(char c, unsigned int freq) : character(c), frequency(freq), code(""), left(nullptr), right(nullptr) {}
        Node(Node* l, Node* r) : character(l->character), frequency(l->frequency + r->frequency), code(""),
            left(l), right(r) {}

        ~Node() {
            delete left;
            delete right;
        }

        bool operator<(const Node& other) const {
            if (frequency == other.frequency) {
                return character < other.character;
            } else {
                return frequency < other.frequency;
            }
        }

        void generateCodes(const string& prefix, map<char, string>& codes) {
            if (left == nullptr && right == nullptr) {
                codes[character] = prefix;
            } else {
                if (left) left->generateCodes(prefix + "0", codes);
                if (right) right->generateCodes(prefix + "1", codes);
            }
        }
    };

    // Custom comparator for the set to order nodes by frequency and character
    struct NodePtrCompare {
        bool operator()(const Node* a, const Node* b) const {
            if (a->frequency == b->frequency) {
                return a->character < b->character;
            } else {
                return a->frequency < b->frequency;
            }
        }
    };

    static map<char, string> codeTable(const map<char, uint>& frequency) {
        if (frequency.size() == 1) {
            // Handle the case where there is only one character
            map<char, string> singleCode;
            singleCode[frequency.begin()->first] = "0";
            return singleCode;
        }

        set<Node*, NodePtrCompare> nodes;

        for (const auto& pair : frequency) {
            nodes.insert(new Node(pair.first, pair.second));
        }

        while (nodes.size() > 1) {
            auto left = *nodes.begin();
            nodes.erase(nodes.begin()); // remove the smallest node
            auto right = *nodes.begin();
            nodes.erase(nodes.begin()); // remove the next smallest node

            Node* parent = new Node(left, right);   // left will be smaller
            nodes.insert(parent);
        }
        Node* root = *nodes.begin();
        nodes.erase(nodes.begin());

        map<char, string> codes;
        root->generateCodes("", codes);
        delete root;  // clean up memory

        return codes;
    }

    static Node* inverseCodeTree(const map<char, uint>& frequency) {
        set<Node*, NodePtrCompare> nodes;

        for (const auto& pair : frequency) {
            nodes.insert(new Node(pair.first, pair.second));
        }

        if (nodes.size() == 1) {
            // Handle the case where there is only one character
            Node* singleNode = *nodes.begin();
            singleNode->code = "0";
        }
        while (nodes.size() > 1) {
            auto left = *nodes.begin();
            nodes.erase(nodes.begin()); // remove the smallest node
            auto right = *nodes.begin();
            nodes.erase(nodes.begin()); // remove the next smallest node

            Node* parent = new Node(left, right);   // left will be smaller
            nodes.insert(parent);
        }
        Node* root = *nodes.begin();
        nodes.erase(nodes.begin());

        return root;
    }

public:
    huffman() = default;
    ~huffman() = default;

    static void compress(const string & input_filename, const string & output_filename) {
        // Implement the compression algorithm here
        cout << "Huffman compressing data..." << endl;

        // Read the input file
        ifstream file(input_filename, ios::binary | std::ios::ate);  // open file bin mode, at the end
        if (!file) {
            throw runtime_error("Failed to open file");
        }

        streamsize size = file.tellg();  // get the size of the file
        file.seekg(0, ios::beg);  // move the cursor back to the beginning of the file
        vector<char> input(size);
        file.read(&input[0], size);  // read the file into a vector

        file.close();

        // Perform Huffman compression

        // Statistics map
        map<char, uint> frequency;
        for (char c : input) {
            if (frequency.find(c) == frequency.end()) {
                frequency[c] = 0;
            }
            frequency[c]++;
        }

        // Code table
        map<char, string> codes = codeTable(frequency);

        // Map the characters to their codes
        string bits;
        for (char c : input) {
            bits += codes[c];
        }

        // Open the output file
        ofstream outfile(output_filename, ios::binary);
        if (!outfile) {
            throw runtime_error("Failed to open output file");
        }

        // Write the number of usable bits of the last byte
        uint8_t usableBits = bits.size() % 8;
        if (usableBits == 0) {usableBits = 8;}
        outfile.write(reinterpret_cast<const char*>(&usableBits), 1);

        // Write the size of the frequency table
        uint8_t sizeOfTable = frequency.size();
        outfile.write(reinterpret_cast<const char*>(&sizeOfTable), 1);

        // Write the frequency table to the output file
        for (const auto& pair : frequency) {
            outfile.write(reinterpret_cast<const char*>(&pair.first), sizeof(pair.first));
            outfile.write(reinterpret_cast<const char*>(&pair.second), sizeof(pair.second));
        }

        // Write the compressed data
        size_t bitsSize = bits.size();
        while (bitsSize >= 8) {
            char byte = 0;
            for (int i = 0; i < 8; ++i) {
                byte = (byte << 1) | (bits[bits.size()-bitsSize+i] - '0');
            }
            bitsSize -= 8;
            outfile.write(&byte, 1);  // write the byte to the file
        }
        if (bitsSize > 0) {
            char byte = 0;
            for (int i = 0; i < bitsSize; ++i) {
                byte = (byte << 1) | (bits[bits.size()-bitsSize+i] - '0');
            }
            byte <<= (8 - int(bitsSize));  // shift the remaining bits to the left
            outfile.write(&byte, 1);  // write the last byte
        }

        outfile.close();
        cout << "done." << endl;
    }

    static void decompress(const string& input, const string& output) {
        // Implement the decompression algorithm here
        cout << "Huffman decompressing data..." << endl;

        // Read the compressed file

        // Open the input file
        ifstream file(input, ios::binary);
        if (!file) {
            throw runtime_error("Failed to open file");
        }

        // Read the number of usable bits of the last byte
        uint8_t usableBits;
        file.read(reinterpret_cast<char*>(&usableBits), 1);

        // Read the size of the frequency table
        uint8_t tmp_sizeOfTable;
        uint16_t sizeOfTable;
        file.read(reinterpret_cast<char*>(&tmp_sizeOfTable), 1);
        if (tmp_sizeOfTable == 0) {
            sizeOfTable = 256;  // By compression, the size of the table is 256
        } else {
            sizeOfTable = tmp_sizeOfTable;
        }

        // Read the frequency table
        map<char, uint> frequency;
        size_t originalSize = 0;
        for (int i = 0; i < sizeOfTable; ++i) {
            char c;
            uint freq;
            file.read(&c, sizeof(c));
            file.read(reinterpret_cast<char*>(&freq), sizeof(freq));
            frequency[c] = freq;
            originalSize += freq;
        }

        // Read the compressed data
        string bits;
        char byte;
        while (file.read(&byte, 1)) {
            for (int i = 0; i < 8; ++i) {
                bits += ((byte >> (7 - i)) & 1) ? '1' : '0';
            }
        }

        file.close();

        // Remove the unused bits from the last byte
        bits.erase(bits.size() - (8 - usableBits));

        // Build the Huffman tree and decode the data
        Node* codeRoot = inverseCodeTree(frequency);

        // Decode the data using the codes
        string decodedData;
        decodedData.reserve(originalSize);  // reserve space for the decoded data
        Node* current = codeRoot;


        for (char bit : bits) {
            // Handle the case where there is only one character
            if (current->code == "0" && current->left == nullptr && current->right == nullptr && bit == '0') {
                decodedData += current->character;
                current = codeRoot;
            }

            if (bit == '0') {
                current = current->left;
            } else {
                current = current->right;
            }

            if (current->left == nullptr && current->right == nullptr) {
                decodedData += current->character;
                current = codeRoot;  // reset to the root of the tree
            }
        }
        delete codeRoot;


        // Write the decompressed data to the output file
        ofstream outFile(output, ios::binary);
        if (!outFile) {
            throw runtime_error("Failed to open output file");
        }

        outFile.write(decodedData.c_str(), decodedData.size());

        outFile.close();
        cout << "done." << endl;
    }
};

#endif //DATACOMPRESSION_HUFFMAN_HPP
