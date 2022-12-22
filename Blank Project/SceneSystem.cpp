//#include "SceneSystem.h"
//
//void SceneSystem::BuildNodeLists(const Vector3& cameraPos) {
//	for (int i = 0; i < opaque.size(); ++i) {
//		if (frustum.InsideFrustum(opaque[i])) {
//			Vector3 dir = opaque[i]->GetPosition() - cameraPos;
//			opaque[i]->SetCameraDistance(Vector3::Dot(dir, dir));
//
//			opaqueList.emplace_back(opaque[i]);
//		}
//	}
//
//	for (int i = 0; i < transparent.size(); ++i) {
//		if (frustum.InsideFrustum(transparent[i])) {
//			transparentList.emplace_back(transparent[i]);
//		}
//	}
//}
//
//void SceneSystem::SortNodeLists() {
//	std::sort(opaqueList.begin(), opaqueList.end(), SceneNode::CompareByCameraDistance);
//}
//
//void SceneSystem::ClearNodeLists() {
//	transparentList.clear();
//	opaqueList.clear();
//}
//
//void SceneSystem::DrawOpaque(bool forwardRender) {
//	for (int i = 0; i < opaqueList.size(); ++i) {
//		opaqueList[i]->RendObjectDeferred();
//	}
//}
//
//void SceneSystem::DrawTransparent() {
//	for (int i = 0; i < transparent.size(); ++i) {
//		transparent[i]->RendObjectForward();
//	}
//}
//
//
//void SceneSystem::UpdateNodes(const float& dt, Matrix4& ProjectViewMat, const Vector3& cameraPos) {
//	ProjectViewMatrix = ProjectViewMat;
//	frustum.FromMatrix(ProjectViewMatrix);
//	
//	BuildNodeLists(cameraPos);
//	SortNodeLists();
//
//	root->Update(dt);
//}
//
//void SceneSystem::RenderOpaque() {
//	DrawOpaque(false);
//	opaqueList.clear();
//}
//void SceneSystem::RenderTransparent() {
//	DrawTransparent();
//	transparentList.clear();
//}
//void SceneSystem::RenderShadow(Shader* shader, Matrix4& PVMatrix) {
//	for (auto& opaque : opaqueList) {
//		opaque->RenderShadow(shader, PVMatrix);
//	}
//
//	for (auto& trans : transparentList) {
//		trans->RenderShadow(shader, PVMatrix);
//	}
//}
//
//void SceneSystem::AddOpaqueObject(SceneNodePtr obj) {
//	root->AddChild(obj);
//	opaque.emplace_back(obj);
//}
//
//void SceneSystem::AddTransparentObject(SceneNodePtr obj) {
//	root->AddChild(obj);
//	transparent.emplace_back(obj);
//}