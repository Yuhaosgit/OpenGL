#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Component.h"
#include <vector>
#include <fstream>
#include <memory>

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

	//view & projective
	void UpdateViewMatrix();
	const Matrix4& GetViewMatrix() { return viewMatrix; }

	void SetProjMatrix();
	const Matrix4& GetProjMatrix() { return projMatrix; }

	//list
	void UpdateList();
	std::vector<std::weak_ptr<MeshRender>> opaqueList;
	std::vector<std::weak_ptr<MeshRender>> transparentList;
	std::vector<std::weak_ptr<Light>> lightList;

	void SetNearPlane(const float& nearPlane_) { nearPlane = nearPlane_; SetProjMatrix(); }
	void SetFarPlane(const float& farPlane_) { farPlane = farPlane_; SetProjMatrix(); }
	void SetFOV(const float& FOV_) { FOV = FOV_; SetProjMatrix(); }
	
	void SetSize(const float& in_width, const float& in_height) { width = in_width; height = in_height; SetProjMatrix(); }
	float width;
	float height;
private:
	float nearPlane = 0.01f;
	float farPlane = 1000.0f;
	float FOV = 60.0f;

	Matrix4 viewMatrix;
	Matrix4 projMatrix;
};