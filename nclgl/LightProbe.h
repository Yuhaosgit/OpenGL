#pragma once
#include "Component.h"

class LightProbe :public Component {
	void AddInstance(std::shared_ptr<Component> component);
	static std::vector<std::weak_ptr<LightProbe>> lightProbes;

	static const std::string Type;
	std::string GetType() override { return Type; }
};