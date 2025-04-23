#pragma once
#include <fstream>
#include <cstdint>
#include <array>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include "bit_proccesing.hpp"
using namespace std;

struct HuffNode {
    char ch;
    int freq;
    HuffNode *left, *right;
    HuffNode(char c, int f, HuffNode *l = nullptr, HuffNode *r = nullptr);
    bool const isLeaf();
};

class StaticHuffman {
    static const int ALPHABET_SIZE = 256;
    void build_table(HuffNode *x, string path, array<string, ALPHABET_SIZE> &table);
    array<int, ALPHABET_SIZE> count_freq(ifstream &input);
    HuffNode* build_trie(array<int, ALPHABET_SIZE> &freq);
    HuffNode *read_trie(bit_reader &reader);
    void write_trie(HuffNode *x, bit_writer &writer);
    void free_trie(HuffNode *x);

public: 
    void compress(string &input_file, string &output_file);
    void decompress(string &input_file, string &output_file);
};