#include "../include/huffman/huffmantree.hpp"

HuffmanTree::HuffmanTree() {
    this->text = "";
    this->frequencies = {};
    this->codes = {};
    this->root = nullptr;
    this->encodedText = "";
    this->decodedText = "";
}
HuffmanTree::HuffmanTree(std::string text) {
    this->text = text;
    this->frequencies = {};
    this->codes = {};
    this->root = nullptr;
    this->encodedText = "";
    this->decodedText = "";
    calculateFrequencies();
    buildTree();
    generateCodes(root, "");
}
HuffmanTree::HuffmanTree(std::unordered_map<char, int> frequencies) {
    this->text = "";
    this->frequencies = frequencies;
    this->codes = {};
    this->root = nullptr;
    this->encodedText = "";
    this->decodedText = "";
    buildTree();
    generateCodes(root, "");
}
void HuffmanTree::calculateFrequencies() {
    for (char c : text) {
        frequencies[c]++;
    }
}
void HuffmanTree::buildTree() {
    for (const auto& pair : frequencies) {
        minHeap.push(std::make_shared<Node>(pair.second, pair.first));
    }
    while (minHeap.size() > 1) {
        std::shared_ptr<Node> left = minHeap.top();
        minHeap.pop();
        std::shared_ptr<Node> right = minHeap.top();
        minHeap.pop();
        std::shared_ptr<Node> newNode = std::make_shared<Node>(left->getFrequency() + right->getFrequency(), '\0');
        newNode->left = left;
        newNode->right = right;
        minHeap.push(newNode);
    }
    root = minHeap.top();
}
void HuffmanTree::generateCodes(std::shared_ptr<Node> root, std::string str) {
    if (root == nullptr) {
        return;
    }
    if (root->isLeaf()) {
        codes[root->getCharacter()] = str;
    }
    generateCodes(root->getLeft(), str + "0");
    generateCodes(root->getRight(), str + "1");
}
std::unordered_map<char, std::string> HuffmanTree::getCodes() const {
    return codes;
}
std::shared_ptr<Node> HuffmanTree::getRoot() const {
    return root;
}
std::string HuffmanTree::getEncodedText() const {
    return encodedText;
}
std::string HuffmanTree::getDecodedText() const {
    return decodedText;
}
std::string HuffmanTree::decode(const std::string& encodedText) {
    std::string decodedText = "";
    std::shared_ptr<Node> currentNode = root;
    for (char c : encodedText) {
        if (c == '0') {
            currentNode = currentNode->getLeft();
        } else {
            currentNode = currentNode->getRight();
        }
        if (currentNode->isLeaf()) {
            decodedText += currentNode->getCharacter();
            currentNode = root;
        }
    }
    return decodedText;
}
std::string HuffmanTree::encode(const std::string& text) {
    std::string encodedText = "";
    for (char c : text) {
        encodedText += codes[c];
    }
    return encodedText;
}
void HuffmanTree::setText(const std::string& text) {
    this->text = text;
    calculateFrequencies();
    buildTree();
    generateCodes(root, "");
}
void HuffmanTree::setFrequencies(const std::unordered_map<char, int>& frequencies) {
    this->frequencies = frequencies;
    buildTree();
    generateCodes(root, "");
}
void HuffmanTree::setEncodedText(const std::string& encodedText) {
    this->encodedText = encodedText;
}
void HuffmanTree::setDecodedText(const std::string& decodedText) {
    this->decodedText = decodedText;
}
void HuffmanTree::setRoot(std::shared_ptr<Node> root) {
    this->root = root;
}
void HuffmanTree::setCodes(const std::unordered_map<char, std::string>& codes) {
    this->codes = codes;
}
void HuffmanTree::clear() {
    text = "";
    frequencies.clear();
    codes.clear();
    root = nullptr;
    encodedText = "";
    decodedText = "";
}
void HuffmanTree::clearFrequencies() {
    frequencies.clear();
}

