#include "Panel.h"

const std::string Panel::Type = "Panel";
std::vector<std::weak_ptr<Panel>> Panel::panels;

void Panel::AddInstance(std::shared_ptr<Component> component) {
    panels.emplace_back(std::dynamic_pointer_cast<Panel>(component));
}

Panel::Panel(const std::string& in_name, Vector2 in_size, Vector2 in_position)
    :size(in_size), position(in_position), name(in_name) {}

void Panel::Update() {
    const ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(position.x, position.y));
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y));

    if (!ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings)) {
        ImGui::End();
        throw(name + " menu initialize fault.");
    }

    for (auto text : texts) {
        ImGui::Text(text.second.GetContent().c_str());
    }

    for (auto slider : sliders) {
        ImGui::SliderFloat(slider.second.name.c_str(), slider.second.target, slider.second.min, slider.second.max);
    }

    ImGui::End();
}