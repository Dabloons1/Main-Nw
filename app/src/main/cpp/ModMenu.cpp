#include "ModMenu.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "Icon.h"
#include "Font.h"
#include "Hooks.h"
#include <ctime>
#include <cstdio>
#include <vector>
#include <dlfcn.h>
#include "Includes.h"
#include "Il2Cpp.h"
#include "dobby.h"

// Global state
bool IsMenuInitialized = false;
bool IsMenuVisible = true;

// Feature toggles (add your own!)
bool IsUnlimitedDiamonds = false;
bool IsGodMode = false;
bool IsUnlimitedAmmo = false;
bool IsESPEnabled = false;
float ESPWidth = 3.4f;

// Additional dummy toggles
bool Dummytoggle2 = false;
bool Dummytoggle3 = false;
bool Dummytoggle4 = false;
bool Dummytoggle5 = false;
bool Dummytoggle6 = false;
bool Dummytoggle7 = false;
bool Dummytoggle8 = false;
bool Dummytoggle9 = false;
bool Dummytoggle10 = false;
bool Dummytoggle11 = false;
bool Dummytoggle12 = false;
bool Dummytoggle13 = false;
bool Dummytoggle14 = false;

// G_IL2CPP is defined in Il2Cpp.cpp, just declare it here
extern uintptr_t G_IL2CPP;

// Helper: Rainbow color animation
ImVec4 RainbowColor(float t) {
    float r = sinf(t * 6.283185f) * 0.5f + 0.5f;
    float g = sinf(t * 6.283185f + 2.094f) * 0.5f + 0.5f;
    float b = sinf(t * 6.283185f + 4.188f) * 0.5f + 0.5f;
    return ImVec4(r, g, b, 1.0f);
}

// Animated net background
void DrawNetBackground(const ImVec2& window_pos, const ImVec2& window_size, float time) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const float grid_spacing = 40.0f;
    const float amplitude = 5.0f;
    const int cols = static_cast<int>(window_size.x / grid_spacing) + 2;
    const int rows = static_cast<int>(window_size.y / grid_spacing) + 2;

    ImU32 line_color = ImGui::GetColorU32(ImVec4(0.0f, 0.6f, 0.0f, 0.6f));
    ImU32 point_color = ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 0.8f));

    std::vector<ImVec2> points(rows * cols);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float x = window_pos.x + col * grid_spacing;
            float y = window_pos.y + row * grid_spacing;
            float offset = sinf(time * 3.0f + (col + row) * 0.5f) * amplitude;
            points[row * cols + col] = ImVec2(x, y + offset);
        }
    }

    // Draw horizontal lines
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols - 1; ++col) {
            draw_list->AddLine(points[row * cols + col], points[row * cols + col + 1], line_color, 1.0f);
        }
    }

    // Draw vertical lines
    for (int col = 0; col < cols; ++col) {
        for (int row = 0; row < rows - 1; ++row) {
            draw_list->AddLine(points[row * cols + col], points[(row + 1) * cols + col], line_color, 1.0f);
        }
    }

    // Draw points
    for (const auto& pt : points) {
        draw_list->AddCircleFilled(pt, 2.0f, point_color);
    }
}

