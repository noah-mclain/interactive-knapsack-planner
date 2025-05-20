#include "../../include/gui/huffman_gui.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <cmath>

HuffmanGUI::HuffmanGUI()
    : m_huffmanInputMode(HuffmanInputMode::TEXT)
    , m_newChar(0)
    , m_newFreq(0)
    , m_editChar(0)
    , m_editFreq(0)
    , m_editFreqIndex(-1)
    , m_showHuffmanResults(false)
{
    // Initialize char buffers with explicit null termination
    m_inputText[0] = '\0';
    m_encodedText[0] = '\0';
    
    // Clear result buffers completely (use memset for safety)
    std::memset(m_resultEncodedText, 0, sizeof(m_resultEncodedText));
    std::memset(m_resultDecodedText, 0, sizeof(m_resultDecodedText));
}

HuffmanGUI::~HuffmanGUI()
{
    // Huffman tree will be cleaned up automatically by unique_ptr
}

void HuffmanGUI::update(float deltaTime)
{
    // Update Huffman animation
    updateHuffmanAnimation(deltaTime);
}

void HuffmanGUI::render()
{
    ImGui::Text("Huffman Encoding");
    ImGui::Separator();
    
    // Input mode selection
    ImGui::Text("Select Input Mode:");
    ImGui::Columns(3, "input_mode_columns", false);
    
    // Radio buttons for input mode
    bool isTextMode = (m_huffmanInputMode == HuffmanInputMode::TEXT);
    bool isFreqMode = (m_huffmanInputMode == HuffmanInputMode::FREQUENCIES);
    bool isEncodedMode = (m_huffmanInputMode == HuffmanInputMode::ENCODED);
    
    if (ImGui::RadioButton("Text Input", isTextMode)) {
        m_huffmanInputMode = HuffmanInputMode::TEXT;
        m_showHuffmanResults = false;
    }
    ImGui::NextColumn();
    
    if (ImGui::RadioButton("Character Frequencies", isFreqMode)) {
        m_huffmanInputMode = HuffmanInputMode::FREQUENCIES;
        m_showHuffmanResults = false;
    }
    ImGui::NextColumn();
    
    if (ImGui::RadioButton("Encoded String", isEncodedMode)) {
        m_huffmanInputMode = HuffmanInputMode::ENCODED;
        m_showHuffmanResults = false;
    }
    ImGui::Columns(1);
    
    ImGui::Separator();
    
    // Different input forms based on mode
    switch (m_huffmanInputMode) {
        case HuffmanInputMode::TEXT:
        {
            ImGui::Text("Enter text to encode:");
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::InputTextMultiline("##text_input", m_inputText, sizeof(m_inputText), ImVec2(-1, 80), 
                                          ImGuiInputTextFlags_AllowTabInput | 
                                          ImGuiInputTextFlags_CtrlEnterForNewLine |
                                          ImGuiInputTextFlags_EnterReturnsTrue, 
                                          InputTextCallback)) {
                m_showHuffmanResults = false;
            }
            ImGui::PopItemWidth();
            break;
        }
            
        case HuffmanInputMode::FREQUENCIES:
        {
            ImGui::Text("Character Frequencies:");
            
            // Table for frequencies
            if (ImGui::BeginTable("freq_table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Character", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Frequency", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                
                // Display existing frequencies
                int index = 0;
                for (auto it = m_charFrequencies.begin(); it != m_charFrequencies.end();) {
                    ImGui::TableNextRow();
                    
                    // Character
                    ImGui::TableNextColumn();
                    char displayChar[32];
                    // Handle special characters for display
                    if (it->first == '\n') snprintf(displayChar, sizeof(displayChar), "\\n");
                    else if (it->first == '\r') snprintf(displayChar, sizeof(displayChar), "\\r");
                    else if (it->first == '\t') snprintf(displayChar, sizeof(displayChar), "\\t");
                    else if (it->first == ' ') snprintf(displayChar, sizeof(displayChar), "Space");
                    else snprintf(displayChar, sizeof(displayChar), "%c", it->first);
                    ImGui::Text("%s", displayChar);
                    
                    // Frequency
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", it->second);
                    
                    // Actions
                    ImGui::TableNextColumn();
                    ImGui::PushID(index);
                    
                    // Edit button
                    if (ImGui::Button("Edit")) {
                        m_editFreqIndex = index;
                        m_editChar = it->first;
                        m_editFreq = it->second;
                        ImGui::OpenPopup("EditFreqPopup");
                    }
                    
                    ImGui::SameLine();
                    
                    // Delete button
                    if (ImGui::Button("Delete")) {
                        it = m_charFrequencies.erase(it);
                        m_showHuffmanResults = false;
                    } else {
                        ++it;
                    }
                    
                    ImGui::PopID();
                    ImGui::TableNextColumn(); // Empty column
                    
                    index++;
                }
                
                ImGui::EndTable();
            }
            
            // Add new character frequency
            ImGui::Separator();
            ImGui::Text("Add New Character Frequency");
            
            ImGui::PushItemWidth(150);
            
            // Character input - limit to one character
            char freqCharBuf[2] = {m_newChar, 0};
            if (ImGui::InputText("Character", freqCharBuf, 2, 
                                ImGuiInputTextFlags_CharsNoBlank | 
                                ImGuiInputTextFlags_AllowTabInput |
                                ImGuiInputTextFlags_EnterReturnsTrue |
                                ImGuiInputTextFlags_CallbackEdit)) {
                m_newChar = freqCharBuf[0];
                // Move focus to frequency field when enter is pressed
                ImGui::SetKeyboardFocusHere(0);
            }

            // Show placeholder suggestions for character input if empty
            if (m_newChar == 0 && ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Try a character like 'a', 'e', 't', etc.");
                ImGui::EndTooltip();
            }

            ImGui::SameLine();
            if (ImGui::InputInt("Frequency", &m_newFreq, 1, 5, 
                              ImGuiInputTextFlags_AllowTabInput |
                              ImGuiInputTextFlags_EnterReturnsTrue)) {
                // Add character when Enter is pressed in frequency field
                if (m_newChar != 0 && m_newFreq > 0) {
                    m_charFrequencies[m_newChar] = m_newFreq;
                    m_newChar = 0;
                    m_newFreq = 0;
                    m_showHuffmanResults = false;
                }
            }

            // Show placeholder suggestions for frequency input if zero
            if (m_newFreq == 0 && ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Common frequencies: 'e':12, 't':9, 'a':8...");
                ImGui::EndTooltip();
            }

            ImGui::PopItemWidth();

            if (ImGui::Button("Add Character") && m_newChar != 0 && m_newFreq > 0) {
                m_charFrequencies[m_newChar] = m_newFreq;
                m_newChar = 0;
                m_newFreq = 0;
                m_showHuffmanResults = false;
            }

            ImGui::SameLine();

            // Add commonly used character sets button
            if (ImGui::Button("Add Example Set")) {
                ImGui::OpenPopup("ExampleCharSetPopup");
            }

            if (ImGui::BeginPopup("ExampleCharSetPopup")) {
                ImGui::Text("Choose a character set:");
                ImGui::Separator();
                
                if (ImGui::Selectable("English Common Letters")) {
                    // Clear existing frequencies first
                    m_charFrequencies.clear();
                    
                    // Add common English letters with approximate frequencies
                    m_charFrequencies['e'] = 12;
                    m_charFrequencies['t'] = 9;
                    m_charFrequencies['a'] = 8;
                    m_charFrequencies['o'] = 8;
                    m_charFrequencies['i'] = 7;
                    m_charFrequencies['n'] = 7;
                    m_charFrequencies['s'] = 6;
                    m_charFrequencies['h'] = 6;
                    m_charFrequencies['r'] = 6;
                    m_charFrequencies['d'] = 4;
                    m_charFrequencies['l'] = 4;
                    m_charFrequencies['u'] = 3;
                    
                    m_showHuffmanResults = false;
                    ImGui::CloseCurrentPopup();
                }
                
                if (ImGui::Selectable("Simple Testing Set")) {
                    // Clear existing frequencies first
                    m_charFrequencies.clear();
                    
                    // Simple set for testing
                    m_charFrequencies['a'] = 5;
                    m_charFrequencies['b'] = 2;
                    m_charFrequencies['c'] = 1;
                    m_charFrequencies['d'] = 1;
                    
                    m_showHuffmanResults = false;
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::EndPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Clear All")) {
                ImGui::OpenPopup("ClearFreqConfirmPopup");
            }
            
            // Confirmation popup for clearing
            if (ImGui::BeginPopupModal("ClearFreqConfirmPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Are you sure you want to clear all character frequencies?");
                ImGui::Separator();
                
                if (ImGui::Button("Yes", ImVec2(120, 0))) {
                    m_charFrequencies.clear();
                    m_showHuffmanResults = false;
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::SameLine();
                
                if (ImGui::Button("No", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::EndPopup();
            }
            
            // Edit popup for frequency
            if (ImGui::BeginPopupModal("EditFreqPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Edit Character Frequency");
                ImGui::Separator();
                
                // Character can't be edited, only frequency
                char displayChar[32];
                if (m_editChar == '\n') snprintf(displayChar, sizeof(displayChar), "\\n");
                else if (m_editChar == '\r') snprintf(displayChar, sizeof(displayChar), "\\r");
                else if (m_editChar == '\t') snprintf(displayChar, sizeof(displayChar), "\\t");
                else if (m_editChar == ' ') snprintf(displayChar, sizeof(displayChar), "Space");
                else snprintf(displayChar, sizeof(displayChar), "%c", m_editChar);
                
                ImGui::Text("Character: %s", displayChar);
                if (ImGui::InputInt("Frequency", &m_editFreq, 1, 5, 
                                  ImGuiInputTextFlags_AllowTabInput |
                                  ImGuiInputTextFlags_EnterReturnsTrue |
                                  ImGuiInputTextFlags_CallbackEdit)) {
                    // Save frequency when Enter is pressed
                    if (m_editFreq > 0) {
                        m_charFrequencies[m_editChar] = m_editFreq;
                        m_editFreqIndex = -1;
                        m_showHuffmanResults = false;
                        ImGui::CloseCurrentPopup();
                    }
                }
                
                // Still keep the buttons for those who prefer mouse interaction
                if (ImGui::Button("Save", ImVec2(120, 0)) && m_editFreq > 0) {
                    // Since we're editing the frequency for an existing character,
                    // update the map
                    m_charFrequencies[m_editChar] = m_editFreq;
                    m_editFreqIndex = -1;
                    m_showHuffmanResults = false;
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::SameLine();
                
                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    m_editFreqIndex = -1;
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::EndPopup();
            }
            break;
        }
            
        case HuffmanInputMode::ENCODED:
        {
            ImGui::Text("Enter encoded string (binary):");
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            if (ImGui::InputTextMultiline("##encoded_input", m_encodedText, sizeof(m_encodedText), ImVec2(-1, 80),
                                          ImGuiInputTextFlags_AllowTabInput | 
                                          ImGuiInputTextFlags_CtrlEnterForNewLine |
                                          ImGuiInputTextFlags_EnterReturnsTrue,
                                          InputTextCallback)) {
                // Validate the input to ensure it's only 0s and 1s
                for (char& c : m_encodedText) {
                    if (c != '0' && c != '1' && c != '\0') {
                        c = '0'; // Replace invalid characters with 0
                    }
                }
                m_showHuffmanResults = false;
            }
            ImGui::PopItemWidth();
            
            ImGui::Separator();
            ImGui::Text("Character Frequencies (required for decoding):");
            
            // Character frequencies table (similar to FREQUENCIES mode)
            if (ImGui::BeginTable("freq_decode_table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Character", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Frequency", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();
                
                // Display existing frequencies
                int index = 0;
                for (auto it = m_charFrequencies.begin(); it != m_charFrequencies.end();) {
                    ImGui::TableNextRow();
                    
                    // Character
                    ImGui::TableNextColumn();
                    char displayChar[32];
                    if (it->first == '\n') snprintf(displayChar, sizeof(displayChar), "\\n");
                    else if (it->first == '\r') snprintf(displayChar, sizeof(displayChar), "\\r");
                    else if (it->first == '\t') snprintf(displayChar, sizeof(displayChar), "\\t");
                    else if (it->first == ' ') snprintf(displayChar, sizeof(displayChar), "Space");
                    else snprintf(displayChar, sizeof(displayChar), "%c", it->first);
                    ImGui::Text("%s", displayChar);
                    
                    // Frequency
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", it->second);
                    
                    // Actions
                    ImGui::TableNextColumn();
                    ImGui::PushID(100 + index); // Different ID namespace
                    
                    // Edit button
                    if (ImGui::Button("Edit")) {
                        m_editFreqIndex = index;
                        m_editChar = it->first;
                        m_editFreq = it->second;
                        ImGui::OpenPopup("EditFreqDecodePopup");
                    }
                    
                    ImGui::SameLine();
                    
                    // Delete button
                    if (ImGui::Button("Delete")) {
                        it = m_charFrequencies.erase(it);
                        m_showHuffmanResults = false;
                    } else {
                        ++it;
                    }
                    
                    ImGui::PopID();
                    ImGui::TableNextColumn(); // Empty column
                    
                    index++;
                }
                
                ImGui::EndTable();
            }
            
            // Add new character frequency (same as in FREQUENCIES mode)
            ImGui::Separator();
            ImGui::Text("Add New Character Frequency");
            
            ImGui::PushItemWidth(150);
            char decodeCharBuf[2] = {m_newChar, 0};
            if (ImGui::InputText("Character", decodeCharBuf, 2, 
                                ImGuiInputTextFlags_CharsNoBlank)) {
                m_newChar = decodeCharBuf[0];
            }
            
            ImGui::SameLine();
            ImGui::InputInt("Frequency", &m_newFreq);
            ImGui::PopItemWidth();
            
            if (ImGui::Button("Add Character") && m_newChar != 0 && m_newFreq > 0) {
                m_charFrequencies[m_newChar] = m_newFreq;
                m_newChar = 0;
                m_newFreq = 0;
                m_showHuffmanResults = false;
            }
            
            // Edit popup for frequency
            if (ImGui::BeginPopupModal("EditFreqDecodePopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Edit Character Frequency");
                ImGui::Separator();
                
                // Character can't be edited, only frequency
                char displayChar[32];
                if (m_editChar == '\n') snprintf(displayChar, sizeof(displayChar), "\\n");
                else if (m_editChar == '\r') snprintf(displayChar, sizeof(displayChar), "\\r");
                else if (m_editChar == '\t') snprintf(displayChar, sizeof(displayChar), "\\t");
                else if (m_editChar == ' ') snprintf(displayChar, sizeof(displayChar), "Space");
                else snprintf(displayChar, sizeof(displayChar), "%c", m_editChar);
                
                ImGui::Text("Character: %s", displayChar);
                ImGui::InputInt("Frequency", &m_editFreq);
                
                if (ImGui::Button("Save", ImVec2(120, 0)) && m_editFreq > 0) {
                    m_charFrequencies[m_editChar] = m_editFreq;
                    m_editFreqIndex = -1;
                    m_showHuffmanResults = false;
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::SameLine();
                
                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    m_editFreqIndex = -1;
                    ImGui::CloseCurrentPopup();
                }
                
                ImGui::EndPopup();
            }
            break;
        }
    }
    
    // Build/create button
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    
    bool canBuildTree = false;
    
    switch (m_huffmanInputMode) {
        case HuffmanInputMode::TEXT:
            canBuildTree = (m_inputText[0] != '\0');
            if (ImGui::Button("Encode Text", ImVec2(150, 0)) && canBuildTree) {
                buildHuffmanTree();
            }
            break;
            
        case HuffmanInputMode::FREQUENCIES:
            canBuildTree = !m_charFrequencies.empty();
            if (ImGui::Button("Build Tree", ImVec2(150, 0)) && canBuildTree) {
                buildHuffmanTree();
            }
            break;
            
        case HuffmanInputMode::ENCODED:
            canBuildTree = (m_encodedText[0] != '\0') && !m_charFrequencies.empty();
            if (ImGui::Button("Decode", ImVec2(150, 0)) && canBuildTree) {
                buildHuffmanTree();
            }
            break;
    }
    
    ImGui::PopStyleColor(3);
    
    // Information message if the button is disabled
    if (!canBuildTree) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Enter data first");
    }
    
    // Display results section if available
    if (m_showHuffmanResults && m_huffmanTree) {
        ImGui::Separator();
        // Make this more visible with colors
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.3f, 1.0f), "Results:");
        
        // Force update results directly
        if (m_huffmanTree) {
            if (m_resultEncodedText[0] == '\0') {
                std::string encodedText = m_huffmanTree->getEncodedText();
                if (!encodedText.empty()) {
                    std::strncpy(m_resultEncodedText, encodedText.c_str(), sizeof(m_resultEncodedText) - 1);
                    m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                } else if (m_encodedText[0] != '\0') {
                    std::strncpy(m_resultEncodedText, m_encodedText, sizeof(m_resultEncodedText) - 1);
                    m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                }
            }
            
            if (m_resultDecodedText[0] == '\0') {
                std::string decodedText = m_huffmanTree->getDecodedText();
                if (!decodedText.empty()) {
                    std::strncpy(m_resultDecodedText, decodedText.c_str(), sizeof(m_resultDecodedText) - 1);
                    m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                } else if (m_inputText[0] != '\0') {
                    std::strncpy(m_resultDecodedText, m_inputText, sizeof(m_resultDecodedText) - 1);
                    m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                }
            }
        }
        
        // Results Tabs - if no tab selected, select encoding/decoding by default
        static int activeTab = 1;  // Default to encoding/decoding tab
        ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_None;
        
        if (ImGui::BeginTabBar("HuffmanResultTabs", tabFlags)) {
            // Tree Visualization Tab
            if (ImGui::BeginTabItem("Tree Visualization")) {
                ImGui::Text("Huffman Tree:");
                
                // Show appropriate guidance in the UI based on mode
                if (m_huffmanAnimation.decodingMode) {
                    ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), 
                                    "Decoding Mode: Follow the tree from root to leaf nodes as you read each 0/1 bit.");
                } else {
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), 
                                    "Encoding Mode: Each path from root to leaf defines a character's Huffman code.");
                }
                
                // Create a child window for the tree visualization with scrolling
                float treeHeight = 650.0f;
                
                // Control panel area
                ImGui::BeginChild("ControlPanel", ImVec2(0, 40), false);
                
                // Use more muted colors for buttons
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.4f, 0.3f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.35f, 0.8f));
                
                // Zoom/pan controls
                static float zoomLevel = 1.0f;
                static ImVec2 panOffset(0.0f, 0.0f);
                
                // Variables for tracking scroll position
                static bool initialScrollSet = false;
                static float lastCanvasWidth = 0;
                static float lastCanvasHeight = 0;
                static int lastTotalNodes = 0;
                
                if (ImGui::Button("Zoom In")) {
                    zoomLevel *= 1.2f;
                }
                ImGui::SameLine();
                if (ImGui::Button("Zoom Out")) {
                    zoomLevel = std::max(0.1f, zoomLevel / 1.2f);
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset View")) {
                    zoomLevel = 1.0f;
                    panOffset = ImVec2(0.0f, 0.0f);
                    
                    // Reset initial scroll state to force re-centering
                    initialScrollSet = false;
                }
                
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "| Ctrl+Scroll: Zoom, Shift+Scroll: Horizontal, Scroll: Vertical");
                
                ImGui::PopStyleColor(2);
                ImGui::EndChild();
                
                // Define a clear visual border for the tree area
                ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(80, 180, 100, 200)); // Softer green border
                ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);
                
                // Background color for the tree area - softer dark background
                ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(28, 30, 36, 255));
                
                // Use a different style for decoding mode
                if (m_huffmanAnimation.decodingMode) {
                    // Use a slightly bluer background for decoding mode
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(26, 28, 38, 255));
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
                }
                
                // This is the scrollable tree area - with both scrollbars enabled
                ImGui::BeginChild("TreeArea", ImVec2(0, treeHeight - 40), true, 
                               ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
                
                // Get the exact dimensions of our tree container
                ImVec2 boxPos = ImGui::GetCursorScreenPos();
                ImVec2 contentSize = ImGui::GetContentRegionAvail();
                float contentWidth = contentSize.x;
                float contentHeight = contentSize.y;
                
                // Store box bounds
                ImVec2 boxMin = boxPos;
                ImVec2 boxMax = ImVec2(boxPos.x + contentWidth, boxPos.y + contentHeight);
                
                // Calculate if mouse is over the tree area
                ImVec2 mousePos = ImGui::GetIO().MousePos;
                bool isMouseOverTreeArea = mousePos.x >= boxMin.x && mousePos.x <= boxMax.x &&
                                          mousePos.y >= boxMin.y && mousePos.y <= boxMax.y;
                
                // Count total number of nodes to better estimate required space
                int totalNodes = 0;
                int maxDepth = 0;
                if (m_huffmanTree && m_huffmanTree->getRoot()) {
                    countTotalNodes(m_huffmanTree->getRoot(), 0, totalNodes, maxDepth);
                }
                
                // Count nodes in left and right subtrees of the root to determine tree balance
                int leftSubtreeNodes = 0;
                int rightSubtreeNodes = 0;
                if (m_huffmanTree && m_huffmanTree->getRoot()) {
                    if (m_huffmanTree->getRoot()->getLeft()) {
                        countSubtreeNodes(m_huffmanTree->getRoot()->getLeft(), leftSubtreeNodes);
                    }
                    if (m_huffmanTree->getRoot()->getRight()) {
                        countSubtreeNodes(m_huffmanTree->getRoot()->getRight(), rightSubtreeNodes);
                    }
                }
                
                // Calculate tree size to ensure proper scrolling
                float estimatedTreeWidth = std::pow(2, maxDepth) * 200.0f * zoomLevel;
                float estimatedTreeHeight = (maxDepth + 1) * 240.0f * zoomLevel;
                
                // Create a canvas large enough to contain the entire tree
                // Add extra padding on all sides for better scrolling
                ImVec2 canvasSize(std::max(contentWidth * 2.0f, estimatedTreeWidth + 800.0f),
                                 std::max(contentHeight * 2.0f, estimatedTreeHeight + 300.0f));
                
                // Calculate initial scroll position to center horizontally, but position at top of tree
                float initialScrollX = (canvasSize.x - contentWidth) * 0.5f;
                float initialScrollY = 50.0f; // Position near the top with small offset
                
                // Set the content size for proper scrolling - crucial for scroll to work!
                ImGui::SetCursorPos(ImVec2(0, 0));
                ImGui::InvisibleButton("canvas", canvasSize, ImGuiButtonFlags_None);
                
                // Set scroll positions to center the canvas initially when tree is first shown
                if (!initialScrollSet || 
                    std::abs(lastCanvasWidth - canvasSize.x) > 50 || 
                    std::abs(lastCanvasHeight - canvasSize.y) > 50 ||
                    lastTotalNodes != totalNodes) {
                    
                    ImGui::SetScrollX(initialScrollX);
                    ImGui::SetScrollY(initialScrollY);
                    initialScrollSet = true;
                    lastCanvasWidth = canvasSize.x;
                    lastCanvasHeight = canvasSize.y;
                    lastTotalNodes = totalNodes;
                }
                
                // Calculate the center point of the virtual canvas
                // Center the tree horizontally but position root near the top
                ImVec2 canvasCenter(
                    boxMin.x + canvasSize.x * 0.5f,
                    boxMin.y + 120.0f  // Position near the top with enough room for the tree root
                );
                
                // Apply the pan offset to the center point
                canvasCenter.x += panOffset.x;
                canvasCenter.y += panOffset.y;
                
                // Add a small helper note about scrolling
                ImGui::SetCursorPos(ImVec2(10, 10));
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.7f), "Scroll with mouse wheel, drag with middle mouse button");
                
                // Handle mouse wheel for zooming and panning
                if (ImGui::IsWindowFocused() && isMouseOverTreeArea) {
                    float wheel = ImGui::GetIO().MouseWheel;
                    if (wheel != 0) {
                        if (ImGui::GetIO().KeyShift) {
                            // Horizontal scrolling when Shift is held
                            panOffset.x += wheel * 30.0f;
                        } else if (ImGui::GetIO().KeyCtrl) {
                            // Zoom when Ctrl is held
                            float prevZoom = zoomLevel;
                            zoomLevel *= (1.0f + wheel * 0.1f);
                            // Constrain zoom to reasonable limits
                            zoomLevel = std::max(0.1f, std::min(5.0f, zoomLevel));
                            
                            // Adjust pan offset to zoom toward mouse position
                            float zoomRatio = zoomLevel / prevZoom;
                            ImVec2 mouseRelPos = ImVec2(mousePos.x - canvasCenter.x, mousePos.y - canvasCenter.y);
                            panOffset.x -= mouseRelPos.x * (zoomRatio - 1.0f);
                            panOffset.y -= mouseRelPos.y * (zoomRatio - 1.0f);
                        } else {
                            // Vertical scrolling by default
                            panOffset.y += wheel * 30.0f;
                        }
                    }
                    
                    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
                        ImVec2 delta = ImGui::GetIO().MouseDelta;
                        panOffset.x += delta.x;
                        panOffset.y += delta.y;
                    }
                }
                
                // Calculate base offsets (independent of zoom)
                float baseXOffset = 250.0f;
                float baseYOffset = 150.0f;
                
                // Scale offsets based on zoom for final rendering
                float xOffset = baseXOffset * zoomLevel;
                float yOffset = baseYOffset * zoomLevel;
                
                // Render the tree at the canvas center
                if (m_huffmanTree && m_huffmanTree->getRoot()) {
                    renderHuffmanTree(m_huffmanTree->getRoot(), canvasCenter.x, canvasCenter.y, xOffset, yOffset, "", true);
                }
                
                ImGui::EndChild(); // End of TreeArea
                
                // Pop styles
                if (m_huffmanAnimation.decodingMode) {
                    ImGui::PopStyleColor(); // ChildBg for decoding mode
                    ImGui::PopStyleVar();   // ChildRounding
                }
                
                ImGui::PopStyleColor(2); // ChildBg and Border
                ImGui::PopStyleVar();    // BorderSize
                
                // Add animation controls
                ImGui::Separator();
                if (ImGui::Button("Restart Animation")) {
                    resetHuffmanAnimation();
                }
                
                ImGui::EndTabItem();
            }
            
            // Encoding/Decoding Results Tab - Force select this tab by default
            if (ImGui::BeginTabItem("Encoding/Decoding", nullptr, activeTab == 1 ? ImGuiTabItemFlags_SetSelected : 0)) {
                // Reset the active tab flag after first use
                activeTab = 0;
                // Display different content based on mode
                if (m_huffmanInputMode == HuffmanInputMode::ENCODED) {
                    // Decoding results - Force get data from tree first
                    if (m_huffmanTree) {
                        std::string encodedText = m_huffmanTree->getEncodedText();
                        if (!encodedText.empty()) {
                            std::strncpy(m_resultEncodedText, encodedText.c_str(), sizeof(m_resultEncodedText) - 1);
                            m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                        } else if (m_encodedText[0] != '\0') {
                            std::strncpy(m_resultEncodedText, m_encodedText, sizeof(m_resultEncodedText) - 1);
                            m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                        }
                        
                        std::string decodedText = m_huffmanTree->getDecodedText();
                        if (!decodedText.empty()) {
                            std::strncpy(m_resultDecodedText, decodedText.c_str(), sizeof(m_resultDecodedText) - 1);
                            m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                        }
                    }
                    
                    // Draw UI elements
                    ImGui::Text("Encoded String:");
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                    
                    // Add more visible indicators for debugging
                    if (strlen(m_resultEncodedText) > 0) {
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.3f, 1.0f), 
                            "Encoded length: %zu bytes", strlen(m_resultEncodedText));
                    } else {
                        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), 
                            "Warning: Encoded text is empty! Check HuffmanTree implementation.");
                    }
                    
                    ImGui::InputTextMultiline("##encoded_result", m_resultEncodedText, sizeof(m_resultEncodedText), 
                                             ImVec2(ImGui::GetContentRegionAvail().x, 80), ImGuiInputTextFlags_ReadOnly);
                    ImGui::PopItemWidth();
                    
                    ImGui::Text("Decoded Text:");
                    
                    // Add more visible indicators for debugging
                    if (strlen(m_resultDecodedText) > 0) {
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.3f, 1.0f), 
                            "Decoded length: %zu bytes", strlen(m_resultDecodedText));
                    } else {
                        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), 
                            "Warning: Decoded text is empty! Check HuffmanTree implementation.");
                    }
                    
                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputTextMultiline("##decoded_result", m_resultDecodedText, sizeof(m_resultDecodedText), 
                                             ImVec2(ImGui::GetContentRegionAvail().x, 80), ImGuiInputTextFlags_ReadOnly);
                    ImGui::PopItemWidth();
                    ImGui::PopStyleColor();
                } else {
                    // Encoding results - Force get data from tree first
                    if (m_huffmanTree) {
                        std::string encodedText = m_huffmanTree->getEncodedText();
                        if (!encodedText.empty()) {
                            std::strncpy(m_resultEncodedText, encodedText.c_str(), sizeof(m_resultEncodedText) - 1);
                            m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                        }
                        
                        std::string decodedText = m_huffmanTree->getDecodedText();
                        if (!decodedText.empty()) {
                            std::strncpy(m_resultDecodedText, decodedText.c_str(), sizeof(m_resultDecodedText) - 1);
                            m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                        } else if (m_inputText[0] != '\0') {
                            std::strncpy(m_resultDecodedText, m_inputText, sizeof(m_resultDecodedText) - 1);
                            m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                        }
                    }
                    
                    // Draw UI elements
                    ImGui::Text("Original Text:");
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                    
                    ImGui::Text("Original length: %zu bytes, Content: %s", strlen(m_resultDecodedText),
                               strlen(m_resultDecodedText) > 0 ? "present" : "empty");
                    
                    ImGui::InputTextMultiline("##original_text", m_resultDecodedText, sizeof(m_resultDecodedText), 
                                             ImVec2(ImGui::GetContentRegionAvail().x, 80), ImGuiInputTextFlags_ReadOnly);
                    ImGui::PopItemWidth();
                    
                    ImGui::Text("Encoded String:");
                    ImGui::Text("Encoded length: %zu bytes, Content: %s", strlen(m_resultEncodedText),
                               strlen(m_resultEncodedText) > 0 ? "present" : "empty");
                    
                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputTextMultiline("##encoded_result", m_resultEncodedText, sizeof(m_resultEncodedText), 
                                             ImVec2(ImGui::GetContentRegionAvail().x, 80), ImGuiInputTextFlags_ReadOnly);
                    ImGui::PopItemWidth();
                    ImGui::PopStyleColor();
                }
                
                ImGui::Separator();
                
                ImGui::EndTabItem();
            }
            
            // Huffman Codes Tab
            if (ImGui::BeginTabItem("Huffman Codes")) {
                ImGui::Text("Character Codes:");
                
                if (ImGui::BeginTable("codes_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                    ImGui::TableSetupColumn("Character", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                    ImGui::TableSetupColumn("Frequency", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                    ImGui::TableSetupColumn("Huffman Code", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();
                    
                    auto codes = m_huffmanTree->getCodes();
                    for (const auto& pair : m_charFrequencies) {
                        char c = pair.first;
                        int freq = pair.second;
                        
                        ImGui::TableNextRow();
                        
                        // Character
                        ImGui::TableNextColumn();
                        char displayChar[32];
                        if (c == '\n') snprintf(displayChar, sizeof(displayChar), "\\n");
                        else if (c == '\r') snprintf(displayChar, sizeof(displayChar), "\\r");
                        else if (c == '\t') snprintf(displayChar, sizeof(displayChar), "\\t");
                        else if (c == ' ') snprintf(displayChar, sizeof(displayChar), "Space");
                        else snprintf(displayChar, sizeof(displayChar), "%c", c);
                        ImGui::Text("%s", displayChar);
                        
                        // Frequency
                        ImGui::TableNextColumn();
                        ImGui::Text("%d", freq);
                        
                        // Code
                        ImGui::TableNextColumn();
                        if (codes.find(c) != codes.end()) {
                            ImGui::Text("%s", codes[c].c_str());
                        } else {
                            ImGui::Text("N/A");
                        }
                    }
                    
                    ImGui::EndTable();
                }
                
                // Efficiency statistics
                ImGui::Separator();
                ImGui::Text("Compression Statistics:");
                
                if (m_resultDecodedText[0] != '\0' && m_resultEncodedText[0] != '\0') {
                    // Calculate the original size in bits (assuming 8 bits per character)
                    size_t originalSizeBits = strlen(m_resultDecodedText) * 8;
                    
                    // Encoded size in bits
                    size_t encodedSizeBits = strlen(m_resultEncodedText);
                    
                    // Compression ratio
                    float compressionRatio = static_cast<float>(originalSizeBits) / encodedSizeBits;
                    
                    // Space saving percentage
                    float spaceSaving = (1.0f - (static_cast<float>(encodedSizeBits) / originalSizeBits)) * 100.0f;
                    
                    // Avoid potential buffer overflow by limiting string display
                    size_t decodedTextLen = strlen(m_resultDecodedText);
                    if (decodedTextLen > 1000) {
                        // If text is too long, show truncated version in the UI
                        ImGui::Text("Original Size: %zu bits (%zu bytes) [Text truncated for display]", 
                                   originalSizeBits, decodedTextLen);
                    } else {
                        ImGui::Text("Original Size: %zu bits (%zu bytes)", originalSizeBits, decodedTextLen);
                    }
                    
                    ImGui::Text("Encoded Size: %zu bits (%zu bytes)", encodedSizeBits, (encodedSizeBits + 7) / 8);
                    ImGui::Text("Compression Ratio: %.2f:1", compressionRatio);
                    ImGui::Text("Space Saving: %.2f%%", spaceSaving);
                    
                    // Calculate average code length
                    float totalCodeLength = 0.0f;
                    float totalFrequency = 0.0f;
                    
                    auto codes = m_huffmanTree->getCodes();
                    for (const auto& pair : m_charFrequencies) {
                        if (codes.find(pair.first) != codes.end()) {
                            totalCodeLength += static_cast<float>(codes[pair.first].length() * pair.second);
                            totalFrequency += static_cast<float>(pair.second);
                        }
                    }
                    
                    float avgCodeLength = totalCodeLength / totalFrequency;
                    ImGui::Text("Average Code Length: %.2f bits", avgCodeLength);
                    
                    // Entropy calculation
                    float entropy = 0.0f;
                    for (const auto& pair : m_charFrequencies) {
                        float probability = static_cast<float>(pair.second) / totalFrequency;
                        entropy -= probability * std::log2(probability);
                    }
                    
                    ImGui::Text("Entropy: %.2f bits", entropy);
                    ImGui::Text("Encoding Efficiency: %.2f%%", (entropy / avgCodeLength) * 100.0f);
                }
                
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
    }
}

