#pragma once

#include <string>
#include <memory>
#include <vector>
#include "OGLRenderer.h"
#include "MeshRender.h"

class Terrain : public MeshRender{
private:
	int width, height;
	std::shared_ptr<Mesh> terrainMesh;
public:
	void AddInstance(std::shared_ptr<Component> component);
	static std::vector<std::weak_ptr<Terrain>> terrains;

	static const std::string Type;
	std::string GetType() override { return Type; }

	Terrain() = default;
	Terrain(const std::string& name, const float& stretchFlat, const float& stretchHeight, const float& texScale);

	~Terrain() = default;

	Vector3 GetHeight(int x, int z) const;
};