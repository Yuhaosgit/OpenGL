#pragma once
#include "Matrix4.h"
#include "Component.h"

class MeshRender;
class Light;
class Material;

class Camera : public Component {
public:
	void AddInstance(std::shared_ptr<Component> component);
	static std::vector<std::weak_ptr<Camera>> cameras;

	static const std::string Type;
	std::string GetType() override { return Type; }

	Camera();
	~Camera() { }

	void UpdateCamera();

	//camera attributes
	void UpdateViewMatrix();
	const Matrix4& GetViewMatrix() { return viewMatrix; }

	void SetProjMatrix();
	const Matrix4& GetProjMatrix() { return projMatrix; }

	void SetNearPlane(float nearPlane_) { nearPlane = nearPlane_; SetProjMatrix(); }
	void SetFarPlane(float farPlane_) { farPlane = farPlane_; SetProjMatrix(); }
	void SetFOV(float FOV_) { FOV = FOV_; SetProjMatrix(); }
	
	void SetSize(int in_width, int in_height) { width = in_width; height = in_height; SetProjMatrix(); }
	int width;
	int height;

	//list update every frame
	void UpdateList();
	std::vector<std::weak_ptr<MeshRender>> opaqueList;
	std::vector<std::weak_ptr<MeshRender>> transparentList;
	std::vector<std::weak_ptr<Light>> lightList;
private:
	float nearPlane = 0.01f;
	float farPlane = 1000.0f;
	float FOV = 60.0f;

	Matrix4 viewMatrix;
	Matrix4 projMatrix;
};