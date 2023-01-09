#include "UI.h"
#include "Panel.h"
#include "Win32Window.h"

void UI::initialization() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    Win32Window* window32 = (Win32Window*)Window::GetWindow();
    ImGui_ImplWin32_Init(window32->GetHandle());
    ImGui_ImplOpenGL3_Init("#version 400");

    ImGui::StyleColorsClassic();
}

void UI::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void UI::Render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    for (auto panel : Panel::panels) {
        panel.lock()->Update();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}