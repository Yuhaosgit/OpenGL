#pragma once
#include <memory>
#include <vector>
#include <string>

#include "Component.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Quaternion.h"

class GameObject;

class Transform : public Component, public std::enable_shared_from_this<Transform> {
	friend class Camera;
public:
	void AddInstance(std::shared_ptr<Component> component);
	static std::vector<std::weak_ptr<Transform>> transforms;

	static const std::string Type;
	std::string GetType() override { return Type; }

	Transform();

	const Matrix4& GetModelMatrix() { return modelMatrix; }

	void SetScale(const Vector3& vec);
	const Vector3& GetScale();

	void SetRotate(const Vector3& vec);
	void SetRotate(const Quaternion& quat);
	const Quaternion& GetRotate();

	void SetPosition(const Vector3& vec);
	void Translate(const Vector3& vec);
	const Vector3& GetPosition();

	void Update();
	void Update(const Vector3& vec);

	void AddChild(std::weak_ptr<Transform> child);
	std::weak_ptr<Transform> parent;
	std::vector<std::weak_ptr<Transform>> children;

private:
	Matrix4 modelMatrix;

	Matrix4 positionMatrix;
	Matrix4 scaleMatrix;
	Quaternion rotate;
};