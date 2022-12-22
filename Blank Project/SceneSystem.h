//#pragma once
//#include "../nclgl/GameObject.h"
//#include "../nclgl/Frustum.h"
//
//class SceneSystem {
//public:
//	SceneSystem(){
//		root = std::make_shared<GameObject>();
//	}
//	~SceneSystem() { 
//		ClearNodeLists();
//		transparent.clear();
//		opaque.clear();
//	}
//	void UpdateNodes(const float& dt, Matrix4& ProjectViewMat, const Vector3& cameraPos);
//
//	void RenderOpaque();
//	void RenderTransparent();
//	void RenderShadow(Shader* shader, Matrix4& PVMatrix);
//
//	void AddOpaqueObject(GameObject obj);
//	void AddTransparentObject(GameObject obj);
//private:
//	GameObject root;
//
//	Matrix4 ProjectViewMatrix;
//
//	Frustum frustum;
//	vector<GameObject> transparent;
//	vector<GameObject> opaque;
//
//	vector<GameObject> transparentList;
//	vector<GameObject> opaqueList;
//
//	void BuildNodeLists(const Vector3& cameraPos);
//	void SortNodeLists();
//	void ClearNodeLists();
//	void DrawOpaque(bool forwardRender);
//	void DrawTransparent();
//};