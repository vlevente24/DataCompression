#include "lz77.hpp"
#include "huffman.hpp"
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " binary_file.data" << std::endl;
        return 1;
    }

    //std::string inputFile = "sample1.data";
    std::string inputFile = argv[1];

    try {
        lz77::compress(inputFile, "enc_" + inputFile);
        huffman::compress("enc_" + inputFile, "enc_" + inputFile);

        huffman::decompress("enc_" + inputFile, "dec_" + inputFile);
        lz77::decompress("dec_" + inputFile, "dec_" + inputFile);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 2;
    }

    return 0;
}
