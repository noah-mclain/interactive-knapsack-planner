#include "../include/gui/gui_app.hpp"
#include "gui/imgui_sfml_bridge.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

GuiApp::GuiApp(sf::RenderWindow& window)
    : m_window(window)
    , m_shouldClose(false)
    , m_currentTab(0)
    , m_capacity(15)  // Lower capacity within the 1-15 range
    , m_editIndex(-1)
    , m_editWeight(0)
    , m_editValue(0)
    , m_newWeight(0)
    , m_newValue(0)
    , m_useGreedy(true)
    , m_useBruteForce(true)
    , m_useDynamicProgramming(true)
    , m_useBranchAndBound(true)
    , m_hasResults(false)
{
    // Initialize with some lower default items
    m_weights = {3, 5, 7};
    m_values = {5, 10, 15};
    
    // Setup style with more appealing, less saturated colors
    ImGui::StyleColorsDark();
    ImVec4* colors = ImGui::GetStyle().Colors;
    
    // Background colors - softer, less harsh
    colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.16f, 0.21f, 0.94f); // Softer dark blue-gray
    
    // Headers - more muted tones
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.27f, 0.32f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.35f, 0.45f, 0.60f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.45f, 0.60f, 1.00f);
    
    // Buttons - more muted blue tones
    colors[ImGuiCol_Button] = ImVec4(0.30f, 0.40f, 0.55f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.45f, 0.60f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.40f, 0.55f, 1.00f);
    
    // Tabs - more muted blue tones
    colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.30f, 0.40f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.45f, 0.60f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.30f, 0.40f, 0.50f, 1.00f);
    
    // Text colors - improved readability
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    
    // Style adjustments for modern look
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;     // Rounded corners on widgets
    style.GrabRounding = 3.0f;      // Rounded sliders
    style.WindowRounding = 5.0f;    // Rounded window corners
    style.ScrollbarRounding = 5.0f; // Rounded scrollbars
    style.TabRounding = 5.0f;       // Rounded tabs
    style.FramePadding = ImVec2(8, 4);  // More padding in widgets
    style.ItemSpacing = ImVec2(10, 8);  // More space between items
}

GuiApp::~GuiApp()
{
    // Cleanup any dynamically allocated memory in results
    for (auto& result : m_results) {
        if (result.algorithm_name == "Dynamic Programming" && result.algorithm_data) {
            // Free the DP table
            delete static_cast<std::vector<std::vector<int>>*>(result.algorithm_data);
            result.algorithm_data = nullptr;
        }
    }
}

void GuiApp::processEvents(const sf::Event& event)
{
    ImGuiSFMLBridge::ProcessEvent(event);
    
    // Check if window close was requested
    if (event.is<sf::Event::Closed>()) {
        m_shouldClose = true;
    }
}

void GuiApp::update(sf::Time dt)
{
    ImGuiSFMLBridge::Update(m_window, dt);
}

