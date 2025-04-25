// adaptive_huffman.hpp
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
#define ALPHABET_SIZE 256

struct HuffNode_A {
    HuffNode_A *left = nullptr, *right = nullptr, *parent = nullptr;
    int weight = 0;
    int number = 0;
    uint8_t ch;
    bool isNYT = false;
    bool isLeaf = false;

    HuffNode_A(int num, HuffNode_A *par=nullptr);
};

class AdaptiveHuffman {
    HuffNode_A *root;
    HuffNode_A *NYT; //not yet transmitted
    array<HuffNode_A *, ALPHABET_SIZE> table = {nullptr};
    vector <HuffNode_A*> node_list;

    void initialize_model();
    string get_code(HuffNode_A *x);
    HuffNode_A *get_node(uint8_t ch);
    void swap_nodes(HuffNode_A *a, HuffNode_A *b);
    void update_model(HuffNode_A* x);
    void insert(uint8_t ch);
    void free_tree(HuffNode_A *x);
    void encode(uint8_t ch, bit_writer &writer);



public:
    void compress(string &input_file, string &output_file);
    //void decompress(string &input_file, string &output_file);
};

