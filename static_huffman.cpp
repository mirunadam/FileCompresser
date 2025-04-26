#include "static_huffman.hpp"
#include "bit_proccesing.hpp"
#include <fstream>
#include <cstdint>
#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <stdexcept>
using namespace std;
#define ALPHABET_SIZE 256 

HuffNode::HuffNode(char c, int f, HuffNode *l, HuffNode  *r){
    ch=c;
    freq=f;
    left=l;
    right=r;
}

bool const HuffNode::isLeaf(){
    return !left && !right; //true if both children are null
}

struct CompareHuffNode {
    bool operator()(HuffNode* a, HuffNode* b) { //make the obj callable like a funct +const?
        return a->freq > b->freq;
    }
};

void StaticHuffman::build_table(HuffNode *x, string path, array<string, ALPHABET_SIZE> &table){
    if(x->isLeaf()){
        table[static_cast<uint8_t>(x->ch)] = path; //fills table with character so binary code mappings
        return;
    }

    build_table(x->left, path + "0", table); //depth first as we traverse by the path
    build_table(x->right, path + "1",table);
}

HuffNode* StaticHuffman::read_trie(bit_reader &reader){
    bool isLeaf;
    if(!reader.read_bit(isLeaf)){
        throw runtime_error("error ~ when reading trie data");
    }
    if(isLeaf){
        uint8_t ch;
        if(!reader.read_byte(ch)){ //read the char if it is a leaf
            throw runtime_error("error ~ when reading trie data");
        }
        return new HuffNode(ch, -1, nullptr, nullptr); //leaf node store ch, we dont care about the freq anymore at decompress
    }
    return new HuffNode('\0', -1, read_trie(reader), read_trie(reader)); // \0 to mark its not a leaf but an internal node
}

void StaticHuffman::write_trie(HuffNode *x, bit_writer &writer){
    if(x->isLeaf()){
        writer.write_bit(true); //leaf
        writer.write_byte(x->ch); //write char
        return;
    }
    writer.write_bit(false); //internal node 
    write_trie(x->left, writer); //serialize left part
    write_trie(x->right, writer); //serialize right part
}

void StaticHuffman::free_trie(HuffNode *x){ //works post order ~ first free childern
    if(!x){
        return;
    }
    free_trie(x->left);
    free_trie(x->right);
    delete x;
}

array<int, ALPHABET_SIZE> StaticHuffman::count_freq(ifstream &input){
    array<int, ALPHABET_SIZE> freq ={0};
    char c;
    while(input.get(c)){
        freq[static_cast<uint8_t>(c)]++; //cast to make sure characters are treated as positive indexes
    } 
    input.clear();
    input.seekg(0); //rewinds to start
    return freq;
}

HuffNode* StaticHuffman::build_trie(array<int, ALPHABET_SIZE> &freq){
    priority_queue<HuffNode*, vector<HuffNode*>, CompareHuffNode> pq; //use compare to make sure nodes with low freq are at the top of the tree and have a shorter code
    for (int i=0; i<ALPHABET_SIZE; i++){
        if(freq[i]>0){
            pq.push(new HuffNode(static_cast<char>(i), freq[i], nullptr, nullptr));
        }
    }

    while(pq.size()>1){ //while there are more than the root in queue
        HuffNode *left = pq.top();
        pq.pop();
        HuffNode *right =pq.top();
        pq.pop();

        HuffNode *merged_node = new HuffNode('\0', left->freq + right->freq, left, right); //inernal node with combined frequency
        pq.push(merged_node); 
    }

    if(pq.empty()){
        return nullptr;
    }
    else{
        return pq.top(); //will be the root
    }

}
void print_outputfile_size(string &output_file){
    ifstream output(output_file, ios::binary);
    if(!output){
        throw runtime_error("error ~ at opening output file for computing size");
    }
    output.seekg(0, ios::end);
    uint32_t file_size=output.tellg();
    output.seekg(0); 
    cout << "Output file size: " << file_size << endl;
}

void print_inputfile_size(string &input_file){
    ifstream input(input_file, ios::binary);
    if(!input){
        throw runtime_error("error ~ at opening input file for compression");
    }
    input.seekg(0, ios::end);
    uint32_t file_size=input.tellg();
    input.seekg(0);
    cout << "Input file size: " << file_size << endl;
}

void StaticHuffman::compress(string &input_file, string &output_file){
    ifstream input(input_file, ios::binary);
    if(!input){
        throw runtime_error("error ~ at opening input file for compression");
    }
   
    array<int, ALPHABET_SIZE> freq = count_freq(input);

    HuffNode *root = build_trie(freq);
    if(!root){
        throw runtime_error("error ~ no trie build, empty input file");
    }

    array<string, ALPHABET_SIZE> code_table;
    string path="";
    build_table(root, path, code_table);

    bit_writer writer(output_file);
    write_trie(root, writer); //serialize

    input.seekg(0, ios::end); //move to end of file
    uint32_t file_size=input.tellg(); //
    input.seekg(0); //rewind
    cout << "Input file size: " << file_size << endl;
    //write file size by bytes
    writer.write_byte((file_size>>24)&0xFF); //msb
    writer.write_byte((file_size>>16)&0xFF);
    writer.write_byte((file_size>>8)&0xFF);
    writer.write_byte(file_size & 0xFF); //lsb

    char c;
    while(input.get(c)){
        string &code = code_table[static_cast<uint8_t>(c)];
        for(char bit : code){
            writer.write_bit(bit=='1');
        }
    }
    writer.flush();
    free_trie(root);

    print_outputfile_size(output_file);
}

void StaticHuffman::decompress(string &input_file, string &output_file){
    bit_reader reader(input_file);
    ofstream output(output_file, ios::binary);
    if(!output){
        throw runtime_error("error ~ at opening output file for decompression");
    }

    HuffNode *root=read_trie(reader);

    uint32_t file_size = 0;
    for(int i=0; i<4; i++){
        uint8_t byte;
        if(!reader.read_byte(byte)){
            throw runtime_error("error ~ at reading file size");
        }
        file_size = (file_size<<8)|byte; //reconstructs a the file size from the individual bytes
    }

    for(uint32_t i=0; i<file_size; i++){
        HuffNode *x=root;
        while(!x->isLeaf()){
            bool bit;
            if(!reader.read_bit(bit)){
                throw runtime_error("error ~ unexpected end of file");
            }
            if(bit==1){
                x=x->right;
            }
            else{
                x=x->left;
            }
        }
        output.put(x->ch);
    }
    free_trie(root);
    output.flush();
    output.close(); 
    print_inputfile_size(input_file);
    print_outputfile_size(output_file);
}
