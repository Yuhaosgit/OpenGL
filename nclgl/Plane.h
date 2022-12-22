#pragma once
#include "Vector3.h"

class Plane {
public:
	Plane() = default;
	Plane(const Vector3& vec, const float& dis, bool normalize = false);
	~Plane() = default;

	void SetNormal(const Vector3& vec);
	Vector3 GetNormal() const;

	void SetDistance(const float& dis);
	float GetDistance() const;

	bool SphereInPlane(const Vector3& position, const float& radius) const;
protected:
	Vector3 normal;
	float distance;
};