#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Component.h"

class Material;
class Mesh;

class MeshRender :public Component {
public:
	void AddInstance(std::shared_ptr<Component> component);
	static std::vector<std::weak_ptr<MeshRender>> meshRenders;

	static const std::string Type;
	std::string GetType() override { return Type; }

	MeshRender();
	MeshRender(const MeshRender& copy);

	std::weak_ptr<Mesh> mesh;
	std::unique_ptr<Material> material;
	std::weak_ptr<Material> shadowMaterial;

	void Render();
	void RenderShadow();

	bool castShadow = true;
};