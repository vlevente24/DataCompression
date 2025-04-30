#include <iostream>
#include <string>
#include <sstream>
#include "lz77.hpp"
#include "huffman.hpp"

using namespace std;

streamsize fileSize(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file) {
        throw runtime_error("Failed to open file");
    }
    streamsize size = file.tellg();
    file.close();
    return size;
}

void compressFile(const string& inputFileName) {
    streamsize og, lz, huff, lz_huff;

    og = fileSize(inputFileName);

    lz77::compress(inputFileName, "enc_" + inputFileName);
    lz = fileSize("enc_" + inputFileName);

    huffman::compress(inputFileName, "henc_" + inputFileName);
    huff = fileSize("henc_" + inputFileName);

    huffman::compress("enc_" + inputFileName, "enc_" + inputFileName);
    lz_huff = fileSize("enc_" + inputFileName);

    cout << endl << "File: " << inputFileName << endl;
    cout << "Original size: " << og << " bytes" << endl;
    cout << "LZ77 size: " << lz << " bytes" << endl;
    cout << "Huffman size: " << huff << " bytes" << endl;
    cout << "LZ77 + Huffman size: " << lz_huff << " bytes" << endl << endl;
}

int main() {
    /*
    for (int i = 1; i <= 7; ++i) {
        stringstream inputFile;
        inputFile << "sample" << i << ".data";
        string inputFileName = inputFile.str();
        cout << "Processing file: " << inputFileName << endl;

        try {
            compressFile(inputFileName);
        } catch (const std::exception& e) {
            cerr << "Error: " << e.what() << endl;
            return 2;
        }
    }
    */

    try {

        string inputFileName = "desert.bmp";
        compressFile(inputFileName);

        inputFileName = "lake.bmp";
        compressFile(inputFileName);

        inputFileName = "pattern.bmp";
        compressFile(inputFileName);

        inputFileName = "sw.bmp";
        compressFile(inputFileName);

        inputFileName = "swlogo.bmp";
        compressFile(inputFileName);

    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 2;
    }

    return 0;
}