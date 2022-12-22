//#pragma once
//#include "../nclgl/HeightMap.h"
//#include "Texture.h"
//#include <unordered_map>
//#include <string>
//#include <memory>
//
//class Terrain {
//public:
//	Terrain() = default;
//	Terrain(std::unique_ptr<Shader> terrainShader, const std::string& fileName,const float& stretch_fac, const float& height_fac, const float& tex_fac);
//
//	~Terrain() { delete mesh; }
//
//	virtual void Render() = 0;
//	Vector3 MeshSize() { return mesh->GetHeightMapSize(); }
//	float CoordHeight(const int& x, const int& y) { return mesh->AreaHeight(x, y); }
//protected:
//	std::unique_ptr<Shader> shader;
//	HeightMap* mesh;
//	std::unordered_map<std::string, std::shared_ptr<Texture2D>> textures;
//};
//
//class Land :public Terrain {
//public:
//	Land(std::unique_ptr<Shader> terrainShader, const std::string& fileName, const float& stretch_fac, const float& height_fac, const float& tex_fac);
//	void Render();
//};
//
//class Water :public Terrain {
//public:
//	Water(std::unique_ptr<Shader> terrainShader, const std::string& fileName, const float& stretch_fac, const float& height_fac, const float& tex_fac);
//	void Render();
//
//	void Update(const float& dt);
//private:
//	float _Time = 0;
//	float _Speed = 300;
//	float _Length = 500;
//	float _Amplitude = 4.5;
//};