void GuiApp::render()
{
    // Main window covering the entire screen - make it fixed
    sf::Vector2u windowSize = m_window.getSize();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
    
    if (ImGui::Begin("Interactive Algorithm Visualizer", nullptr, 
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | 
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
        
        // Menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                    m_shouldClose = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        // Tabs for different algorithms
        if (ImGui::BeginTabBar("AlgorithmTabs", ImGuiTabBarFlags_None)) {
            if (ImGui::BeginTabItem("Knapsack Problem")) {
                m_currentTab = 0;
                renderKnapsackTab();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Huffman Encoding")) {
                m_currentTab = 1;
                renderHuffmanTab();
                ImGui::EndTabItem();
            }
            
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
    
    ImGuiSFMLBridge::Render(m_window);
}

bool GuiApp::shouldClose() const
{
    return m_shouldClose;
}

void GuiApp::renderKnapsackTab()
{
    // Knapsack capacity
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.8f);
    ImGui::SliderInt("Knapsack Capacity", &m_capacity, 1, 500);
    ImGui::PopItemWidth();
    
    // Algorithm selection
    ImGui::Text("Select algorithms to use:");
    ImGui::Columns(4, "algorithm_columns", false);
    ImGui::Checkbox("Greedy", &m_useGreedy); ImGui::NextColumn();
    ImGui::Checkbox("Brute Force", &m_useBruteForce); ImGui::NextColumn();
    ImGui::Checkbox("Dynamic Programming", &m_useDynamicProgramming); ImGui::NextColumn();
    ImGui::Checkbox("Branch and Bound", &m_useBranchAndBound);
    ImGui::Columns(1);
    ImGui::Separator();
    
    // Items section
    ImGui::Text("Items");
    
    // Table for items
    if (ImGui::BeginTable("items_table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Weight", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        
        for (std::size_t i = 0; i < m_weights.size(); i++) {
            ImGui::TableNextRow();
            
            // Item ID
            ImGui::TableNextColumn();
            ImGui::Text("%zu", i);
            
            // Weight
            ImGui::TableNextColumn();
            ImGui::Text("%d", m_weights[i]);
            
            // Value
            ImGui::TableNextColumn();
            ImGui::Text("%d", m_values[i]);
            
            // Actions
            ImGui::TableNextColumn();
            ImGui::PushID(static_cast<int>(i));
            
            // Edit button for this item
            if (ImGui::Button("Edit")) {
                m_editIndex = static_cast<int>(i);
                m_editWeight = m_weights[i];
                m_editValue = m_values[i];
                ImGui::OpenPopup("EditItemPopup");
            }
            
            ImGui::SameLine();
            
            // Delete button for this item
            if (ImGui::Button("Delete")) {
                m_weights.erase(m_weights.begin() + i);
                m_values.erase(m_values.begin() + i);
            }
            
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    
    // Add item section
    ImGui::Separator();
    ImGui::Text("Add New Item");
    
    ImGui::PushItemWidth(150);
    ImGui::InputInt("New Weight", &m_newWeight);
    ImGui::SameLine();
    ImGui::InputInt("New Value", &m_newValue);
    ImGui::PopItemWidth();
    
    if (ImGui::Button("Add Item")) {
        if (m_newWeight > 0) {
            m_weights.push_back(m_newWeight);
            m_values.push_back(m_newValue);
            m_newWeight = 0;
            m_newValue = 0;
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Clear All Items")) {
        ImGui::OpenPopup("ClearConfirmPopup");
    }
    
    // Confirmation popup for clearing items
    if (ImGui::BeginPopupModal("ClearConfirmPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to clear all items?");
        ImGui::Text("This action cannot be undone.");
        ImGui::Separator();
        
        if (ImGui::Button("Yes", ImVec2(120, 0))) {
            m_weights.clear();
            m_values.clear();
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("No", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
    
    ImGui::SameLine();
    
    // Solve button with color
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    
    if (ImGui::Button("Solve", ImVec2(120, 0))) {
        solveKnapsack();
    }
    
    ImGui::PopStyleColor(3);
    
    // Display results if available
    if (m_hasResults && !m_results.empty()) {
        renderResults();
    }
    
    // Edit item popup
    if (ImGui::BeginPopupModal("EditItemPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Edit Item %d", m_editIndex + 1);
        ImGui::Separator();
        
        ImGui::InputInt("Edit Weight", &m_editWeight);
        ImGui::InputInt("Edit Value", &m_editValue);
        
        if (ImGui::Button("Save", ImVec2(120, 0))) {
            if (m_editWeight > 0 && m_editIndex >= 0 && m_editIndex < static_cast<int>(m_weights.size())) {
                m_weights[m_editIndex] = m_editWeight;
                m_values[m_editIndex] = m_editValue;
                m_editIndex = -1;
                ImGui::CloseCurrentPopup();
            }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_editIndex = -1;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void GuiApp::renderHuffmanTab()
{
    ImGui::Text("Huffman Encoding UI will be implemented in the future.");
    ImGui::Text("Please use the Knapsack Problem tab for now.");
}

void GuiApp::solveKnapsack()
{
    if (m_weights.size() > 0 && (m_useGreedy || m_useBruteForce || m_useDynamicProgramming || m_useBranchAndBound)) {
        // Free memory from previous results
        for (auto& result : m_results) {
            if (result.algorithm_name == "Dynamic Programming" && result.algorithm_data) {
                delete static_cast<std::vector<std::vector<int>>*>(result.algorithm_data);
                result.algorithm_data = nullptr;
            }
        }
        
        m_results.clear();
        
        // Create boolean array for algorithms
        bool algorithms[4] = {m_useGreedy, m_useBruteForce, m_useDynamicProgramming, m_useBranchAndBound};
        
        // Initialize solver
        KnapsackSolver solver(m_weights.size(), m_weights, m_values, m_capacity, algorithms);
        
        // Solve problem
        solver.solve();
        
        // Get results from solver
        m_results = solver.get_results();
        m_hasResults = !m_results.empty();
        
        // Reset animation state for DP visualization
        if (m_useDynamicProgramming) {
            // We'll use this flag in renderVisualRepresentation to reset animation
            static bool resetDPAnimation = true;
        }
    }
}

void GuiApp::renderResults()
{
    ImGui::Separator();
    ImGui::Text("Results");
    
    // Results table
    if (ImGui::BeginTable("results_table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Algorithm", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Max Profit", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Execution Time", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Selected Items", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        
        for (const auto& result : m_results) {
            ImGui::TableNextRow();
            
            // Algorithm name
            ImGui::TableNextColumn();
            ImGui::Text("%s", result.algorithm_name.c_str());
            
            // Max profit
            ImGui::TableNextColumn();
            ImGui::Text("%.2f", result.max_profit);
            
            // Execution time
            ImGui::TableNextColumn();
            ImGui::Text("%f ms", result.execution_time_ms);
            
            // Selected items
            ImGui::TableNextColumn();
            std::string selected_items_str;
            for (size_t i = 0; i < result.selected_items.size(); ++i) {
                selected_items_str += std::to_string(result.selected_items[i]);
                if (i < result.selected_items.size() - 1) {
                    selected_items_str += ", ";
                }
            }
            ImGui::Text("%s", selected_items_str.c_str());
        }
        
        ImGui::EndTable();
    }
    
    // Visual representation of the knapsack
    ImGui::Separator();
    ImGui::Text("Visual Representation");
    
    if (ImGui::BeginTabBar("ResultTabs")) {
        for (const auto& result : m_results) {
            if (ImGui::BeginTabItem(result.algorithm_name.c_str())) {
                renderVisualRepresentation(result);
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
}

void GuiApp::renderVisualRepresentation(const SolverResult& result)
{
    // Get available content region width
    float availableWidth = ImGui::GetContentRegionAvail().x;
    
    // Set up the knapsack visualization
    ImGui::Text("Visual Representation:");
    
    // Calculate the space needed for all items
    float totalItemWidth = 0;
    float spacing = 2.0f;  // Increased from 5.0f for more padding
    float minItemWidth = 70.0f; // Minimum width to ensure labels are visible
    
    for (int item_id : result.selected_items) {
        if (item_id >= 0 && item_id < static_cast<int>(m_weights.size())) {
            // Calculate item width proportional to its weight, with minimum size
            float itemWidth = (m_weights[item_id] / static_cast<float>(m_capacity)) * 
                              (availableWidth * 0.8f); // Use 80% of available width for proportional calculation
            itemWidth = std::max(itemWidth, minItemWidth);
            totalItemWidth += itemWidth + spacing;
        }
    }
    
    // Add padding to total width
    totalItemWidth += spacing * 2;
    
    // Determine if scrolling is needed - be more conservative to avoid unnecessary scrolling
    bool needsScrolling = totalItemWidth > (availableWidth - 20.0f);
    float childHeight = 100.0f; // Height for the knapsack visualization
    
    // For consistent look with tables, use full available width (minus small margin)
    float tableWidth = availableWidth * 0.98f;
    
    // Use ImGui table for consistent styling with other tables
    if (ImGui::BeginTable("knapsack_visual", 1, 
                         ImGuiTableFlags_Borders | 
                         ImGuiTableFlags_NoHostExtendX |
                         (needsScrolling ? ImGuiTableFlags_ScrollX : 0))) {
        
        ImGui::TableSetupColumn("Items", ImGuiTableColumnFlags_WidthStretch);
        
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        
        // Calculate space inside table cell
        float cellWidth = ImGui::GetContentRegionAvail().x;
        
        // Draw items with consistent spacing
        float current_x = ImGui::GetCursorScreenPos().x + spacing * 2;  // Double spacing from edge
        float total_available_width = cellWidth - (spacing * 4);  // Adjusted for increased padding
        
        // Scale factors for drawing
        float y_pos = ImGui::GetCursorScreenPos().y + spacing * 2;  // Double spacing from top
        float itemHeight = childHeight - (spacing * 6);  // Adjusted to account for increased spacing
        
        for (int item_id : result.selected_items) {
            if (item_id >= 0 && item_id < static_cast<int>(m_weights.size())) {
                // Calculate item width proportional to its weight
                float itemRatio = static_cast<float>(m_weights[item_id]) / static_cast<float>(m_capacity);
                float item_width = std::max(total_available_width * itemRatio, minItemWidth);
                
                // Generate a more pleasing, less saturated color based on the value
                float hue = (m_values[item_id] % 360) / 360.0f;
                ImVec4 itemColor = ImVec4(
                    ImColor::HSV(hue, 0.5f, 0.9f));
                
                // Draw the colored rectangle for the item
                ImGui::GetWindowDrawList()->AddRectFilled(
                    ImVec2(current_x, y_pos),
                    ImVec2(current_x + item_width, y_pos + itemHeight),
                    ImGui::ColorConvertFloat4ToU32(itemColor),
                    5.0f);
                
                // Draw item text
                char label[32];
                snprintf(label, sizeof(label), "#%d\nW:%d\nV:%d", 
                        item_id, m_weights[item_id], m_values[item_id]);
                
                ImGui::GetWindowDrawList()->AddText(
                    ImVec2(current_x + 10, y_pos + 10),  // Increased padding for text
                    IM_COL32(0, 0, 0, 255), // Black text
                    label);
                
                current_x += item_width + spacing;
            }
        }
        
        // Add extra space to make sure everything is visible
        ImGui::Dummy(ImVec2(needsScrolling ? totalItemWidth : cellWidth, itemHeight + spacing * 4));  // Increased spacing
        
        ImGui::EndTable();
    }
    
    // Add DP table visualization for Dynamic Programming algorithm
    if (result.algorithm_name == "Dynamic Programming") {
        ImGui::Separator();
        ImGui::Text("Dynamic Programming Table");
        
        // Set up table animation state (static to persist between frames)
        static float animTimer = 0.0f;
        static int animRow = 0;
        static int animCol = 0;
        static bool animComplete = false;
        static bool isFirstRender = true;
        
        // Backtracking animation state
        static float backtrackAnimTimer = 0.0f;
        static int backtrackAnimStep = -1; // -1 means not started yet
        static std::vector<std::pair<int, int>> backtrackCells;
        
        // Reset animation when switching to this tab
        if (isFirstRender) {
            animTimer = 0.0f;
            animRow = 0;
            animCol = 0;
            animComplete = false;
            backtrackAnimStep = -1;
            backtrackCells.clear();
            isFirstRender = false;
        }
        
        // Access the stored DP table if available
        const std::vector<std::vector<int>>* dp_table = 
            static_cast<const std::vector<std::vector<int>>*>(result.algorithm_data);
        
        // Get window width to ensure table is not too small
        float availableWidth = ImGui::GetContentRegionAvail().x + 20;
        float tableHeight = (m_weights.size() + 2) * ImGui::GetTextLineHeightWithSpacing() + 20;  // Increased height
        
        // Use 98% of the window width (matching other tables)
        float tableWidth = availableWidth * 0.98f;
        
        // Minimum comfortable widths for readability
        float minFirstColumnWidth = 120.0f;  // Minimum width for first column
        float minDataColumnWidth = 45.0f;    // Minimum width for data columns
        int totalColumns = m_capacity + 2;   // First column + capacity + 1
        
        // Calculate dynamic column widths based on available space
        float firstColumnRatio = 1.8f;       // First column is wider than data columns
        float totalRatioUnits = (totalColumns - 1) + firstColumnRatio;
        float unitWidth = tableWidth / totalRatioUnits;
        
        float calculatedFirstColumnWidth = unitWidth * firstColumnRatio;
        float calculatedDataColumnWidth = unitWidth;
        
        // Determine if the calculated widths are too small and scrolling is needed
        bool needsScrolling = (calculatedDataColumnWidth < minDataColumnWidth) || 
                             (calculatedFirstColumnWidth < minFirstColumnWidth);
        
        float firstColumnWidth, columnWidth;
        
        if (needsScrolling) {
            // If calculated widths are too small, use minimum comfortable widths
            firstColumnWidth = minFirstColumnWidth;
            columnWidth = minDataColumnWidth;
        } else {
            // Use the calculated widths that fit in the available space
            firstColumnWidth = calculatedFirstColumnWidth;
            columnWidth = calculatedDataColumnWidth;
        }
        
        // Remove any default padding that might reduce the effective width
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6, 3));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        
        // Create a child window with fixed width to match other tables
        ImGui::BeginChild("DPTableScroll", ImVec2(tableWidth, tableHeight), true, 
                         needsScrolling ? ImGuiWindowFlags_HorizontalScrollbar : ImGuiWindowFlags_None);
        
        // Set table flags - only include ScrollX if we need scrolling
        ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | 
                                     ImGuiTableFlags_RowBg | 
                                     ImGuiTableFlags_SizingFixedFit;
        if (needsScrolling) {
            tableFlags |= ImGuiTableFlags_ScrollX;
        }
        
        // Force all columns to maintain exactly equal width, regardless of content
        tableFlags |= ImGuiTableFlags_SizingStretchSame | 
                     ImGuiTableFlags_NoKeepColumnsVisible |
                     ImGuiTableFlags_SizingStretchProp;
        
        // Force table to fill the entire available width
        if (ImGui::BeginTable("dp_table", totalColumns, tableFlags, ImVec2(tableWidth, 0))) {
            
            // Setup columns with appropriate widths
            ImGui::TableSetupColumn("i\\W", ImGuiTableColumnFlags_WidthFixed, firstColumnWidth);
            
            // Use stretch for all data columns to ensure they fill all available space
            for (int w = 0; w <= m_capacity; w++) {
                char label[8];
                snprintf(label, sizeof(label), "%d", w);
                ImGui::TableSetupColumn(label, ImGuiTableColumnFlags_WidthStretch, 1.5f);
            }
            ImGui::TableHeadersRow();
            
            // Update animation
            if (!animComplete && ImGui::IsWindowFocused()) {
                animTimer += ImGui::GetIO().DeltaTime * 5.0; // Speed factor
                if (animTimer >= 1.0f) {
                    animTimer = 0.0f;
                    animCol++;
                    if (animCol > m_capacity) {
                        animCol = 0;
                        animRow++;
                        if (animRow > static_cast<int>(m_weights.size())) {
                            animComplete = true;
                            // Start backtracking animation once forward fill is complete
                            backtrackAnimStep = 0;
                        }
                    }
                }
            }
            
            // If we don't have the stored DP table, reconstruct it
            std::vector<std::vector<int>> reconstructed_dp;
            if (!dp_table) {
                // Reconstruct the DP table (simplified version)
                reconstructed_dp.resize(m_weights.size() + 1, std::vector<int>(m_capacity + 1, 0));
                
                // Fill in DP table (simple implementation for visualization)
                for (int i = 1; i <= static_cast<int>(m_weights.size()); i++) {
                    for (int w = 0; w <= m_capacity; w++) {
                        if (m_weights[i-1] <= w) {
                            reconstructed_dp[i][w] = std::max(reconstructed_dp[i-1][w], 
                                reconstructed_dp[i-1][w - m_weights[i-1]] + m_values[i-1]);
                        } else {
                            reconstructed_dp[i][w] = reconstructed_dp[i-1][w];
                        }
                    }
                }
            }
            
            // Use either the stored or reconstructed DP table
            const std::vector<std::vector<int>>& dp = dp_table ? *dp_table : reconstructed_dp;
            
            // Find the backtracking path to highlight chosen items in order
            if (backtrackCells.empty() && !dp.empty() && !dp[0].empty()) {
                int w = m_capacity;
                for (int i = static_cast<int>(m_weights.size()); i > 0; i--) {
                    if (dp[i][w] != dp[i-1][w]) {
                        // This item was chosen in the solution
                        backtrackCells.push_back({i, w});
                        w -= m_weights[i-1]; // Update remaining capacity
                    }
                }
            }
            
            // Check for mouse click to restart animation if it's complete
            if (animComplete && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                // Reset animation state
                animTimer = 0.0f;
                animRow = 0;
                animCol = 0;
                animComplete = false;
                backtrackAnimStep = -1;
                backtrackAnimTimer = 0.0f;
            }
            
            // Update backtracking animation
            if (animComplete && backtrackAnimStep >= 0 && backtrackAnimStep <= static_cast<int>(backtrackCells.size()) && ImGui::IsWindowFocused()) {
                backtrackAnimTimer += ImGui::GetIO().DeltaTime * 1.5; // Slower for backtracking
                if (backtrackAnimTimer >= 1.0f) {
                    backtrackAnimTimer = 0.0f;
                    backtrackAnimStep++;
                    if (backtrackAnimStep > static_cast<int>(backtrackCells.size())) {
                        // Animation complete
                    }
                }
            }
            
            // Fill table rows
            for (int i = 0; i <= static_cast<int>(m_weights.size()); i++) {
                ImGui::TableNextRow();
                
                // Item ID column
                ImGui::TableNextColumn();
                if (i == 0) {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.9f, 1.0f), "Item");
                } else {
                    if (i-1 < m_weights.size()) {
                        ImGui::Text("%d (W:%d,V:%d)", i-1, m_weights[i-1], m_values[i-1]);
                    } else {
                        ImGui::Text("%d", i-1);
                    }
                }
                
                // DP table values
                for (int w = 0; w <= m_capacity; w++) {
                    ImGui::TableNextColumn();
                    
                    // Check if this is a chosen cell (in the backtracking path)
                    bool isChosen = false;
                    int chosenIndex = -1;
                    for (size_t idx = 0; idx < backtrackCells.size(); idx++) {
                        if (backtrackCells[idx].first == i && backtrackCells[idx].second == w) {
                            isChosen = true;
                            chosenIndex = static_cast<int>(idx);
                            break;
                        }
                    }
                    
                    // Check if this cell should be animated in backtracking
                    bool isBacktrackAnimated = backtrackAnimStep > 0 && isChosen && chosenIndex < backtrackAnimStep;
                    
                    // Check if this cell should be animated in filling
                    bool isForwardAnimated = !animComplete && i <= animRow && (i < animRow || w <= animCol);
                    
                    // Currently active cell in backtracking
                    bool isActiveBacktrackCell = backtrackAnimStep > 0 && isChosen && chosenIndex == backtrackAnimStep - 1;
                    
                    // Apply colors based on cell state
                    if (isBacktrackAnimated) {
                        // Use a bright highlight color for the active backtracking cell
                        if (isActiveBacktrackCell) {
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, 
                                IM_COL32(255, 100, 100, 200)); // Bright red for active cell
                        } else {
                            // Already visited in backtracking
                            float intensity = 1.0f - (chosenIndex / static_cast<float>(backtrackCells.size()));
                            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, 
                                IM_COL32(
                                    180 + (int)(75 * intensity), 
                                    100 + (int)(40 * intensity), 
                                    100 + (int)(40 * intensity), 
                                    150 + (int)(105 * intensity)
                                ));
                        }
                        
                        // Show the value with order indicator
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Black text
                        
                        char buffer[32];
                        if (!dp.empty() && i < dp.size() && w < dp[i].size()) {
                            snprintf(buffer, sizeof(buffer), "%d [%d]", dp[i][w], chosenIndex + 1); // Show order
                            ImGui::Text("%s", buffer);
                        } else {
                            ImGui::Text("0");
                        }
                        ImGui::PopStyleColor();
                    }
                    else if (isChosen && animComplete && backtrackAnimStep <= 0) {
                        // Show final solution cells when not animating backtracking
                        float intensity = 1.0f - (chosenIndex / static_cast<float>(backtrackCells.size()));
                        ImVec4 highlightColor = ImVec4(
                            0.2f + 0.6f * intensity,  // Red component
                            0.6f + 0.3f * intensity,  // Green component
                            0.8f,                     // Blue component
                            0.7f + 0.3f * intensity   // Alpha component
                        );
                        
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, 
                            ImGui::ColorConvertFloat4ToU32(highlightColor));
                        
                        // Show the value with order indicator
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // Black text
                        
                        char buffer[32];
                        if (!dp.empty() && i < dp.size() && w < dp[i].size()) {
                            snprintf(buffer, sizeof(buffer), "%d [%d]", dp[i][w], chosenIndex + 1); // Show order
                            ImGui::Text("%s", buffer);
                        } else {
                            ImGui::Text("0");
                        }
                        ImGui::PopStyleColor();
                    }
                    else if (isForwardAnimated) {
                        // Softer highlight for forward animation
                        float animIntensity = (animRow == i && animCol == w) ? 0.7f : 0.3f;
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, 
                            IM_COL32(
                                160 + (int)(20 * animIntensity),
                                170 + (int)(30 * animIntensity),
                                190 + (int)(40 * animIntensity),
                                100 + (int)(50 * animIntensity)
                            ));
                        
                        // Display values
                        if (i == 0) {
                            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.9f, 1.0f), "%d", w);
                        } else if (!dp.empty() && i < dp.size() && w < dp[i].size()) {
                            ImGui::Text("%d", dp[i][w]);
                        } else {
                            ImGui::Text("0");
                        }
                    } else {
                        // Display values with normal styling
                        if (i == 0) {
                            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.9f, 1.0f), "%d", w);
                        } else if (!dp.empty() && i < dp.size() && w < dp[i].size()) {
                            ImGui::Text("%d", dp[i][w]);
                        } else {
                            ImGui::Text("0");
                        }
                    }
                }
            }
            
            ImGui::EndTable();
        }
        
        ImGui::EndChild(); // End the scrollable area
        ImGui::PopStyleVar(3); // Pop all the style vars we pushed
        
        // Add explanation of the DP table with backtracking
        if (backtrackAnimStep > 0 && backtrackAnimStep <= static_cast<int>(backtrackCells.size())) {
            ImGui::TextWrapped(
                "Backtracking: Step %d of %d - Selecting items in reverse order",
                backtrackAnimStep, static_cast<int>(backtrackCells.size())
            );
            
            if (backtrackAnimStep - 1 < static_cast<int>(backtrackCells.size()) && backtrackAnimStep > 0) {
                auto& cell = backtrackCells[backtrackAnimStep - 1];
                int itemIndex = cell.first - 1;
                ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), 
                    "Current step: Adding item #%d (W:%d, V:%d)",
                    itemIndex, m_weights[itemIndex], m_values[itemIndex]);
            }
        } else {
            ImGui::TextWrapped(
                "The table shows the maximum value achievable for each capacity. Highlighted cells with numbers [1], [2], etc. indicate items chosen in backtracking order."
            );
        }
        ImGui::Separator();
    }
    
    // Performance metrics
    ImGui::Text("Performance Metrics:");
    
    if (ImGui::BeginTable("perf_metrics", 2, ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("Metric", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();
        
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("Execution Time");
        ImGui::TableNextColumn(); ImGui::Text("%f ms", result.execution_time_ms);
        
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("Memory Usage");
        ImGui::TableNextColumn(); ImGui::Text("%zu bytes", result.memory_usage);
        
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("Time Complexity");
        ImGui::TableNextColumn(); 
        if (result.algorithm_name == "Greedy")
            ImGui::Text("O(n log n)");
        else if (result.algorithm_name == "Brute Force")
            ImGui::Text("O(2^n)");
        else if (result.algorithm_name == "Dynamic Programming")
            ImGui::Text("O(n*W)");
        else if (result.algorithm_name == "Branch and Bound")
            ImGui::Text("O(2^n) - worst case, often better in practice");
        
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("Space Complexity");
        ImGui::TableNextColumn(); 
        if (result.algorithm_name == "Greedy")
            ImGui::Text("O(n)");
        else if (result.algorithm_name == "Brute Force")
            ImGui::Text("O(2^n)");
        else if (result.algorithm_name == "Dynamic Programming")
            ImGui::Text("O(n*W)");
        else if (result.algorithm_name == "Branch and Bound")
            ImGui::Text("O(2^n) - worst case, often better in practice");
        
        ImGui::EndTable();
    }
} 