#include "Frustum.h"

//bool Frustum::InsideFrustum(SceneNodePtr node)
//{
//	int count = 0;
//	for (int i = 0; i < 6; ++i) {
//		if (!plane[i].SphereInPlane(node->GetPosition(), node->GetBoundingRadius()))
//			++count;
//	}
//	return count == 6 ? false : true;
//}

void Frustum::FromMatrix(const Matrix4& mat) {
	Vector3 x_axis = Vector3(mat.values[0], mat.values[4], mat.values[8]);
	Vector3 y_axis = Vector3(mat.values[1], mat.values[5], mat.values[9]);
	Vector3 z_axis = Vector3(mat.values[2], mat.values[6], mat.values[10]);
	Vector3 w_axis = Vector3(mat.values[3], mat.values[7], mat.values[11]);

	plane[0] = Plane(w_axis - x_axis, (mat.values[15] - mat.values[12]), true);
	plane[1] = Plane(w_axis + x_axis, (mat.values[15] + mat.values[12]), true);

	plane[2] = Plane(w_axis - y_axis, (mat.values[15] - mat.values[13]), true);
	plane[3] = Plane(w_axis + y_axis, (mat.values[15] + mat.values[13]), true);

	plane[4] = Plane(w_axis - z_axis, (mat.values[15] - mat.values[14]), true);
	plane[5] = Plane(w_axis + z_axis, (mat.values[15] + mat.values[14]), true);

}