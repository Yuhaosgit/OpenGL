//#pragma once
//#include <vector>
//#include <memory>
//#include "SceneNode.h"
//#include "Texture.h"
//
//class DecalSystem {
//	class Decal :public SceneNode {
//	public:
//		Decal(float remainTime_) :remainTime(remainTime_) { }
//		float remainTime;
//	};
//
//public:
//	DecalSystem(std::unique_ptr<Shader> decalShader_);
//	void AddDecal(const Vector3& scale, const Vector3& pos, float rot, float remainTime = 60);
//	void Update(float dt);
//	void RenderDecals();
//private:
//	vector<Decal> decals;
//	std::unique_ptr<Shader> decalShader;
//	Texture2D decalTex;
//	Texture2D decalNormalTex;
//};