void HuffmanGUI::updateHuffmanAnimation(float deltaTime) {
    if (!m_huffmanAnimation.isAnimating)
        return;
    
    m_huffmanAnimation.timer += deltaTime * 2.0f; // Animation speed factor
    
    if (m_huffmanAnimation.timer >= 1.0f) {
        m_huffmanAnimation.timer = 0.0f;
        
        // If we're animating tree traversal
        if (m_huffmanAnimation.currentStep < static_cast<int>(m_huffmanAnimation.treeTraversalNodes.size())) {
            m_huffmanAnimation.currentStep++;
            
            // If traversal is complete, start encoding/decoding animation
            if (m_huffmanAnimation.currentStep >= static_cast<int>(m_huffmanAnimation.treeTraversalNodes.size())) {
                // Set up for encoding/decoding animation
                m_huffmanAnimation.encodingStep = 0;
                m_huffmanAnimation.currentText = "";
                m_huffmanAnimation.currentEncoded = "";
            }
        }
        // If we're animating encoding/decoding
        else if (!m_huffmanAnimation.encodingComplete) {
            if (m_huffmanAnimation.decodingMode) {
                animateDecoding();
            } else {
                animateEncoding();
            }
        }
    }
}

int HuffmanGUI::InputTextCallback(ImGuiInputTextCallbackData* data)
{
    // Allow the backspace/delete keys to properly work
    if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit)
    {
        // Handle edit events if needed
        return 0;
    }
    else if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize callback
        if (data->Buf)
        {
            // Handle resizing if needed
            return 0;
        }
    }
    return 0;
}

