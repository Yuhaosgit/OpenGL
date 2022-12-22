#include "Plane.h"

Plane::Plane(const Vector3& vec, const float& dis, bool normalize)
	:normal(vec), distance(dis) {
	if (normalize) {
		float length = sqrt(Vector3::Dot(normal, normal));
		normal = normal / length;
		distance /= length;
	}
}

void Plane::SetNormal(const Vector3& vec) {
	normal = vec;
}
Vector3 Plane::GetNormal() const {
	return normal;
}

void Plane::SetDistance(const float& dis) {
	distance = dis;
}
float Plane::GetDistance() const {
	return distance;
}

bool Plane::SphereInPlane(const Vector3& position, const float& radius) const {
	if (Vector3::Dot(position, normal) + distance <= -radius)
		return false;
	return true;
}
