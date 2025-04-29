cmake_minimum_required(VERSION 3.28)
project(DataCompression)

set(CMAKE_CXX_STANDARD 20)

add_executable(DataCompression main.cpp
        lz77.hpp
        huffman.hpp)
