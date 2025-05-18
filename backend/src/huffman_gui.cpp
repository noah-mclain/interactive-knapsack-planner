#include "gui/huffman_gui.hpp"
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
    // Initialize char buffers
    m_inputText[0] = '\0';
    m_encodedText[0] = '\0';
    m_resultEncodedText[0] = '\0';
    m_resultDecodedText[0] = '\0';
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
            ImGui::PopItemWidth();
            
            if (ImGui::Button("Add Character") && m_newChar != 0 && m_newFreq > 0) {
                m_charFrequencies[m_newChar] = m_newFreq;
                m_newChar = 0;
                m_newFreq = 0;
                m_showHuffmanResults = false;
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
        ImGui::Text("Results:");
        
        // Results Tabs
        if (ImGui::BeginTabBar("HuffmanResultTabs")) {
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
                float treeHeight = 650.0f; // Increased height for the tree view
                
                // Use a different style for decoding mode
                if (m_huffmanAnimation.decodingMode) {
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.14f, 0.17f, 0.94f)); // Darker background
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f); // More rounded corners
                }
                
                // Store the current cursor position - this marks the top-left of our box
                ImVec2 boxStart = ImGui::GetCursorScreenPos();
                
                // Begin the child window that forms our box outline
                ImGui::BeginChild("TreeView", ImVec2(0, treeHeight), true, 
                                ImGuiWindowFlags_HorizontalScrollbar);
                
                // Get the exact dimensions of our box container
                ImVec2 boxPos = ImGui::GetCursorScreenPos();
                ImVec2 contentSize = ImGui::GetContentRegionAvail();
                float contentWidth = contentSize.x;
                float contentHeight = contentSize.y;
                
                // Store box bounds for debugging
                ImVec2 boxMin = boxPos;
                ImVec2 boxMax = ImVec2(boxPos.x + contentWidth, boxPos.y + contentHeight);
                
                // Add zoom and pan controls for better tree navigation
                static float zoomLevel = 1.0f;
                static ImVec2 panOffset(0.0f, 0.0f);
                
                // Set the starting position for the tree root relative to the box
                float startX = contentWidth / 2.0f + panOffset.x;  // Center horizontally + pan
                float startY = 80.0f + panOffset.y;  // Position down from the top + pan
                
                ImGui::SetCursorPos(ImVec2(10, 10));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.4f, 0.6f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.5f, 0.7f, 0.7f));
                
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
                }
                
                ImGui::PopStyleColor(2);
                
                // Create a specific rectangle for the tree visualization area
                ImVec2 treeAreaMin = ImVec2(boxMin.x + 10, boxMin.y + 50); // Add some padding from the controls
                ImVec2 treeAreaMax = ImVec2(boxMax.x - 10, boxMax.y - 10);
                ImVec2 treeAreaSize = ImVec2(treeAreaMax.x - treeAreaMin.x, treeAreaMax.y - treeAreaMin.y);
                
                // Calculate if mouse is over the tree area
                ImVec2 mousePos = ImGui::GetIO().MousePos;
                bool isMouseOverTreeArea = mousePos.x >= treeAreaMin.x && mousePos.x <= treeAreaMax.x &&
                                          mousePos.y >= treeAreaMin.y && mousePos.y <= treeAreaMax.y;
                
                // Handle mouse wheel for zooming and panning ONLY when mouse is over the tree area
                if (ImGui::IsWindowFocused() && isMouseOverTreeArea) {
                    // Vertical mouse wheel controls zoom
                    float wheel = ImGui::GetIO().MouseWheel;
                    if (wheel != 0) {
                        zoomLevel *= (1.0f + wheel * 0.1f);
                        zoomLevel = std::max(0.1f, std::min(5.0f, zoomLevel));
                    }
                    
                    // Handle panning with middle mouse button
                    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
                        ImVec2 delta = ImGui::GetIO().MouseDelta;
                        panOffset.x += delta.x;
                        panOffset.y += delta.y;
                    }
                }
                
                // Adjust offsets based on mode and available space
                float xOffset;
                float yOffset;
                
                // Count total number of nodes to better estimate required space
                int totalNodes = 0;
                int maxDepth = 0;
                if (m_huffmanTree->getRoot()) {
                    countTotalNodes(m_huffmanTree->getRoot(), 0, totalNodes, maxDepth);
                }
                
                // Calculate adaptive spacing based on available width and tree size
                // Use the content width divided by (2^(max depth/2)) to ensure nodes at deepest level don't overflow
                float maxWidthNeeded = std::pow(2, std::min(maxDepth, 5)) * 90.0f; // Estimate width needed (90px per node)
                float scaleFactor = std::min(1.3f, contentWidth / maxWidthNeeded);
                
                // Calculate tree size to ensure proper scrolling
                float estimatedTreeWidth = std::pow(2, maxDepth) * 150.0f * zoomLevel;
                float estimatedTreeHeight = (maxDepth + 1) * 180.0f * zoomLevel;
                
                // Ensure we have enough content area for scrolling when zoomed in
                ImGui::SetCursorPos(ImVec2(0, 0));
                ImGui::Dummy(ImVec2(std::max(contentWidth, estimatedTreeWidth), std::max(contentHeight, estimatedTreeHeight)));
                
                if (m_huffmanAnimation.decodingMode) {
                    // In decoding mode, adjust spacing based on available space
                    xOffset = contentWidth / (maxDepth + 2) * scaleFactor * zoomLevel;
                    yOffset = std::min(150.0f, contentHeight / (maxDepth + 1)) * zoomLevel;
                } else {
                    // More compact spacing for encoding mode
                    xOffset = contentWidth / (maxDepth + 3) * scaleFactor * zoomLevel;
                    yOffset = std::min(130.0f, contentHeight / (maxDepth + 2)) * zoomLevel;
                }
                
                // Render the tree with animation
                if (m_huffmanTree->getRoot()) {
                    // Now we have a box specifically for the tree
                    ImVec2 treeRegion = ImGui::GetContentRegionAvail();
                    float treeAreaX = treeRegion.x;
                    float treeAreaY = treeRegion.y;
                    
                    // Draw a clear, visible rectangle to show the exact bounds of the tree area
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    
                    // Draw a more visible border for the tree visualization area
                    float borderThickness = 2.0f;
                    ImU32 borderColor = m_huffmanAnimation.decodingMode ? 
                        IM_COL32(100, 130, 200, 200) : IM_COL32(100, 200, 130, 200);
                    
                    // Draw the border using four separate lines for better clarity
                    drawList->AddRect(
                        treeAreaMin, 
                        treeAreaMax,
                        borderColor,
                        8.0f,  // Rounded corners
                        ImDrawFlags_RoundCornersAll,
                        borderThickness
                    );
                    
                    // Add a semi-transparent fill
                    drawList->AddRectFilled(
                        treeAreaMin,
                        treeAreaMax,
                        m_huffmanAnimation.decodingMode ? 
                            IM_COL32(30, 35, 45, 255) : IM_COL32(30, 35, 45, 255), // Darker blue-gray background
                        8.0f  // Rounded corners
                    );
                        
                    // Set proper rendering coordinates - center the tree in the visible area
                    float renderX = treeAreaSize.x / 2.0f + panOffset.x; 
                    float renderY = 80.0f + panOffset.y;
                    
                    // Always highlight the tree center point for reference
                    drawList->AddCircle(
                        ImVec2(treeAreaMin.x + renderX, treeAreaMin.y + renderY - 50), 
                        4.0f, 
                        m_huffmanAnimation.decodingMode ? 
                            IM_COL32(100, 150, 250, 150) : IM_COL32(150, 250, 150, 150), 
                        8, 2.0f);
                        
                    // Add coordinates label for debugging - make it more visible
                    char coordsLabel[32];
                    snprintf(coordsLabel, sizeof(coordsLabel), "Tree Root: %.0f,%.0f", renderX, renderY);
                    drawList->AddText(
                        ImVec2(treeAreaMin.x + 10, treeAreaMin.y + 5),
                        IM_COL32(180, 180, 180, 200),
                        coordsLabel);
                    
                    // Render the tree strictly within this box
                    if (m_huffmanTree->getRoot()) {
                        renderHuffmanTree(m_huffmanTree->getRoot(), renderX, renderY, xOffset, yOffset, "", true);
                    }
                }
                
                ImGui::EndChild();
                
                // Pop style changes if we're in decoding mode
                if (m_huffmanAnimation.decodingMode) {
                    ImGui::PopStyleColor();
                    ImGui::PopStyleVar();
                }
                
                // Add animation controls
                ImGui::Separator();
                if (ImGui::Button("Restart Animation")) {
                    resetHuffmanAnimation();
                }
                
                ImGui::EndTabItem();
            }
            
            // Encoding/Decoding Results Tab
            if (ImGui::BeginTabItem("Encoding/Decoding")) {
                // Display different content based on mode
                if (m_huffmanInputMode == HuffmanInputMode::ENCODED) {
                    // Decoding results
                    ImGui::Text("Encoded String:");
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputTextMultiline("##encoded_result", m_resultEncodedText, sizeof(m_resultEncodedText), ImVec2(-1, 80), ImGuiInputTextFlags_ReadOnly);
                    ImGui::PopItemWidth();
                    
                    ImGui::Text("Decoded Text:");
                    ImGui::InputTextMultiline("##decoded_result", m_resultDecodedText, sizeof(m_resultDecodedText), ImVec2(-1, 80), ImGuiInputTextFlags_ReadOnly);
                    ImGui::PopStyleColor();
                } else {
                    // Encoding results
                    ImGui::Text("Original Text:");
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::InputTextMultiline("##original_text", m_resultDecodedText, sizeof(m_resultDecodedText), ImVec2(-1, 80), ImGuiInputTextFlags_ReadOnly);
                    ImGui::PopItemWidth();
                    
                    ImGui::Text("Encoded String:");
                    ImGui::InputTextMultiline("##encoded_result", m_resultEncodedText, sizeof(m_resultEncodedText), ImVec2(-1, 80), ImGuiInputTextFlags_ReadOnly);
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
                    strncpy(m_resultEncodedText, encodedResult.c_str(), sizeof(m_resultEncodedText) - 1);
                    m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                    
                    strncpy(m_resultDecodedText, m_inputText, sizeof(m_resultDecodedText) - 1);
                    m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                    
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
                    strncpy(m_inputText, sampleText.c_str(), sizeof(m_inputText) - 1);
                    m_inputText[sizeof(m_inputText) - 1] = '\0';
                    
                    std::string encodedResult = m_huffmanTree->encode(sampleText);
                    strncpy(m_resultEncodedText, encodedResult.c_str(), sizeof(m_resultEncodedText) - 1);
                    m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                    
                    strncpy(m_resultDecodedText, sampleText.c_str(), sizeof(m_resultDecodedText) - 1);
                    m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                    
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
                    std::string decodedResult = m_huffmanTree->decode(std::string(m_encodedText));
                    strncpy(m_resultDecodedText, decodedResult.c_str(), sizeof(m_resultDecodedText) - 1);
                    m_resultDecodedText[sizeof(m_resultDecodedText) - 1] = '\0';
                    
                    strncpy(m_resultEncodedText, m_encodedText, sizeof(m_resultEncodedText) - 1);
                    m_resultEncodedText[sizeof(m_resultEncodedText) - 1] = '\0';
                    
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
        }
    }
}

