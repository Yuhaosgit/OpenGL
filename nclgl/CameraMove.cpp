#include "CameraMove.h"
#include "GameObject.h"
#include "Window.h"

const std::string CameraMove::Type = "CameraMove";

void CameraMove::Start() {

}

void CameraMove::Update() {
	std::weak_ptr<Transform> transform = gameObject->GetComponent<Transform>();

	auto Turn = [&]() {
	pitch += (Window::GetMouse()->GetRelativePosition().y);
	yaw += (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::max(pitch, -90.0f);
	pitch = std::min(pitch, 90.0f);

	if (yaw > 360)
		yaw -= 360.0f;
	if (yaw < 0)
		yaw += 360.0f;

	Quaternion q_pitch = Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), pitch);
	Quaternion q_yaw = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), yaw);

	transform.lock()->SetRotate(q_pitch * q_yaw);
	};

	auto Move = [&]() {
		auto direct = transform.lock()->GetRotate().RotationMatrix(); 

		Vector3 forward = -Vector3(direct.values[2], direct.values[6], direct.values[10]).Normalised();
		Vector3 left = -Vector3(direct.values[0], direct.values[4], direct.values[8]).Normalised();

		float constSpeed = speed * Window::GetTimer()->GetTimeDeltaSeconds();

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
			transform.lock()->Translate(forward * constSpeed);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
			transform.lock()->Translate(-forward * constSpeed);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
			transform.lock()->Translate(left * constSpeed);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
			transform.lock()->Translate(-left * constSpeed);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT))
			transform.lock()->Translate(Vector3(0, -1, 0) * constSpeed);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE))
			transform.lock()->Translate(Vector3(0, 1, 0) * constSpeed);
	};

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::L))
		cameraLock = !cameraLock;

	if (!cameraLock) {
		Move();
		Turn();
	}
}