void HuffmanGUI::buildHuffmanTree() {
    // Reset results
    m_resultEncodedText[0] = '\0';
    m_resultDecodedText[0] = '\0';
    
    try {
        switch (m_huffmanInputMode) {
            case HuffmanInputMode::TEXT:
                if (m_inputText[0] != '\0') {
                    // Create Huffman tree from input text
                    m_huffmanTree = std::make_unique<HuffmanTree>(std::string(m_inputText));
                    
                    // Create frequencies map for display
                    m_charFrequencies.clear();
                    for (char* c = m_inputText; *c != '\0'; c++) {
                        m_charFrequencies[*c]++;
                    }
                    
                    // Encode the text
                    std::string encodedResult = m_huffmanTree->encode(std::string(m_inputText));
                    
                    // Make sure to copy the strings completely and ensure null termination
                    std::strncpy(m_resultEncodedText, encodedResult.c_str(), sizeof(m_resultEncodedText) - 1); 
                    m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                    
                    std::strncpy(m_resultDecodedText, m_inputText, sizeof(m_resultDecodedText) - 1);
                    m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                    
                    // Store the results in the HuffmanTree object too
                    m_huffmanTree->setEncodedText(encodedResult);
                    m_huffmanTree->setDecodedText(std::string(m_inputText));
                    
                    m_huffmanAnimation.decodingMode = false;
                    m_showHuffmanResults = true;
                }
                break;
                
            case HuffmanInputMode::FREQUENCIES:
                if (!m_charFrequencies.empty()) {
                    // Create Huffman tree from frequencies
                    m_huffmanTree = std::make_unique<HuffmanTree>(m_charFrequencies);
                    
                    // Build a sample text for encoding
                    std::string sampleText;
                    for (const auto& pair : m_charFrequencies) {
                        // Add each character proportional to its frequency, but limit for readability
                        sampleText.append(std::min(pair.second, 5), pair.first);
                    }
                    
                    // Make sure sample text isn't too large
                    if (sampleText.length() > sizeof(m_inputText) - 1) {
                        sampleText = sampleText.substr(0, sizeof(m_inputText) - 1);
                    }
                    
                    // Encode the sample text
                    std::strncpy(m_inputText, sampleText.c_str(), sizeof(m_inputText) - 1);
                    m_inputText[sizeof(m_inputText) - 1] = '\0';
                    
                    std::string encodedResult = m_huffmanTree->encode(sampleText);
                    std::strncpy(m_resultEncodedText, encodedResult.c_str(), sizeof(m_resultEncodedText) - 1);
                    m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                    
                    std::strncpy(m_resultDecodedText, sampleText.c_str(), sizeof(m_resultDecodedText) - 1);
                    m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                    
                    // Store results in HuffmanTree object
                    m_huffmanTree->setEncodedText(encodedResult);
                    m_huffmanTree->setDecodedText(sampleText);
                    
                    m_huffmanAnimation.decodingMode = false;
                    m_showHuffmanResults = true;
                }
                break;
                
            case HuffmanInputMode::ENCODED:
                if (m_encodedText[0] != '\0' && !m_charFrequencies.empty()) {
                    // Validate encoded text (should only contain 0s and 1s)
                    bool validEncoding = true;
                    for (char* c = m_encodedText; *c != '\0'; c++) {
                        if (*c != '0' && *c != '1') {
                            validEncoding = false;
                            break;
                        }
                    }
                    
                    if (!validEncoding) {
                        // Handle invalid encoding - could show an error message here
                        return;
                    }
                    
                    // Create Huffman tree from frequencies
                    m_huffmanTree = std::make_unique<HuffmanTree>(m_charFrequencies);
                    
                    // Decode the encoded string
                    std::string encodedStr(m_encodedText);
                    std::string decodedResult = m_huffmanTree->decode(encodedStr);
                    
                    std::strncpy(m_resultDecodedText, decodedResult.c_str(), sizeof(m_resultDecodedText) - 1);
                    m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                    
                    std::strncpy(m_resultEncodedText, m_encodedText, sizeof(m_resultEncodedText) - 1);
                    m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                    
                    // Store results in HuffmanTree object
                    m_huffmanTree->setEncodedText(encodedStr);
                    m_huffmanTree->setDecodedText(decodedResult);
                    
                    m_huffmanAnimation.decodingMode = true;
                    m_showHuffmanResults = true;
                }
                break;
        }
        
        // Reset animation state for tree visualization
        if (m_huffmanTree) {
            resetHuffmanAnimation();
        }
    } catch (const std::exception& e) {
        // Handle any exceptions during tree building or encoding/decoding
        // In a real application, you would log this error or show it to the user
        m_huffmanTree.reset(); // Clear the tree on error
        m_showHuffmanResults = false;
    }
}

