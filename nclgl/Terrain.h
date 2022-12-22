#pragma once

#include <string>
#include <memory>
#include <vector>
#include "OGLRenderer.h"
#include "MeshRender.h"

struct Prefab;
class Terrain : public MeshRender{
private:
	int width, height;
public:
	void AddInstance(std::shared_ptr<Component> component);
	static std::vector<std::weak_ptr<Terrain>> terrains;

	static const std::string Type;
	std::string GetType() override { return Type; }

	Terrain() = default;
	Terrain(const std::string& name, const float& stretchFlat, const float& stretchHeight, const float& texScale);

	~Terrain() = default;

	Vector3 GetHeight(const unsigned int& x, const unsigned int& z) const;
};