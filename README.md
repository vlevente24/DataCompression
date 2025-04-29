# LZ77 + Huffman Compressor

This project is a C++ tool for compressing and decompressing binary files using a combination of the LZ77 and Huffman algorithms.

## Algorithms in Brief

- **LZ77:** Finds repeated patterns in the input data using a sliding window and replaces them with (distance, length, next character) triples.
- **Huffman:** Builds a binary tree based on symbol frequencies to generate a prefix-free binary code, ensuring no code is a prefix of another. This allows for unambiguous and efficient decoding.

## How It Works

The program first compresses the input file with LZ77, then further compresses the result using Huffman coding.

## Usage

Run the program after compilation from the command line:

```bash
./compressor inputfile
```

where the first and only (`inputfile`) parameter is the name of the file to be compressed. The command should be run from the same directory as the input file.

The program automatically creates the compressed (`enc_inputfile`) and decompressed (`dec_inputfile`) files in the same directory.