void HuffmanGUI::countTotalNodes(const std::shared_ptr<Node>& node, int depth, int& count, int& maxDepth) {
    if (!node) return;
    
    count++;
    maxDepth = std::max(maxDepth, depth);
    
    countTotalNodes(node->getLeft(), depth + 1, count, maxDepth);
    countTotalNodes(node->getRight(), depth + 1, count, maxDepth);
}

void HuffmanGUI::countSubtreeNodes(const std::shared_ptr<Node>& node, int& count) {
    if (!node) return;
    
    count++;
    countSubtreeNodes(node->getLeft(), count);
    countSubtreeNodes(node->getRight(), count);
}

void HuffmanGUI::resetHuffmanAnimation() {
    m_huffmanAnimation.isAnimating = true;
    m_huffmanAnimation.timer = 0.0f;
    m_huffmanAnimation.currentStep = 0;
    m_huffmanAnimation.treeTraversalNodes.clear();
    m_huffmanAnimation.currentText = "";
    m_huffmanAnimation.currentEncoded = "";
    m_huffmanAnimation.encodingStep = 0;
    m_huffmanAnimation.encodingComplete = false;
    
    // Perform tree traversal and store paths for animation
    if (m_huffmanTree && m_huffmanTree->getRoot()) {
        // Function to recursively traverse tree and collect nodes with their codes
        std::function<void(std::shared_ptr<Node>, std::string)> traverseTree = 
            [this, &traverseTree](std::shared_ptr<Node> node, std::string code) {
                if (node) {
                    m_huffmanAnimation.treeTraversalNodes.push_back({node, code});
                    
                    if (node->getLeft()) {
                        traverseTree(node->getLeft(), code + "0");
                    }
                    if (node->getRight()) {
                        traverseTree(node->getRight(), code + "1");
                    }
                }
            };
        
        traverseTree(m_huffmanTree->getRoot(), "");
    }
}