void HuffmanGUI::renderHuffmanTree(const std::shared_ptr<Node>& node, float x, float y, float xOffset, float yOffset, const std::string& code, bool animateTraversal) {
    if (!node)
        return;
    
    // Get current draw list for rendering
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Adjust position to be relative to the tree box
    ImVec2 nodePos(ImGui::GetWindowPos().x + x, ImGui::GetWindowPos().y + y);
    
    // For animating nodes in order of traversal
    bool isHighlighted = false;
    bool isInPath = false;
    bool isDecoding = m_huffmanAnimation.decodingMode;
    
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
    
    // Calculate node colors and sizes
    ImU32 nodeColor;
    ImU32 nodeBorderColor = IM_COL32(100, 100, 100, 255); // Consistent border color
    float nodeSize = 40.0f;  // Increased base node size for better visibility
    
    if (isHighlighted) {
        // Bright highlight for the active node
        nodeColor = IM_COL32(255, 200, 50, 255);
        nodeSize = 45.0f; // Slightly larger for emphasis
    } else if (isInPath) {
        // Previously traversed node
        nodeColor = IM_COL32(120, 210, 140, 250);
    } else if (node->isLeaf()) {
        // Leaf nodes - use consistent green for leaves with slight variations
        int freq = node->getFrequency();
        float brightness = 0.6f + (freq % 5) * 0.08f; // Slight variation based on frequency
        
        // Use a consistent light green for leaf nodes
        nodeColor = IM_COL32(
            120 * brightness, 
            210 * brightness, 
            140 * brightness, 
            250
        );
        
        // Character 'w' in yellow as in the example image
        if (node->getCharacter() == 'w') {
            nodeColor = IM_COL32(255, 200, 50, 255); // Yellow
        }
    } else {
        // Internal nodes - use the same consistent green
        nodeColor = IM_COL32(120, 210, 140, 250);
    }
    
    // Draw the node with a white border for better clarity
    // Grey border
    drawList->AddCircleFilled(nodePos, nodeSize + 2.0f, nodeBorderColor, 16);
    // Main node color
    drawList->AddCircleFilled(nodePos, nodeSize, nodeColor, 16);
    
    // Draw node content
    char label[64];
    
    if (node->isLeaf()) {
        // Format with character and frequency
        char displayChar = node->getCharacter();
        // Handle special characters for display
        if (displayChar == '\n') snprintf(label, sizeof(label), "'\\n':%d", node->getFrequency());
        else if (displayChar == '\r') snprintf(label, sizeof(label), "'\\r':%d", node->getFrequency());
        else if (displayChar == '\t') snprintf(label, sizeof(label), "'\\t':%d", node->getFrequency());
        else if (displayChar == ' ') snprintf(label, sizeof(label), "' ':%d", node->getFrequency());
        else snprintf(label, sizeof(label), "'%c':%d", displayChar, node->getFrequency());
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
    
    // Draw node text
    drawList->AddText(textPos, IM_COL32(0, 0, 0, 255), label);
    
    // Count the number of nodes in each subtree for better balancing
    int leftCount = 0, rightCount = 0;
    countSubtreeNodes(node->getLeft(), leftCount);
    countSubtreeNodes(node->getRight(), rightCount);
    
    // Recursively draw child nodes with improved spacing
    if (node->getLeft() || node->getRight()) {
        // Increase vertical spacing between levels for clarity
        float childY = y + yOffset * 1.2f;
        
        // Calculate better horizontal separation
        float totalNodes = leftCount + rightCount;
        float separationFactor = 1.5f + (code.length() * 0.3f); // Increase separation with depth
        float baseOffset = 200.0f / (code.length() + 2.0f); // Base horizontal spacing
        
        // Calculate appropriate offsets for left and right children
        float leftX = x - baseOffset * separationFactor * (leftCount > 0 ? 1.0f : 0.5f);
        float rightX = x + baseOffset * separationFactor * (rightCount > 0 ? 1.0f : 0.5f);
        
        // Adjust horizontal spacing based on tree depth and balance
        if (code.length() < 2) {
            // For top levels, use wider spacing
            leftX = x - baseOffset * 2.5f;
            rightX = x + baseOffset * 2.5f;
        }
        
        // Draw edges to children with improved visibility
        if (node->getLeft()) {
            ImVec2 leftPos(ImGui::GetWindowPos().x + leftX, ImGui::GetWindowPos().y + childY);
            
            // Grey edges for consistency with the reference image
            ImU32 edgeColor = IM_COL32(140, 140, 140, 220);
            float edgeThickness = 2.5f;
            
            // Draw edge
            drawList->AddLine(nodePos, leftPos, edgeColor, edgeThickness);
            
            // Calculate midpoint for edge label
            ImVec2 midPoint(
                (nodePos.x + leftPos.x) * 0.5f,
                (nodePos.y + leftPos.y) * 0.5f
            );
            
            // Draw label circle (white with grey border)
            drawList->AddCircleFilled(midPoint, 15.0f, IM_COL32(220, 220, 220, 255), 12);
            drawList->AddCircle(midPoint, 15.0f, IM_COL32(120, 120, 120, 200), 12, 1.5f);
            
            // Draw the '0' label
            ImVec2 labelSize = ImGui::CalcTextSize("0");
            ImVec2 labelPos(
                midPoint.x - labelSize.x * 0.5f,
                midPoint.y - labelSize.y * 0.5f
            );
            
            drawList->AddText(labelPos, IM_COL32(0, 0, 0, 255), "0");
            
            // Recursively draw left subtree
            renderHuffmanTree(node->getLeft(), leftX, childY, xOffset, yOffset, code + "0", animateTraversal);
        }
        
        if (node->getRight()) {
            ImVec2 rightPos(ImGui::GetWindowPos().x + rightX, ImGui::GetWindowPos().y + childY);
            
            // Grey edges for consistency
            ImU32 edgeColor = IM_COL32(140, 140, 140, 220);
            float edgeThickness = 2.5f;
            
            // Draw edge
            drawList->AddLine(nodePos, rightPos, edgeColor, edgeThickness);
            
            // Calculate midpoint for edge label
            ImVec2 midPoint(
                (nodePos.x + rightPos.x) * 0.5f,
                (nodePos.y + rightPos.y) * 0.5f
            );
            
            // Draw label circle (white with grey border)
            drawList->AddCircleFilled(midPoint, 15.0f, IM_COL32(220, 220, 220, 255), 12);
            drawList->AddCircle(midPoint, 15.0f, IM_COL32(120, 120, 120, 200), 12, 1.5f);
            
            // Draw the '1' label
            ImVec2 labelSize = ImGui::CalcTextSize("1");
            ImVec2 labelPos(
                midPoint.x - labelSize.x * 0.5f,
                midPoint.y - labelSize.y * 0.5f
            );
            
            drawList->AddText(labelPos, IM_COL32(0, 0, 0, 255), "1");
            
            // Recursively draw right subtree
            renderHuffmanTree(node->getRight(), rightX, childY, xOffset, yOffset, code + "1", animateTraversal);
        }
    }
} 