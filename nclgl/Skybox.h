#pragma once
#include <array>
#include "Component.h"
#include "Texture.h"

class Skybox :public Component {
public:
	void AddInstance(std::shared_ptr<Component> component) {}

	static const std::string Type;
	std::string GetType() override { return Type; }

	Skybox() = delete;
	Skybox(const std::string& fileName);
	Skybox(std::shared_ptr<TextureCube> cubeMap);

	std::weak_ptr<TextureCube> skyboxTexture;
};