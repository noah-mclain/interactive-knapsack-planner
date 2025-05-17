#pragma once
#include <vector>
#include <memory>
#include <queue>
#include <unordered_map>
#include <string>
#include "node.hpp"

class HuffmanTree{
    public:
        HuffmanTree();
        HuffmanTree(std::string text);
        HuffmanTree(std::unordered_map<char, int> frequencies);
        void buildTree();
        void generateCodes(std::shared_ptr<Node> root, std::string str);
        void printCodes();
        std::unordered_map<char, std::string> getCodes() const;
        std::shared_ptr<Node> getRoot() const;
        std::string getEncodedText() const;
        std::string getDecodedText() const;
        std::string decode(const std::string& encodedText);
        std::string encode(const std::string& text);
        void setText(const std::string& text);
        void setFrequencies(const std::unordered_map<char, int>& frequencies);
        void setEncodedText(const std::string& encodedText);
        void setDecodedText(const std::string& decodedText);
        void setRoot(std::shared_ptr<Node> root);
        void setCodes(const std::unordered_map<char, std::string>& codes);
    private:
        std::string text;
        std::unordered_map<char, int> frequencies;
        std::unordered_map<char, std::string> codes;
        std::shared_ptr<Node> root;
        std::string encodedText;
        std::string decodedText;
        std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, std::greater<std::shared_ptr<Node>>> minHeap;
        void calculateFrequencies();
        void buildMinHeap();
        void clear();
        void clearFrequencies();
};