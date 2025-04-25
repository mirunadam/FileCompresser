#include "adaptive_huffman.hpp"
#include "bit_proccesing.hpp"
#include <fstream>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>
using namespace std;
#define ALPHABET_SIZE 256

HuffNode_A::HuffNode_A(int num, HuffNode_A *par){
    number=num;
    parent=par;
}

void AdaptiveHuffman::initialize_model(){
    root=NYT = new HuffNode_A(512); //double the alph size to have enough space
    NYT->isNYT=true;
    NYT->isLeaf=false;
    node_list.resize(513);
    node_list[512]=NYT;
}

string AdaptiveHuffman::get_code(HuffNode_A *x){
    string path="";
    while(x->parent){
        if(x == x->parent->left){
            path = "0" + path;
        }
        else{
            path = "1" + path;
        }
        x=x->parent;
    }
    return path;
}

HuffNode_A* AdaptiveHuffman::get_node(uint8_t ch){
    if(table[ch]){
        return table[ch];
    }
    else{
        return nullptr;
    }
} 

void AdaptiveHuffman::swap_nodes(HuffNode_A *a, HuffNode_A *b){
    if(!a || !b || a==b || a->parent==b || b->parent==a){
        return;
    }
    swap(node_list[a->number], node_list[b->number]); //swaps positions in vect
    swap(a->number, b->number); //swap the int node num

    HuffNode_A *parent_a = a->parent;
    HuffNode_A *parent_b = b->parent;

    if(parent_a){ //a partent to point to b
        if(parent_a->left == a){
            parent_a->left = b;
        }
        else{
            parent_a->right=b;
        }
    }
    if(parent_b){ //b parent to pint to a
        if(parent_b->left==b){
            parent_b->left = a;
        }
        else{
            parent_b->right = a;
        }
    }
    swap(a->parent, b->parent); //swap parents
}

void AdaptiveHuffman::update_model(HuffNode_A* x){
    while(x){
        for(int i = x->number+1; i<node_list.size(); ++i){ //looks for nodes with > num above the current node x
            if(node_list[i] && (node_list[i]->weight == x->weight) && //same weight
            (node_list[i]->parent !=x) && (x->parent !=node_list[i])){  //not in a parenting relship
                swap_nodes(x, node_list[i]);
                break;
            }
        }
        x->weight++;
        x=x->parent; //from leaf to root
    }
}

void AdaptiveHuffman::insert(uint8_t ch){
    HuffNode_A *newNYT = new HuffNode_A(NYT->number-2, NYT);
    HuffNode_A *leaf= new HuffNode_A(NYT->number-1, NYT);
    leaf->ch = ch;
    leaf->isLeaf = true;
    
    NYT->left = newNYT; //original NYT becomes internal node
    NYT->right = leaf;
    NYT->isNYT = false;
    NYT->isLeaf = false;

    node_list[newNYT->number] = newNYT;
    node_list[leaf->number]= leaf;

    table[ch]=leaf; //put in the symbol table
    NYT = newNYT;

    update_model(leaf); //rebalance from the new leaf
}

void AdaptiveHuffman::encode(uint8_t ch, bit_writer &writer){
    HuffNode_A *node = get_node(ch);
    if(node){ //if character was already encountered
        string path=get_code(node);
        for(char bit : path){ //write each bit of path to output
            writer.write_bit(bit=='1');
        }
        update_model(node); //update since we encountered +1
    }
    else{ //if char is new
        string path =get_code(NYT); //NYT code transmits to the decoder that it is a new character
        for(char bit : path){
            writer.write_bit(bit=='1');
        }
        writer.write_byte(ch); //writes the raw char
        insert(ch); //adds it to the tree
    }
}

void AdaptiveHuffman::free_tree(HuffNode_A *x){ //works post order ~ first free childern
    if(!x){
        return;
    }
    free_tree(x->left);
    free_tree(x->right);
    delete x;
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

void AdaptiveHuffman::compress(string &input_file, string &output_file){
    ifstream input(input_file, ios::binary);
    if(!input){
        throw runtime_error("error ~ at opening input file for compression");
    }
    AdaptiveHuffman tree;
    tree.initialize_model();

    bit_writer writer(output_file);
    char c;
    while(input.get(c)){
        tree.encode(static_cast<uint8_t>(c), writer);
    }
    writer.flush();
    print_inputfile_size(input_file);
    print_outputfile_size(output_file);
    free_tree(root);
}