// Setup ImGui with dark theme
void SetupImGui() {
    if (IsMenuInitialized) {
        LOGI("SetupImGui called but already initialized");
        return;
    }
    
    LOGI("=== SETTING UP IMGUI ===");
    LOGI("Creating ImGui context...");
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    
    // Get screen dimensions from Unity
    int(*get_width)(void*) = nullptr;
    int(*get_height)(void*) = nullptr;
    
    if (G_IL2CPP) {
        get_width = (int (*)(void*)) (Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), 
                                                            OBFUSCATE("UnityEngine"), 
                                                            OBFUSCATE("Screen"), 
                                                            OBFUSCATE("get_width"), 0));
        get_height = (int (*)(void*)) (Il2CppGetMethodOffset(OBFUSCATE("UnityEngine.dll"), 
                                                              OBFUSCATE("UnityEngine"), 
                                                              OBFUSCATE("Screen"), 
                                                              OBFUSCATE("get_height"), 0));
        if (get_width && get_height) {
            io.DisplaySize = ImVec2((float)get_width(0), (float)get_height(0));
            LOGI("Display size: %.0fx%.0f", io.DisplaySize.x, io.DisplaySize.y);
        }
    }
    
    // Fallback display size
    if (io.DisplaySize.x == 0 || io.DisplaySize.y == 0) {
        io.DisplaySize = ImVec2(1080, 1920);
        LOGI("Using fallback display size");
    }

    // Setup dark theme
    ImGui::StyleColorsDark();
    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowTitleAlign = ImVec2(0.5, 0.5);
    style->PopupRounding = 3;
    style->WindowPadding = ImVec2(4, 4);
    style->FramePadding  = ImVec2(2, 2);
    style->ItemSpacing   = ImVec2(2, 2);
    style->ScrollbarSize = 17;
    style->WindowBorderSize = 3;
    style->ChildBorderSize  = 1;
    style->PopupBorderSize  = 1;
    style->FrameBorderSize  = 1;
    style->WindowRounding    = 3;
    style->ChildRounding     = 3;
    style->FrameRounding     = 3;
    style->ScrollbarRounding = 2;
    style->GrabRounding      = 3;
    ImGui::GetStyle().Alpha = 1.0f;

    // Custom colors (dark green/blue theme)
    style->Colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style->Colors[ImGuiCol_WindowBg]               = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    style->Colors[ImGuiCol_ChildBg]                = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style->Colors[ImGuiCol_PopupBg]                = ImVec4(0.15f, 0.15f, 0.15f, 0.95f);
    style->Colors[ImGuiCol_Border]                 = ImVec4(0.25f, 0.25f, 0.25f, 0.60f);
    style->Colors[ImGuiCol_FrameBg]                = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style->Colors[ImGuiCol_TitleBg]                = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    style->Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.12f, 0.12f, 0.12f, 0.39f);
    style->Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.33f, 0.47f, 0.64f, 0.50f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.33f, 0.47f, 0.64f, 0.70f);
    style->Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.33f, 0.47f, 0.64f, 1.00f);
    style->Colors[ImGuiCol_CheckMark]              = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    style->Colors[ImGuiCol_SliderGrab]             = ImVec4(0.33f, 0.47f, 0.64f, 1.00f);
    style->Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.33f, 0.47f, 0.64f, 1.00f);
    style->Colors[ImGuiCol_Button]                 = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive]           = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style->Colors[ImGuiCol_Header]                 = ImVec4(0.25f, 0.35f, 0.50f, 0.60f);
    style->Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.33f, 0.47f, 0.64f, 0.80f);
    style->Colors[ImGuiCol_HeaderActive]           = ImVec4(0.33f, 0.47f, 0.64f, 1.00f);
    style->Colors[ImGuiCol_Separator]              = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);

    ImGui_ImplOpenGL3_Init("#version 100");
    
    io.IniFilename = NULL;
    
    // Setup fonts
    static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 };
    ImFontConfig icons_config;
    ImFontConfig CustomFont;
    CustomFont.FontDataOwnedByAtlas = false;

    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.OversampleH = 2.5;
    icons_config.OversampleV = 2.5;

    io.Fonts->AddFontFromMemoryTTF(const_cast<std::uint8_t*>(Custom), sizeof(Custom), 32.0f, &CustomFont);
    // Font Awesome icons disabled for now - add font_awesome_data if needed
    // io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, 32.0f, &icons_config, icons_ranges);
    
    ImFontConfig font_cfg;
    font_cfg.SizePixels = 32.0f;
    io.Fonts->AddFontDefault(&font_cfg);
    ImGui::GetStyle().ScaleAllSizes(3.0f);

    IsMenuInitialized = true;
    LOGI("=== IMGUI SETUP COMPLETE! ===");
    LOGI("Menu should now be ready to render");
}

