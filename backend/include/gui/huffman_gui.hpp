#pragma once

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "huffman/huffmantree.hpp"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

// Struct for tracking Huffman animation state
struct HuffmanAnimationState {
    bool isAnimating = false;
    float timer = 0.0f;
    int currentStep = 0;
    std::vector<std::pair<std::shared_ptr<Node>, std::string>> treeTraversalNodes;
    std::string currentText;
    std::string currentEncoded;
    int encodingStep = 0;
    bool encodingComplete = false;
    bool decodingMode = false;
};

class HuffmanGUI {
public:
    HuffmanGUI();
    ~HuffmanGUI();

    // Main render method
    void render();
    
    // Animation update
    void update(float deltaTime);
    
    // Getters/setters
    bool showingResults() const { return m_showHuffmanResults; }
    
private:
    // Helper methods
    void buildHuffmanTree();
    void resetHuffmanAnimation();
    void renderHuffmanTree(const std::shared_ptr<Node>& node, float x, float y, float xOffset, float yOffset, 
                          const std::string& code, bool animateTraversal);
    void countSubtreeNodes(const std::shared_ptr<Node>& node, int& count);
    void countTotalNodes(const std::shared_ptr<Node>& node, int depth, int& count, int& maxDepth);
    void updateHuffmanAnimation(float deltaTime);
    void animateEncoding();
    void animateDecoding();
    
    // Input text callback
    static int InputTextCallback(ImGuiInputTextCallbackData* data);

    // Huffman tab parameters
    enum class HuffmanInputMode {
        TEXT,
        FREQUENCIES,
        ENCODED
    };
    
    HuffmanInputMode m_huffmanInputMode;
    char m_newChar;
    int m_newFreq;
    char m_editChar;
    int m_editFreq;
    int m_editFreqIndex;
    char m_inputText[4096];
    char m_encodedText[4096];
    std::unordered_map<char, int> m_charFrequencies;
    std::unique_ptr<HuffmanTree> m_huffmanTree;
    bool m_showHuffmanResults;
    char m_resultEncodedText[4096];
    char m_resultDecodedText[4096];
    
    // Animation
    HuffmanAnimationState m_huffmanAnimation;
}; 