#include "../../include/huffman/node.hpp"

using namespace std;

Node::Node(int frequency, char character){
    this->frequency = frequency;
    this->character = character;
    this->left = nullptr;
    this->right = nullptr;
}
int Node::getFrequency() const{
    return frequency;
}
char Node::getCharacter() const{
    return character;
}
std::shared_ptr<Node> Node::getLeft() const{
    return left;
}
std::shared_ptr<Node> Node::getRight() const{
    return right;
}
bool Node::isLeaf() const{
    return left == nullptr && right == nullptr;
}

