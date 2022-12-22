//#include "GameLogic.h"
//#include "Window.h"
//#include "../Blank Project/Renderer.h"
//
//Vector3 GameLogic::ScreenToWorldPosition(const Vector2& screenPos) {
//	Renderer* renderer = Renderer::instance();
//	Matrix4 InverseMatrix = (renderer->projMatrix * renderer->viewMatrix).Inverse();
//	Vector4 worldPos;
//
//	worldPos.x = (screenPos.x / renderer->GetWindowWidth()) * 2 - 1;
//	worldPos.y = (screenPos.y / renderer->GetWindowHeight()) * 2 - 1;
//	worldPos.z = 0.99 * 2 - 1;
//	worldPos.w = 1;
//
//	worldPos = InverseMatrix * worldPos;
//
//	Vector3 result = Vector3(worldPos.x / worldPos.w, worldPos.y / worldPos.w, worldPos.z / worldPos.w);
//	return result;
//}
//
//void GameLogic::AddDecal() {
//	Renderer* renderer = Renderer::instance();
//	Vector3 worldPos = ScreenToWorldPosition(Window::GetMouse()->GetRelativePosition());
//	worldPos.y = renderer->land->CoordHeight(worldPos.x, worldPos.z)+50;
//	renderer->mainDecals->AddDecal(Vector3(200, 200, 200), worldPos, 0);
//}