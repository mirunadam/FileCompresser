#include "static_huffman.hpp"
#include "bit_proccesing.hpp"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
//g++ -g -o stathuff static_main.cpp static_huffman.cpp bit_proccesing.cpp

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

    StaticHuffman static_huffman;
    try{
        if (mode == "-c") {
            static_huffman.compress(input, output);
        } else if (mode == "-d") {
            static_huffman.decompress(input, output);
        } else {
            cerr << "Unknown mode: " << mode << endl;
        }
    } catch(const exception &e){
        cerr << "Error: " << e.what() << endl;
    }
    return 0;
}
