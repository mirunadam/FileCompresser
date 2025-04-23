#include "bit_proccesing.hpp"
#include <fstream>
#include <cstdint>
#include <string>
using namespace std;

bit_reader::bit_reader(const string &filename){
    input.open(filename, ios::binary);
}

bit_reader::~bit_reader(){
    input.close();
}

bool bit_reader::read_bit(bool &bit){
    if(nbits == 0){
        if(!input.get((char&)buffer)){ //if get failed
            return false;
        }
        nbits = 8;
    }
    bit = (buffer >> (--nbits)) &1; //extract bits from msb to lsb
    //right shift to go at the lsb and mask all bits except the lsb
    return true;
}

bool bit_reader::read_byte(uint8_t &byte){
    byte = 0;
    for(int i = 0; i<8; ++i){
        bool bit;
        if(!read_bit(bit)){
            return false;
        }
        byte |=bit <<(7-i); //shifts the bit to its correct position in the byte bfore merging 
    }
    return true;
}

bit_writer::bit_writer(const string &filename){
    output.open(filename, ios::binary);
}

bit_writer::~bit_writer(){
    write_buffer();
    output.close();
}

void bit_writer::write_bit(bool bit){
    buffer |= bit<<(7-nbits);
    if(++nbits==8){
        write_buffer(); 
    }
}

void bit_writer::write_byte(uint8_t byte){
    for(int i=7; i>=0; --i){
        write_bit((byte >> i)&1);
    }
}

void bit_writer::write_buffer(){
    if(nbits>0){
        output.put(buffer);
        buffer=0;
        nbits=0;
    }
}

void bit_writer::flush() {
    write_buffer();
    output.flush();
}
