#pragma once
#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>
#include "node.hpp"

class HuffmanTree{
    public:
        HuffmanTree();
        HuffmanTree(std::string text);
        HuffmanTree(std::unordered_map<char, int> frequencies);
        void buildTree();
        

    private:

};