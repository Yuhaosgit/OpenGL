//#include "DecalSystem.h"
//#include "../Blank Project/Renderer.h"
//#include "TextureLoader.h"
//
//DecalSystem::DecalSystem(std::unique_ptr<Shader> decalShader_) :decalShader(std::move(decalShader_)) {
//	decalTex = TextureLoader::LoadTexture2D("../Textures/doge.png");
//	decalNormalTex = TextureLoader::LoadTexture2D("../Textures/dogeNormal.png");
//}
//
//void DecalSystem::AddDecal(const Vector3& scale, const Vector3& pos, float rot, float remainTime){
//	Decal decal(remainTime);
//	decal.SettleDownModel(&decal, scale, pos, rot);
//	decals.emplace_back(decal);
//}
//
//void DecalSystem::Update(float dt) {
//	for (int i = 0; i < decals.size(); ++i) {
//		decals[i].Update(dt);
//		//decals[i].remainTime -= dt;
//		//if (decals[i].remainTime <= 0.0f) {
//		//	decals.erase(decals.begin() + i);
//		//}
//	}
//}
//
//void DecalSystem::RenderDecals() {
//	Renderer* renderer = Renderer::instance();
//	renderer->BindShader(decalShader.get());
//
//	decalShader->SubmitMatrix4(renderer->viewMatrix, "viewMatrix");
//	decalShader->SubmitMatrix4(renderer->projMatrix, "projMatrix");
//
//	renderer->gBuffer->stencilDepthTarget.Submit(decalShader.get(), "stencilDepthTex", 0);
//	renderer->gBuffer->normalTarget.Submit(decalShader.get(), "normals", 1);
//
//	decalTex.Submit(decalShader.get(), "targetTex", 2);
//	decalNormalTex.Submit(decalShader.get(), "targetNormalTex", 3);
//
//	Vector2 pixelSize = Vector2(1 / renderer->GetWidth(), 1 / renderer->GetHeight());
//	decalShader->SubmitVector2(pixelSize, "pixelSize");
//
//	glCullFace(GL_FRONT);
//	glDepthFunc(GL_ALWAYS);
//	for (int i = 0; i < decals.size(); ++i) {
//		decalShader->SubmitMatrix4(decals[i].GetModelMatrix(), "modelMatrix");
//		renderer->cube->Draw();
//	}
//	glDepthFunc(GL_LESS);
//	glCullFace(GL_BACK);
//}