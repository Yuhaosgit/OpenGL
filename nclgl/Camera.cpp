#include "Camera.h"
#include "OGLRenderer.h"
#include "GameObject.h"

#include <algorithm>
#include <iostream>

std::vector<std::weak_ptr<Camera>> Camera::cameras;
const std::string Camera::Type = "Camera";

void Camera::AddInstance(std::shared_ptr<Component> component) {
	cameras.emplace_back(std::dynamic_pointer_cast<Camera>(component));
}

Camera::Camera() :width(OGLRenderer::GetCurrentWidth()), height(OGLRenderer::GetCurrentHeight()){
	SetProjMatrix();
}

void Camera::UpdateCamera() {
	UpdateViewMatrix();
	UpdateList();
}

void Camera::UpdateViewMatrix() {
	auto transform = gameObject->GetComponent<Transform>();
	auto posMatrix = transform->positionMatrix;
	posMatrix.values[12] = -posMatrix.values[12];
	posMatrix.values[13] = -posMatrix.values[13];
	posMatrix.values[14] = -posMatrix.values[14];

	viewMatrix = transform->rotate.RotationMatrix() * posMatrix;
}

void Camera::SetProjMatrix() {
	float aspect = width / height;

	projMatrix = Matrix4::Perspective(nearPlane, farPlane, aspect, FOV);
}

void Camera::UpdateList() {
	opaqueList.clear();
	transparentList.clear();
	lightList.clear();

	for (int i = 0; i < MeshRender::meshRenders.size(); ++i) {
		opaqueList.emplace_back(MeshRender::meshRenders[i]);
	}

	for (auto light : Light::lights) {
		if (light.lock()->GetLightType() == LightType::Direct) {
			lightList.emplace_back(light);
		}
	}

	auto UpdateLightShadow = [&]() {
		for (auto light : lightList) {
			light.lock()->shadowList.clear();
			if (!light.lock()->shadowOpen)
				continue;
			if (light.lock()->GetLightType() == LightType::Direct) {
				auto transform = light.lock()->gameObject->GetComponent<Transform>();
				auto direction = transform->GetRotate().RotationMatrix().Forward();

				light.lock()->lightProjMatrix = Matrix4::Orthographic(0.1, 50, 50, -50, 50, -50);
				light.lock()->lightViewMatrix = Matrix4::BuildViewMatrix(direction * 20, Vector3(0, 0, 0));
			
				for (int i = 1; i < MeshRender::meshRenders.size(); ++i) {
					light.lock()->shadowList.emplace_back(MeshRender::meshRenders[i]);
				}
			}
		}
	};
	UpdateLightShadow();
}