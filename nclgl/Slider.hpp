#pragma once
#include <string>
#include <any>

class Slider {
	friend class Panel;
public:
	Slider() = default;
	Slider(const std::string& in_name) :name(in_name) {}

	void SetSlider(float* in_target, float in_min, float in_max) {
		target = in_target;
		min = in_min;
		max = in_max;
	}
private:
	std::string name;

	float* target = nullptr;
	float min = 0.0f;
	float max = 0.0f;
};
