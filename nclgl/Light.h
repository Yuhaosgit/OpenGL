#pragma once
#include "Component.h"
#include "FrameBuffer.h"

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

	void OpenShadow();
	void CloseShadow();

	bool IfShadowCast() { return shadowOpen; }

	Vector4 color;
	float range;
	float intensity = 2.0;
	float lightWidth = 0.7f;
	float nearPlane = 7.5f;
	
	const int ShadowSize = 2048;
	Matrix4 lightViewMatrix;
	Matrix4 lightProjMatrix;
	std::vector<std::weak_ptr<MeshRender>> shadowList;
	std::shared_ptr<FrameBuffer> shadowBuffer;
	
	std::weak_ptr<Material> material;
private:
	std::weak_ptr<Mesh> mesh;
	LightType lightType;

	bool shadowOpen = false;
};