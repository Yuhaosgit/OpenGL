#include "RenderPass.h"
#include "FrameBuffer.h"
#include "GameObject.h"
#include "Importer.h"
#include "Skybox.h"

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

		Material::shadowMatrix = light.lock()->lightProjMatrix * light.lock()->lightViewMatrix;
		for (auto shadowObj : light.lock()->shadowList) {
			Material::modelMatrix = shadowObj.lock()->gameObject->GetComponent<Transform>()->GetModelMatrix();
			shadowObj.lock()->RenderShadow();
		}
	}
}

void ShadowPass::RenderAfterSet() {
	glViewport(0, 0, OGLRenderer::GetCurrentWidth(), OGLRenderer::GetCurrentHeight());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
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
	GeometryFBO* gBuffer = FrameBuffer<GeometryFBO>().instance();
	LightFBO* lightBuffer = FrameBuffer<LightFBO>().instance();
	lightBuffer->Bind();
	CopyRenderTexture(gBuffer->stencilDepthTarget, lightBuffer->stencilDepthTarget, RenderTextureFormat::STENCIL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glDepthMask(GL_FALSE);
	glDepthFunc(GL_ALWAYS);

	glStencilMask(0x00);
	glStencilFunc(GL_EQUAL, 1, 0xff);
}

void DirectLightPass::RenderFunction(Camera* camera) {
	for (auto light : camera->lightList) {
		if (light.lock()->GetLightType() != LightType::Direct)
			continue;

		auto lightMat = light.lock()->material.lock();

		lightMat->SetVector2("pixelSize", Vector2(1.0f / camera->width, 1.0f / camera->height));
		lightMat->SetVector3("cameraPos", camera->gameObject->GetComponent<Transform>()->GetPosition());
		lightMat->SetMatrix4("inverseProjView", (camera->GetProjMatrix() * camera->GetViewMatrix()).Inverse());

		lightMat->SetFloat("lightWidth", light.lock()->lightWidth);
		lightMat->SetFloat("nearPlane", light.lock()->nearPlane);

		lightMat->SetFloat("lightIntensity", light.lock()->intensity);
		lightMat->SetVector4("lightColour", light.lock()->color);
		auto direct = light.lock()->gameObject->GetComponent<Transform>()->GetRotate().RotationMatrix();
		lightMat->SetVector3("lightOrientation", Vector3(direct.values[2], direct.values[6], direct.values[10]));

		lightMat->SetTexture("enviDiffuseTex", Importer::GetTexture("EnviDiffuse"));

		Material::shadowMatrix = light.lock()->lightProjMatrix * light.lock()->lightViewMatrix;
		light.lock()->Render();
	}
}

void DirectLightPass::RenderAfterSet() {
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glStencilFunc(GL_ALWAYS, 1, 0xff);
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

	auto skyboxTex = Skybox::skyboxTexture.lock();
	Importer::GetMaterial("CombineMaterial")->SetTexture("cubeTex", skyboxTex);

	Importer::GetMaterial("CombineMaterial")->SubmitData(false, true, true, false);
	Importer::GetMesh("Plane")->Draw();
}

void CombinePass::RenderAfterSet() {
	glDepthFunc(GL_LESS);
}
#pragma endregion

#pragma region Environment Diffuse
void EnvironmentDiffusePass::RenderPreset() {
	auto indirectDiffuseBuffer = FrameBuffer<IndirectDiffuseFBO>().instance();
	indirectDiffuseBuffer->Bind();

	int size = indirectDiffuseBuffer->cubemapResolution;
	glViewport(0, 0, size, size);
}

void EnvironmentDiffusePass::RenderFunction(Camera* camera) {
	Matrix4 captureViews[] =
	{
	   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f,  0.0f,  0.0f),Vector3(0.0f, -1.0f,  0.0f)),
	   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
	   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)),
	   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)),
	   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)),
	   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f))
	};

	auto mat = Importer::GetMaterial("IndirectDiffuseMaterial");
	mat->SetTexture("environmentMap", Skybox::skyboxTexture.lock());

	auto indirectDiffuseBuffer = FrameBuffer<IndirectDiffuseFBO>().instance();

	Material::projMatrix = camera->GetProjMatrix();
	for (unsigned int i = 0; i < 6; ++i)
	{
		Material::viewMatrix = captureViews[i];
		camera->UpdateList();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, indirectDiffuseBuffer->indirectDiffuse->texture, 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glClear(GL_COLOR_BUFFER_BIT);

		Importer::GetMaterial("IndirectDiffuseMaterial")->SubmitData(false, true, true, false);
		Importer::GetMesh("Plane")->Draw();
	}
}

void EnvironmentDiffusePass::RenderAfterSet() {

}
#pragma endregion