void HuffmanGUI::animateEncoding() {
    size_t inputLen = strlen(m_inputText);
    if (inputLen == 0 || m_huffmanAnimation.encodingStep >= static_cast<int>(inputLen)) {
        m_huffmanAnimation.encodingComplete = true;
        return;
    }
    
    // Process a small batch of characters at a time for smoother animation
    int charsToProcess = std::min(3, static_cast<int>(inputLen) - m_huffmanAnimation.encodingStep);
    int i = 0;
    
    auto codes = m_huffmanTree->getCodes();
    
    while (i < charsToProcess && m_huffmanAnimation.encodingStep < static_cast<int>(inputLen)) {
        // Get current character to encode
        char currentChar = m_inputText[m_huffmanAnimation.encodingStep];
        
        // Add character to the current text
        m_huffmanAnimation.currentText += currentChar;
        
        // Add its code to the encoded text
        if (codes.find(currentChar) != codes.end()) {
            m_huffmanAnimation.currentEncoded += codes[currentChar];
        } else {
            // If we don't have a code for this character, create a default one (shouldn't happen normally)
            std::string defaultCode = "0";
            m_huffmanAnimation.currentEncoded += defaultCode;
        }
        
        // Move to the next character
        m_huffmanAnimation.encodingStep++;
        i++;
    }
    
    // Check if encoding is complete
    if (m_huffmanAnimation.encodingStep >= static_cast<int>(inputLen)) {
        m_huffmanAnimation.encodingComplete = true;
        
        // When animation completes, make sure the result buffers are updated
        if (m_resultEncodedText[0] == '\0' && !m_huffmanAnimation.currentEncoded.empty()) {
            std::strncpy(m_resultEncodedText, m_huffmanAnimation.currentEncoded.c_str(), sizeof(m_resultEncodedText) - 1);
            m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
        }
        
        if (m_resultDecodedText[0] == '\0' && !m_huffmanAnimation.currentText.empty()) {
            std::strncpy(m_resultDecodedText, m_huffmanAnimation.currentText.c_str(), sizeof(m_resultDecodedText) - 1);
            m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
        }
        
        // Also update the Huffman tree object
        if (m_huffmanTree) {
            m_huffmanTree->setEncodedText(m_huffmanAnimation.currentEncoded);
            m_huffmanTree->setDecodedText(m_huffmanAnimation.currentText);
        }
    }
}

