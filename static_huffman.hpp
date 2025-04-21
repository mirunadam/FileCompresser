#pragma once
#include "bit_proccesing.hpp"
#include <fstream>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>

struct HuffNode {
    char ch;
    int freq;
    HuffNode *left, *right;
    HuffNode(char c, int f, HuffNode *l = nullptr, HuffNode *r = nullptr);
    bool isLeaf();
};