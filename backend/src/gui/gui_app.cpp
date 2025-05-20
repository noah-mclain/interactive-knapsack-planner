#include "../../include/gui/gui_app.hpp"
#include "../../include/gui/imgui_sfml_bridge.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

GuiApp::GuiApp(sf::RenderWindow& window)
    : m_window(window)
    , m_shouldClose(false)
    , m_currentTab(0)
    , m_knapsackGUI()
    , m_huffmanGUI()
{
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
    // No need for manual cleanup, component classes handle their own cleanup
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
    
    // Update Huffman animation if we're on that tab
    if (m_currentTab == 1) {
        m_huffmanGUI.update(dt.asSeconds());
    }
}

void GuiApp::render()
{
    // Disable ImGui demo window and metrics which might be creating Debug##Default window
    ImGui::SetCurrentContext(ImGui::GetCurrentContext());
    ImGuiIO& io = ImGui::GetIO();
    // Make sure ImGui doesn't create any auxiliary windows
    io.ConfigFlags = ImGuiConfigFlags_None;
    
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
                m_knapsackGUI.render();
                ImGui::EndTabItem();
            }
            
            if (ImGui::BeginTabItem("Huffman Encoding")) {
                m_currentTab = 1;
                m_huffmanGUI.render();
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