void HuffmanGUI::animateDecoding() {
    size_t encodedLen = strlen(m_encodedText);
    if (encodedLen == 0 || m_huffmanAnimation.encodingComplete) {
        return;
    }
    
    if (m_huffmanTree && m_huffmanTree->getRoot()) {
        // Perform incremental decoding
        std::shared_ptr<Node> currentNode = m_huffmanTree->getRoot();
        std::string bitsSoFar = m_huffmanAnimation.currentEncoded;
        std::string newBits = "";
        
        // Find how many more bits we can decode in this step
        int startPos = bitsSoFar.length();
        int i = startPos;
        
        // Handle the case where we might have already processed all input bits
        if (i >= static_cast<int>(encodedLen)) {
            m_huffmanAnimation.encodingComplete = true;
            
            // Update result buffers when animation completes
            if (m_resultEncodedText[0] == '\0' && !m_huffmanAnimation.currentEncoded.empty()) {
                std::strncpy(m_resultEncodedText, m_huffmanAnimation.currentEncoded.c_str(), sizeof(m_resultEncodedText) - 1);
                m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
            }
            
            if (m_resultDecodedText[0] == '\0' && !m_huffmanAnimation.currentText.empty()) {
                std::strncpy(m_resultDecodedText, m_huffmanAnimation.currentText.c_str(), sizeof(m_resultDecodedText) - 1);
                m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
            }
            
            // Also update the Huffman tree object
            if (m_huffmanTree) {
                m_huffmanTree->setEncodedText(m_huffmanAnimation.currentEncoded);
                m_huffmanTree->setDecodedText(m_huffmanAnimation.currentText);
            }
            
            return;
        }
        
        // Process a small chunk of bits at a time for better animation
        int bitsToProcess = std::min(5, static_cast<int>(encodedLen) - i);
        int bitsProcessed = 0;
        
        while (bitsProcessed < bitsToProcess && i < static_cast<int>(encodedLen)) {
            char bit = m_encodedText[i];
            
            // Validate bit is either '0' or '1'
            if (bit != '0' && bit != '1') {
                i++;
                continue;
            }
            
            newBits += bit;
            
            if (bit == '0' && currentNode->getLeft()) {
                currentNode = currentNode->getLeft();
            } else if (bit == '1' && currentNode->getRight()) {
                currentNode = currentNode->getRight();
            } else {
                // Invalid bit or tree structure
                i++;
                bitsProcessed++;
                continue;
            }
            
            i++;
            bitsProcessed++;
            
            // If we reached a leaf node
            if (currentNode->isLeaf()) {
                m_huffmanAnimation.currentText += currentNode->getCharacter();
                currentNode = m_huffmanTree->getRoot();
                break;
            }
        }
        
        // Update the bits we've processed
        m_huffmanAnimation.currentEncoded += newBits;
        
        // Check if decoding is complete
        if (m_huffmanAnimation.currentEncoded.length() >= encodedLen) {
            m_huffmanAnimation.encodingComplete = true;
            
            // Update result buffers when animation completes
            if (m_resultEncodedText[0] == '\0' && !m_huffmanAnimation.currentEncoded.empty()) {
                std::strncpy(m_resultEncodedText, m_huffmanAnimation.currentEncoded.c_str(), sizeof(m_resultEncodedText) - 1);
                m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
            }
            
            if (m_resultDecodedText[0] == '\0' && !m_huffmanAnimation.currentText.empty()) {
                std::strncpy(m_resultDecodedText, m_huffmanAnimation.currentText.c_str(), sizeof(m_resultDecodedText) - 1);
                m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
            }
            
            // Also update the Huffman tree object
            if (m_huffmanTree) {
                m_huffmanTree->setEncodedText(m_huffmanAnimation.currentEncoded);
                m_huffmanTree->setDecodedText(m_huffmanAnimation.currentText);
            }
        }
    }
}

