#pragma once
#include <memory>
#include "Plane.h"
#include "GameObject.h"
#include "Matrix4.h"

class Frustum {
protected:
	Plane plane[6];
public:
	Frustum() = default;
	~Frustum() = default;

	void FromMatrix(const Matrix4& mat);
	//bool InsideFrustum(std::shared_ptr<GameObject> node);
};