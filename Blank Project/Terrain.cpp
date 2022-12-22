//#include "Terrain.h"
//#include "Renderer.h"
//#include "../nclgl/TextureLoader.h"
//
//#define LANDPATH "../Textures/Land/"
//#define WATERPATH "../Textures/Water/"
//
//Terrain::Terrain(std::unique_ptr<Shader> terrainShader, const std::string& fileName, const float& stretch_fac, const float& height_fac, const float& tex_fac)
//{
//	shader = std::move(terrainShader);
//	mesh = new HeightMap(fileName, stretch_fac, height_fac, tex_fac);
//}
//
//#pragma region Land
//Land::Land(std::unique_ptr<Shader> terrainShader, const std::string& fileName, const float& stretch_fac, const float& height_fac, const float& tex_fac)
//	:Terrain(std::move(terrainShader), LANDPATH +fileName, stretch_fac, height_fac, tex_fac)
//{
//	textures["diffuseTex_ground"] =  std::make_shared<Texture2D>
//		(TextureLoader::LoadTexture2D(LANDPATH"ground_tex.tga",true));
//
//	textures["diffuseTex_weed"] = std::make_shared<Texture2D>
//		(TextureLoader::LoadTexture2D(LANDPATH"weed_tex.tga",true));
//
//	textures["bumpTex_ground"] = std::make_shared<Texture2D>
//		(TextureLoader::LoadTexture2D(LANDPATH"ground_nor.tga",true));
//
//	textures["bumpTex_weed"] = std::make_shared<Texture2D>
//		(TextureLoader::LoadTexture2D(LANDPATH"weed_nor.tga",true));
//}
//
//void Land::Render() {
//	Renderer::instance()->BindShader(shader.get());
//
//	int i = 0;
//	for (auto &tex : textures) {
//		tex.second->Submit(shader.get(), tex.first, i);
//		++i;
//	}
//
//	Renderer::instance()->modelMatrix.ToIdentity();
//	Renderer::instance()->UpdateShaderMatrices();
//
//	mesh->Draw();
//}
//#pragma endregion
//
//#pragma region Water
//Water::Water(std::unique_ptr<Shader> terrainShader, const std::string& fileName, const float& stretch_fac, const float& height_fac, const float& tex_fac)
//	:Terrain(std::move(terrainShader), WATERPATH + fileName, stretch_fac, height_fac, tex_fac) 
//{
//	textures["water_diffuseTex"] = std::make_shared<Texture2D>
//		(TextureLoader::LoadTexture2D(WATERPATH"water_tex.png", true));
//
//	textures["water_bumpTex"] = std::make_shared<Texture2D>
//		(TextureLoader::LoadTexture2D(WATERPATH"water_nor.png", true));
//}
//
//void Water::Render() {
//	Renderer::instance()->BindShader(shader.get());
//
//	shader->SubmitVector3(Renderer::instance()->CameraPosition(), "cameraPos");
//
//	int i = 0;
//	for (auto &tex : textures) {
//		tex.second->Submit(shader.get(), tex.first, i);
//		++i;
//	}
//
//	shader->SubmitFloat(_Time, "_Time");
//	shader->SubmitFloat(_Speed, "_Speed");
//	shader->SubmitFloat(_Length, "_Length");
//	shader->SubmitFloat(_Amplitude, "_Amplitude");
//
//	Vector2 center(mesh->GetHeightMapSize().x / 2, mesh->GetHeightMapSize().z / 2);
//	shader->SubmitVector2(center, "_Center");
//
//	Renderer::instance()->modelMatrix.values[13] = 80;
//	Renderer::instance()->UpdateShaderMatrices();
//
//	Renderer::instance()->mainLightSystem->SubmitMainLightData(shader.get());
//
//	mesh->Draw();
//}
//
//void Water::Update(const float& dt) {
//	_Time += dt;
//}
//#pragma endregion
