#pragma once
#include <vector>
#include <string>
#include "Component.h"

class BaseScript :public Component {
public:
	void AddInstance(std::shared_ptr<Component> component);
	static std::vector<std::weak_ptr<BaseScript>> scripts;

	virtual void Start() = 0;
	virtual void Update() = 0;
};