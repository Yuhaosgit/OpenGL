#include "RenderPass.h"
#include "FrameBuffer.h"
#include "GameObject.h"
#include "Importer.h"

#pragma region ShadowPass
void ShadowPass::RenderPreset() {
	FrameBuffer<ShadowFBO>().instance()->Bind();
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, ShadowMapSize, ShadowMapSize);
	glCullFace(GL_FRONT);
}

void ShadowPass::RenderFunction(Camera* camera) {
	for (auto light : camera->lightList) {
		if (!light.lock()->shadowOpen)
			continue;

		Material::shadowMatrix = light.lock()->shadowMatrix;
		for (auto shadowObj : light.lock()->shadowList) {
			Material::modelMatrix = shadowObj.lock()->gameObject->GetComponent<Transform>()->GetModelMatrix();
			shadowObj.lock()->RenderShadow();
		}
	}
}

void ShadowPass::RenderAfterSet() {
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, OGLRenderer::GetWidth(), OGLRenderer::GetHeight());
	glCullFace(GL_BACK);
}
#pragma endregion

#pragma region Gbuffer
void GbufferPass::RenderPreset() {
	FrameBuffer<GeometryFBO>().instance()->Bind();
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glDepthFunc(GL_LESS);

	glDisable(GL_BLEND);
}

void GbufferPass::RenderFunction(Camera* camera) {
	Material::projMatrix = camera->GetProjMatrix();
	Material::viewMatrix = camera->GetViewMatrix();

	for (auto opaque : camera->opaqueList) {
		Material::modelMatrix = opaque.lock()->gameObject->GetComponent<Transform>()->GetModelMatrix();
		opaque.lock()->Render();
	}
}

void GbufferPass::RenderAfterSet() {

}
#pragma endregion

#pragma region Decal
void DecalPass::RenderPreset() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DecalPass::RenderFunction(Camera* camera) {
	//for (int i = 0; i < camera->opaqueList.size(); ++i) {
	//	if (camera->visibleObject[i].lock()->decal != nullptr)
	//		camera->visibleObject[i].lock()->decal->Render();
	//}
}

void DecalPass::RenderAfterSet() {
	glDisable(GL_BLEND);
}
#pragma endregion

#pragma region Light
void DirectLightPass::RenderPreset() {
	FrameBuffer<LightFBO>().instance()->Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_ALWAYS);
}

void DirectLightPass::RenderFunction(Camera* camera) {
	for (auto light : camera->lightList) {
		if (light.lock()->GetLightType() != LightType::Direct)
			continue;

		auto lightMat = light.lock()->material.lock();

		lightMat->SetVector2("pixelSize", Vector2(1.0f / OGLRenderer::GetWidth(), 1.0f / OGLRenderer::GetHeight()));
		lightMat->SetVector3("cameraPos", camera->gameObject->GetComponent<Transform>()->GetPosition());
		lightMat->SetMatrix4("inverseProjView", (camera->GetProjMatrix() * camera->GetViewMatrix()).Inverse());

		lightMat->SetVector4("lightColour", light.lock()->color);

		auto direct = light.lock()->gameObject->GetComponent<Transform>()->GetRotate().RotationMatrix();
		lightMat->SetVector3("lightOrientation", Vector3(direct.values[2], direct.values[6], direct.values[10]));
		Material::shadowMatrix = light.lock()->shadowMatrix;
		light.lock()->Render();
	}
}

void DirectLightPass::RenderAfterSet() {
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
#pragma endregion

#pragma region Combination
void CombinePass::RenderPreset() {
	FrameBuffer<ScreenFBO>().instance()->Bind();
	glDepthFunc(GL_ALWAYS);
}

void CombinePass::RenderFunction(Camera* camera) {
	Material::projMatrix = camera->GetProjMatrix();
	Material::viewMatrix = camera->GetViewMatrix();

	auto skyboxTex = camera->gameObject->GetComponent<Skybox>()->skyboxTexture.lock();
	Importer::MaterialSet["CombineMaterial"]->SetTexture("cubeTex", skyboxTex);

	Importer::MaterialSet["CombineMaterial"]->SubmitData(false, true, true, false);
	Importer::MeshSet["Plane"]->Draw();
}

void CombinePass::RenderAfterSet() {
	glDepthFunc(GL_LESS);
}
#pragma endregion