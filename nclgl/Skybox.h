#pragma once
#include <array>
#include "Component.h"
#include "Texture.h"

class Skybox {
public:
	Skybox() = delete;
	Skybox(const std::string& fileName);
	Skybox(std::shared_ptr<TextureCube> cubeMap);

	static std::weak_ptr<TextureCube> skyboxTexture;
};