#pragma once
#include <memory>

class Node{
    public:
        Node(int frequency, char character);
        //Node(int frequency, std::shared_ptr<Node> left, std::shared_ptr<Node> right);
        int getFrequency() const;
        char getCharacter() const;
        std::shared_ptr<Node> getLeft() const;
        std::shared_ptr<Node> getRight() const;
        bool isLeaf() const;
    private:
        int frequency;
        char character;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
};