// Main menu drawing function
void DrawModMenu() {
    if (!IsMenuInitialized) {
        LOGI("Menu not initialized yet");
        return;
    }
    
    if (!IsMenuVisible) {
        LOGI("Menu is hidden");
        return;
    }
    
    static int drawCount = 0;
    drawCount++;
    if (drawCount % 60 == 1) { // Log every 60 frames
        LOGI("=== DRAWING MOD MENU (frame %d) ===", drawCount);
    }

    const ImVec2 windowDim = ImVec2(700, 700);
    ImGui::SetNextWindowSize(windowDim, ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);

    // Test: Always show a simple window first
    if (ImGui::Begin("Test Window")) {
        ImGui::Text("Hello from ImGui!");
        ImGui::Text("Menu Visible: %s", IsMenuVisible ? "YES" : "NO");
        ImGui::Text("Menu Initialized: %s", IsMenuInitialized ? "YES" : "NO");
        if (ImGui::Button("Close Test")) {
            // Close test window
        }
    }
    ImGui::End();
    
    if (ImGui::Begin(ICON_FA_GAMEPAD " LSPosed Mod Menu")) {
        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        float time = ImGui::GetTime();
        DrawNetBackground(pos, size, time);
        
        static int activeTab = 0;
        float windowWidth = ImGui::GetContentRegionAvail().x;

        // Tab buttons
        float spacing = 8.0f;
        int tabCount = 3;
        float buttonWidth = (windowWidth - spacing * (tabCount - 1)) / tabCount;
        float buttonHeight = 48.0f;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        auto TabButton = [&](const char* label, int id) {
            ImVec2 pos = ImGui::GetCursorScreenPos();
            ImGuiStyle& style = ImGui::GetStyle();

            float oldRounding = style.FrameRounding;
            style.FrameRounding = 0.0f;

            ImVec2 size(buttonWidth, buttonHeight);

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            bool pressed = ImGui::Button(label, size);
            ImGui::PopStyleVar();

            if (pressed) activeTab = id;

            style.FrameRounding = oldRounding;

            if (activeTab == id) {
                // Draw rainbow border for active tab
                ImVec2 min = pos;
                ImVec2 max = ImVec2(pos.x + size.x, pos.y + size.y);
                float thickness = 3.0f;
                int segments = 80;

                for (int i = 0; i < segments; i++) {
                    float t0 = (float)i / segments;
                    float t1 = (float)(i + 1) / segments;
                    float shift = time * 0.5f;
                    ImVec4 col0 = RainbowColor(t0 + shift);
                    ImU32 c0 = ImGui::GetColorU32(col0);

                    ImVec2 p0, p1;
                    if (i < segments / 4) {
                        p0 = ImVec2(min.x + size.x * (t0 * 4), min.y);
                        p1 = ImVec2(min.x + size.x * (t1 * 4), min.y);
                    } else if (i < segments / 2) {
                        float tt0 = (t0 - 0.25f) * 4;
                        float tt1 = (t1 - 0.25f) * 4;
                        p0 = ImVec2(max.x, min.y + size.y * tt0);
                        p1 = ImVec2(max.x, min.y + size.y * tt1);
                    } else if (i < 3 * segments / 4) {
                        float tt0 = (t0 - 0.5f) * 4;
                        float tt1 = (t1 - 0.5f) * 4;
                        p0 = ImVec2(max.x - size.x * tt0, max.y);
                        p1 = ImVec2(max.x - size.x * tt1, max.y);
                    } else {
                        float tt0 = (t0 - 0.75f) * 4;
                        float tt1 = (t1 - 0.75f) * 4;
                        p0 = ImVec2(min.x, max.y - size.y * tt0);
                        p1 = ImVec2(min.x, max.y - size.y * tt1);
                    }

                    draw_list->AddLine(p0, p1, c0, thickness);
                }
            }
        };

        // Draw tabs
        for (int i = 0; i < tabCount; i++) {
            if (i > 0) ImGui::SameLine(0, spacing);
            if (i == 0) TabButton(ICON_FA_EYE " Visual", 0);
            else if (i == 1) TabButton(ICON_FA_BARS " Memory", 1);
            else if (i == 2) TabButton(ICON_FA_COGS " Settings", 2);
        }

        ImGui::Separator();

        float spacingChild = 8.0f;

        // Visual Tab
        if (activeTab == 0) {
            float halfWidth = (windowWidth - spacingChild) / 2.0f;

            ImGui::BeginChild("left", ImVec2(halfWidth, 0), true);
            ImGui::PushItemWidth(-1);
            
            ImGui::Checkbox("Unlimited Diamonds", &IsUnlimitedDiamonds);
            ImGui::Checkbox("Ultra Vision", &Dummytoggle2);
            ImGui::Checkbox("Ghost Trail", &Dummytoggle3);
            ImGui::Checkbox("Turbo Fill", &Dummytoggle4);
            ImGui::Checkbox("Phase Shift", &Dummytoggle5);
            ImGui::Checkbox("Stealth Mode", &Dummytoggle6);
            ImGui::Checkbox("Magic Material", &Dummytoggle7);
            
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::SameLine(0, spacingChild);

            ImGui::BeginChild("right", ImVec2(0, 0), true);
            ImGui::PushItemWidth(-1);
            ImGui::Checkbox("World", &Dummytoggle8);
            ImGui::Checkbox("Remap", &Dummytoggle9);
            ImGui::Checkbox("Night", &Dummytoggle10);
            ImGui::Separator();
            ImGui::TextDisabled("Other");

            ImGui::Checkbox("ESP", &IsESPEnabled);
            ImGui::SliderFloat("ESP Width", &ESPWidth, 0.5f, 10.0f, "%.1f");
            ImGui::PopItemWidth();
            ImGui::EndChild();
        }

        // Memory Tab
        if (activeTab == 1) {
            ImGui::BeginChild("memory_tab", ImVec2(0, 0), true);
            ImGui::PushItemWidth(-1);
            ImGui::Text("Memory Features");
            ImGui::Separator();
            ImGui::Checkbox("God Mode", &IsGodMode);
            ImGui::Checkbox("Unlimited Ammo", &IsUnlimitedAmmo);
            ImGui::Checkbox("Feature 3", &Dummytoggle11);
            ImGui::Checkbox("Feature 4", &Dummytoggle12);
            ImGui::PopItemWidth();
            ImGui::EndChild();
        }

        // Settings Tab
        if (activeTab == 2) {
            ImGui::BeginChild("settings_tab", ImVec2(0, 0), true);
            ImGui::PushItemWidth(-1);
            ImGui::Text("Settings");
            ImGui::Separator();
            ImGui::Checkbox("Dark Mode", &Dummytoggle13);
            ImGui::Checkbox("Show FPS", &Dummytoggle14);
            
            if (ImGui::Button("Hide Menu")) {
                IsMenuVisible = false;
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();
        }

        ImGui::End();
    }
}

void CleanupImGui() {
    if (IsMenuInitialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
        IsMenuInitialized = false;
        LOGI("ImGui cleaned up");
    }
}

