#pragma once
#include <array>
#include "Component.h"
#include "Texture.h"

class Mesh;
class Material;

class Skybox :public Component {
	void AddInstance(std::shared_ptr<Component> component);
	static std::weak_ptr<Skybox> skyBox;

	static const std::string Type;
	std::string GetType() override { return Type; }

	Skybox();
	void Render();
private:
	Texture2D upTex;
	Texture2D downTex;
	Texture2D leftTex;
	Texture2D rightTex;
	Texture2D frontTex;
	Texture2D backTex;

	std::weak_ptr<Material> material;
	std::weak_ptr<Mesh> mesh;
};