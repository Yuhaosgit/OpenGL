#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Component.h"

class Material;
class Mesh;

class Decal :public Component{
public:
	void AddInstance(std::shared_ptr<Component> component);
	static std::vector<std::weak_ptr<Decal>> decals;

	static const std::string Type;
	std::string GetType() override { return Type; }

	Decal();

	void Render();

	std::weak_ptr<Mesh> mesh;
	std::weak_ptr<Material> material;
};