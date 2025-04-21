#pragma once
#include <fstream>
#include <cstdint>
#include <string>
using namespace std;

class bit_reader{
    ifstream input;
    uint8_t buffer = 0;
    int nbits = 0;
public:
    bit_reader(const string &filename);
    ~bit_reader(); //destructor
    bool read_bit(bool &bit); //will need for decoding
    bool read_byte(uint8_t &byte); //will need for adaptive
};

class bit_writer{
    ofstream output;
    uint8_t buffer = 0;
    int nbits = 0;
public:
    bit_writer(const string &filename);
    ~bit_writer();
    void write_bit(bool bit);
    void write_byte(uint8_t byte);
    void write_partial_byte();
};