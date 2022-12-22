#pragma once
#include <memory>
#include <string>
#include <vector>

#include "Vector4.h"
#include "Matrix4.h"
#include "Component.h"

class Material;
class Mesh;
class MeshRender;
class GameObject;

enum class LightType {
	Direct, Point
};

class Light:public Component {
public:
	void AddInstance(std::shared_ptr<Component> component);
	static std::vector<std::weak_ptr<Light>> lights;

	static const std::string Type;
	std::string GetType() override { return Type; }

	Light();
	Light(const Light& copy);

	void Render();

	void SetLightType(LightType type);
	LightType GetLightType() { return lightType; }

	Vector4 color;
	float range;

	bool shadowOpen = false;
	Matrix4 shadowMatrix;
	std::vector<std::weak_ptr<MeshRender>> shadowList;

	std::weak_ptr<Material> material;
private:
	std::weak_ptr<Mesh> mesh;
	LightType lightType;
};