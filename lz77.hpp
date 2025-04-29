#ifndef DATACOMPRESSION_LZ77_HPP
#define DATACOMPRESSION_LZ77_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class lz77 {

    static const int bufferSize = 255;  // Maximum size of the lookbehind buffer

    struct LZ77Tuple {
        uint8_t distance;
        uint8_t length;
        char nextChar;
        LZ77Tuple(uint8_t d, uint8_t l, char nextC) : distance(d), length(l), nextChar(nextC) {}
    };

public:
    lz77() = default;
    ~lz77() = default;

    static void compress(const string & input_filename, const string & output_filename) {
        // Implement the compression algorithm here
        cout << "LZ77 compressing data..." << endl;

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

        // Perform LZ77 compression
        vector<LZ77Tuple> compressedData;
        long i = 0;

        while (i < input.size()) {
            long matchLength = 0;
            long matchDistance = 0;

            // Find the longest match in the buffer
            for (long j = 1; (j <= bufferSize) and (i - j >= 0); ++j) {
                long k = 0;
                while (k < 255 and i + k + 1 < input.size() and input[i - j + k] == input[i + k]) {
                    ++k;
                }
                if (k > matchLength) {
                    matchLength = k;
                    matchDistance = j;
                }
            }

            // Insert a tuple into the compressed data
            compressedData.emplace_back(static_cast<uint8_t>(matchDistance), static_cast<uint8_t>(matchLength),
                           input[i + matchLength]);

            // Move to the next character
            i += matchLength + 1;
        }

        // Write the compressed data to the output file
        ofstream outFile(output_filename, ios::binary);
        if (!outFile) {
            throw runtime_error("Failed to open output file");
        }

        for (const auto& tuple : compressedData) {
            outFile.write(reinterpret_cast<const char*>(&tuple.distance), sizeof(tuple.distance));
            outFile.write(reinterpret_cast<const char*>(&tuple.length), sizeof(tuple.length));
            outFile.write(&tuple.nextChar, sizeof(tuple.nextChar));
        }

        outFile.close();

        cout << "done." << endl;

    }
    static void decompress(const string& input, const string& output) {
        // Implement the decompression algorithm here
        cout << "LZ77 decompressing data..." << endl;

        // Read the compressed file
        ifstream file(input, ios::binary);
        if (!file) {
            throw runtime_error("Failed to open file");
        }

        vector<LZ77Tuple> compressedData;
        while (true) {
            uint8_t distance, length;
            char nextChar;

            if (!file.read(reinterpret_cast<char*>(&distance), sizeof(distance))) break;
            if (!file.read(reinterpret_cast<char*>(&length), sizeof(length))) break;
            if (!file.read(&nextChar, sizeof(nextChar))) break;

            compressedData.emplace_back(distance, length, nextChar);
        }

        file.close();

        // Perform LZ77 decompression
        vector<char> decompressedData;
        for (const auto& tuple : compressedData) {
            size_t start = decompressedData.size();

            // Copy the matched data from the lookbehind buffer or the decompressed data
            for (size_t j = 0; j < tuple.length; ++j) {
                decompressedData.push_back(decompressedData[start - tuple.distance + j]);
            }

            // Add the next character
            decompressedData.push_back(tuple.nextChar);
        }

        // Write the decompressed data to the output file
        ofstream outFile(output, ios::binary);
        if (!outFile) {
            throw runtime_error("Failed to open output file");
        }

        outFile.write(decompressedData.data(), static_cast<streamsize>(decompressedData.size()));
        outFile.close();

        cout << "done." << endl;
    }
};

#endif //DATACOMPRESSION_LZ77_HPP
