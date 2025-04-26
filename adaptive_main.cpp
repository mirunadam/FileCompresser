#include "adaptive_huffman.hpp"
#include "bit_proccesing.hpp"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
//g++ -g -o adapthuff adaptive_main.cpp adaptive_huffman.cpp bit_proccesing.cpp

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage:\n";
        cerr << "  FileCompresser -c input.txt output.huff  (compress)\n";
        cerr << "  FileCompresser -d input.huff output.txt  (decompress)\n";
        return 1;
    }

    string mode = argv[1];
    string input = argv[2];
    string output = argv[3];

    AdaptiveHuffman adaptive_huffman;
    try{
        if (mode == "-c") {
            adaptive_huffman.compress(input, output);
        } else if (mode == "-d") {
            adaptive_huffman.decompress(input, output);
        } else {
            cerr << "Unknown mode: " << mode << endl;
        }
    } catch(const exception &e){
        cerr << "Error: " << e.what() << endl;
    }
    return 0;
}
