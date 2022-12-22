#pragma once
#include <memory>
#include <string>
#include <vector>

class GameObject;

class Component {
public:
	GameObject* gameObject;
	virtual std::string GetType() = 0;
	virtual void AddInstance(std::shared_ptr<Component> component) = 0;

	Component();
	~Component();
	Component(const Component& copy);
	void operator=(const Component& copy);
};