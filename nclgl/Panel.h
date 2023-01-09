#pragma once
#include <string>
#include <unordered_map>
#include "Component.h"
#include "Vector2.h"
#include "UI.h"
#include "Text.hpp"
#include "Slider.hpp"

class Panel :public Component{
public:
	void AddInstance(std::shared_ptr<Component> component);
	static std::vector<std::weak_ptr<Panel>> panels;

	static const std::string Type;
	std::string GetType() override { return Type; }

	Panel(const std::string& in_name, Vector2 in_size, Vector2 in_position);
	void Update();
	
	void AddText(const std::string& name) {
		Text temp;
		texts.insert(std::pair(name, temp));
	}

	template <typename T>
	void SetText(const std::string& name, const T& textContent) {
		if (texts.find(name) != texts.end()) {
			texts[name].SetContent(textContent);
		}
	}

	void AddSlider(const std::string& name) {
		Slider temp(name);
		sliders.insert(std::pair(name, temp));
	}

	void SetSlider(const std::string& name, float* in_target, float in_min, float in_max) {
		if (sliders.find(name) != sliders.end()) {
			sliders[name].SetSlider(in_target, in_min, in_max);
		}
	}
private:
	Vector2 size;
	Vector2 position;
	std::string name;

	std::unordered_map<std::string, Text> texts;
	std::unordered_map<std::string, Slider> sliders;
};