void HuffmanGUI::renderHuffmanTree(const std::shared_ptr<Node>& node, float x, float y, float xOffset, float yOffset, const std::string& code, bool animateTraversal) {
    if (!node)
        return;
    
    // Get current draw list for rendering
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Adjust position to be relative to the window
    ImVec2 nodePos(x, y);
    
    // For animating nodes in order of traversal
    bool isHighlighted = false;
    bool isInPath = false;
    
    if (animateTraversal && !m_huffmanAnimation.treeTraversalNodes.empty()) {
        // Check if this node is part of the current traversal stage
        for (int i = 0; i < m_huffmanAnimation.currentStep && i < static_cast<int>(m_huffmanAnimation.treeTraversalNodes.size()); i++) {
            if (m_huffmanAnimation.treeTraversalNodes[i].first.get() == node.get()) {
                isInPath = true;
                // Highlight the most recent node
                if (i == m_huffmanAnimation.currentStep - 1) {
                    isHighlighted = true;
                }
                break;
            }
        }
    }
    
    // Calculate node colors and sizes with less saturation
    ImU32 nodeColor;
    ImU32 nodeBorderColor = IM_COL32(70, 70, 70, 220); // Darker, less saturated border
    
    // Use smaller node size like in the black and white reference image
    float baseNodeSize = 32.0f;
    float nodeSize = baseNodeSize * (yOffset / 150.0f); // Scale node size consistently with zoom
    
    if (isHighlighted) {
        // Softer highlight for the active node
        nodeColor = IM_COL32(230, 190, 60, 230);
        nodeSize = nodeSize * 1.1f; // Slightly larger for emphasis, while maintaining zoom scaling
    } else if (isInPath) {
        // Previously traversed node - more muted
        nodeColor = IM_COL32(110, 180, 120, 230);
    } else if (node->isLeaf()) {
        // Leaf nodes - use less saturated green
        int freq = node->getFrequency();
        float brightness = 0.6f + (freq % 5) * 0.06f;
        
        // More muted, softer green for leaf nodes
        nodeColor = IM_COL32(
            90 * brightness, 
            170 * brightness, 
            110 * brightness, 
            230
        );
        
        // Character 'w' in softer yellow
        if (node->getCharacter() == 'w') {
            nodeColor = IM_COL32(220, 190, 60, 230);
        }
        // Character 'm' in yellow like the original image
        else if (node->getCharacter() == 'm') {
            nodeColor = IM_COL32(220, 190, 60, 230);
        }
    } else {
        // Internal nodes - softer green
        nodeColor = IM_COL32(100, 170, 130, 230);
    }
    
    // Draw the node with a thinner border like in the reference image
    drawList->AddCircleFilled(nodePos, nodeSize + 1.5f, nodeBorderColor, 20);
    drawList->AddCircleFilled(nodePos, nodeSize, nodeColor, 20);
    
    // Draw node content
    char label[64];
    
    if (node->isLeaf()) {
        // Format with character and frequency - simpler format like in reference image
        char displayChar = node->getCharacter();
        // Handle special characters for display
        if (displayChar == '\n') snprintf(label, sizeof(label), "'\\n'\n%d", node->getFrequency());
        else if (displayChar == '\r') snprintf(label, sizeof(label), "'\\r'\n%d", node->getFrequency());
        else if (displayChar == '\t') snprintf(label, sizeof(label), "'\\t'\n%d", node->getFrequency());
        else if (displayChar == ' ') snprintf(label, sizeof(label), "' '\n%d", node->getFrequency());
        else snprintf(label, sizeof(label), "'%c'\n%d", displayChar, node->getFrequency());
    } else {
        // Internal node - just show frequency
        snprintf(label, sizeof(label), "%d", node->getFrequency());
    }
    
    // Calculate text position (centered in node)
    ImVec2 textSize = ImGui::CalcTextSize(label);
    ImVec2 textPos(
        nodePos.x - textSize.x * 0.5f,
        nodePos.y - textSize.y * 0.5f
    );
    
    // Draw text shadow for better readability
    drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 150), label);
    // Draw node text with clearer color
    drawList->AddText(textPos, IM_COL32(0, 0, 0, 230), label);
    
    // Recursively draw child nodes
    if (node->getLeft() || node->getRight()) {
        // Count leaf nodes in each subtree to allocate space proportionally
        int leftLeafCount = 0, rightLeafCount = 0;
        
        // Calculate leaf counts in each subtree
        if (node->getLeft()) {
            countLeafNodes(node->getLeft(), leftLeafCount);
        }
        if (node->getRight()) {
            countLeafNodes(node->getRight(), rightLeafCount);
        }
        
        // Ensure minimum values for proper spacing
        leftLeafCount = std::max(1, leftLeafCount);
        rightLeafCount = std::max(1, rightLeafCount);
        
        // Increase vertical spacing between levels for clarity
        float baseVerticalDistance = 180.0f;
        
        // Apply zoom factor - this scales the entire tree uniformly
        float zoomFactor = yOffset / 150.0f;
        
        // Use a more horizontal layout like the reference image
        // Reduced base angle for wider horizontal spread
        float baseAngle = 40.0f;
        
        // Calculate spread based on tree level to prevent overlaps
        int level = code.length();
        int totalLeaves = leftLeafCount + rightLeafCount;
        
        // Fixed angle based on level to ensure consistent horizontal spread
        // Use exponential decrease to make top levels wider and lower levels narrower
        float levelFactor = std::exp(-level * 0.2f);
        
        // Final angle with minimum to prevent vertical lines
        float finalAngle = std::max(15.0f, baseAngle * levelFactor);
        
        // Calculate fixed edge length to ensure all edges are identical
        // Gradually increase edge length at higher levels for better spacing
        float levelMultiplier = 1.0f + (level > 0 ? 0.0f : 0.2f); // Root level has longer edges
        float edgeLength = baseVerticalDistance * levelMultiplier;
        
        // Calculate the x and y components based on the angle and edge length
        float xComponent = edgeLength * std::sin(finalAngle * 3.14159f / 180.0f);
        float yComponent = edgeLength * std::cos(finalAngle * 3.14159f / 180.0f);
        
        // Apply zoom scaling
        xComponent *= zoomFactor;
        yComponent *= zoomFactor;
        
        // Position left and right children using calculated components to ensure identical edge lengths
        float leftX = x - xComponent;
        float rightX = x + xComponent;
        float childY = y + yComponent;
        
        // Ensure horizontal spacing by spreading nodes at each level
        // Scale factor increases with level to ensure sufficient room for leaf nodes
        float spreadFactor = 1.0f + std::min(3.0f, level * 0.5f);
        
        // Apply spread adjustment based on the number of leaf nodes
        if (leftLeafCount > 1 || rightLeafCount > 1) {
            float leftSpread = std::log(leftLeafCount + 1) * spreadFactor;
            float rightSpread = std::log(rightLeafCount + 1) * spreadFactor;
            
            // Apply spread adjustments
            leftX -= nodeSize * leftSpread * zoomFactor;
            rightX += nodeSize * rightSpread * zoomFactor;
        }
        
        // Draw connecting lines and edge labels
        if (node->getLeft()) {
            // Draw edge to left child
            ImVec2 parentBottomCenter(nodePos.x, nodePos.y + nodeSize + 2.5f);
            ImVec2 childTopCenter(leftX, childY - nodeSize - 2.5f);
            
            // Edge color
            ImU32 edgeColor = IM_COL32(130, 130, 130, 180);
            float edgeThickness = 2.2f * (yOffset / 150.0f); // Scale with zoom
            
            // Draw edge from bottom of parent to top of child
            drawList->AddLine(parentBottomCenter, childTopCenter, edgeColor, edgeThickness);
            
            // Calculate midpoint for edge label
            ImVec2 midPoint(
                parentBottomCenter.x * 0.4f + childTopCenter.x * 0.6f,
                parentBottomCenter.y * 0.4f + childTopCenter.y * 0.6f
            );
            
            // Draw smaller label circle like in the reference image
            float labelSize = 10.0f * (yOffset / 150.0f); // Scale with zoom
            drawList->AddCircleFilled(midPoint, labelSize + 1.0f, IM_COL32(40, 40, 40, 150), 12);
            drawList->AddCircleFilled(midPoint, labelSize, IM_COL32(240, 240, 240, 230), 12);
            
            // Draw the '0' label
            ImVec2 textSize = ImGui::CalcTextSize("0");
            ImVec2 labelPos(
                midPoint.x - textSize.x * 0.5f,
                midPoint.y - textSize.y * 0.5f
            );
            
            drawList->AddText(labelPos, IM_COL32(30, 30, 30, 220), "0");
            
            // Recursively draw left subtree
            renderHuffmanTree(node->getLeft(), leftX, childY, xOffset, yOffset, code + "0", animateTraversal);
        }
        
        if (node->getRight()) {
            // Draw edge to right child
            ImVec2 parentBottomCenter(nodePos.x, nodePos.y + nodeSize + 2.5f);
            ImVec2 childTopCenter(rightX, childY - nodeSize - 2.5f);
            
            // Edge color
            ImU32 edgeColor = IM_COL32(130, 130, 130, 180);
            float edgeThickness = 2.2f * (yOffset / 150.0f); // Scale with zoom
            
            // Draw edge from bottom of parent to top of child
            drawList->AddLine(parentBottomCenter, childTopCenter, edgeColor, edgeThickness);
            
            // Calculate midpoint for edge label
            ImVec2 midPoint(
                parentBottomCenter.x * 0.4f + childTopCenter.x * 0.6f,
                parentBottomCenter.y * 0.4f + childTopCenter.y * 0.6f
            );
            
            // Draw smaller label circle like in the reference image
            float labelSize = 10.0f * (yOffset / 150.0f); // Scale with zoom
            drawList->AddCircleFilled(midPoint, labelSize + 1.0f, IM_COL32(40, 40, 40, 150), 12);
            drawList->AddCircleFilled(midPoint, labelSize, IM_COL32(240, 240, 240, 230), 12);
            
            // Draw the '1' label
            ImVec2 textSize = ImGui::CalcTextSize("1");
            ImVec2 labelPos(
                midPoint.x - textSize.x * 0.5f,
                midPoint.y - textSize.y * 0.5f
            );
            
            drawList->AddText(labelPos, IM_COL32(30, 30, 30, 220), "1");
            
            // Recursively draw right subtree
            renderHuffmanTree(node->getRight(), rightX, childY, xOffset, yOffset, code + "1", animateTraversal);
        }
    }
}

// New helper method to count leaf nodes in a subtree
void HuffmanGUI::countLeafNodes(const std::shared_ptr<Node>& node, int& count) {
    if (!node) return;
    
    if (node->isLeaf()) {
        count++;
    } else {
        countLeafNodes(node->getLeft(), count);
        countLeafNodes(node->getRight(), count);
    }
} 