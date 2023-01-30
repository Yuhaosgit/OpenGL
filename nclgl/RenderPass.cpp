#include "RenderPass.h"
#include "GameObject.h"
#include "Skybox.h"

void ShadowPass::Pass(Camera* camera) {
	auto RenderPreset = []() {
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glCullFace(GL_FRONT);
	};

	auto RenderFunc = [&]() {
		for (auto light : camera->lightList) {
			if (!light.lock()->IfShadowCast())
				continue;

			light.lock()->shadowBuffer->BindFrameBuffer();
			glViewport(0, 0, light.lock()->ShadowSize, light.lock()->ShadowSize);
			Material::shadowMatrix = light.lock()->lightProjMatrix * light.lock()->lightViewMatrix;

			for (auto shadowObj : light.lock()->shadowList) {
				Material::modelMatrix = shadowObj.lock()->gameObject->GetComponent<Transform>()->GetModelMatrix();
				shadowObj.lock()->RenderShadow();
			}
		}
	};

	auto RenderAfterSet = [&]() {
		glViewport(0, 0, camera->width, camera->height);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glCullFace(GL_BACK);
	};

	RenderPreset();
	RenderFunc();
	RenderAfterSet();
}

void GbufferPass::Pass(Camera* camera) {
	auto RenderPreset = []() {
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glDepthFunc(GL_LESS);

		glDisable(GL_BLEND);
	};

	auto RenderFunc = [&]() {
		Material::projMatrix = camera->GetProjMatrix();
		Material::viewMatrix = camera->GetViewMatrix();

		for (auto opaque : camera->opaqueList) {
			Material::modelMatrix = opaque.lock()->gameObject->GetComponent<Transform>()->GetModelMatrix();
			opaque.lock()->Render();
		}
	};

	RenderPreset();
	RenderFunc();
}

void DirectLightPass::Pass(Camera* camera) {
	auto RenderPreset = []() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		glDepthMask(GL_FALSE);
		glDepthFunc(GL_ALWAYS);

		glStencilMask(0x00);
		glStencilFunc(GL_EQUAL, 1, 0xff);
	};

	auto RenderFunc = [&]() {
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

			lightMat->SetTexture("ColorRoughnessTex", colorRoughnessBuffer.lock());
			lightMat->SetTexture("normalMetallicTex", normalMetallicBuffer.lock());
			lightMat->SetTexture("depthTex", stencilDepthBuffer.lock());
			lightMat->SetTexture("shadowTex", light.lock()->shadowBuffer->GetRenderTexture("shadowTex"));
			  
			lightMat->SetTexture("enviDiffuseTex", Importer::GetTexture("EnviDiffuse"));
			lightMat->SetTexture("prefilterMap", Importer::GetTexture("Prefilter"));
			lightMat->SetTexture("LUT", Importer::GetTexture("LUT"));

			Material::shadowMatrix = light.lock()->lightProjMatrix * light.lock()->lightViewMatrix;
			light.lock()->Render();
		}
	};

	auto RenderAfterSet = [&]() {
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);

		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glStencilFunc(GL_ALWAYS, 1, 0xff);
	};

	RenderPreset();
	RenderFunc();
	RenderAfterSet();
}

void CombinePass::Pass(Camera* camera) {
	auto RenderPreset = []() {
		glDepthFunc(GL_ALWAYS);
	};

	auto RenderFunc = [&]() {
		Material::projMatrix = camera->GetProjMatrix();
		Material::viewMatrix = camera->GetViewMatrix();

		auto skyboxTex = Skybox::skyboxTexture;
		material->SetTexture("cubeTex", skyboxTex.lock());
		material->SetTexture("directLightTex", opaqueTex.lock());
		material->SetTexture("depthTex", depthTex.lock());

		material->SubmitData(false, true, true, false);
		Importer::GetMesh("Plane")->Draw();
	};

	auto RenderAfterSet = [&]() {
		glDepthFunc(GL_LESS);
	};

	RenderPreset();
	RenderFunc();
	RenderAfterSet();
}

const Matrix4 captureViews[] = {
   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f,  0.0f,  0.0f),Vector3(0.0f, -1.0f,  0.0f)),
   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f,  0.0f,  0.0f), Vector3(0.0f, -1.0f,  0.0f)),
   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  1.0f,  0.0f), Vector3(0.0f,  0.0f,  1.0f)),
   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f,  0.0f), Vector3(0.0f,  0.0f, -1.0f)),
   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  0.0f,  1.0f), Vector3(0.0f, -1.0f,  0.0f)),
   Matrix4::BuildViewMatrix(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f,  0.0f, -1.0f), Vector3(0.0f, -1.0f,  0.0f))
};

void EnvironmentDiffusePass::Pass(Camera* camera) {
	auto RenderPreset = [&]() {
		glViewport(0, 0, camera->width, camera->height);
	};

	auto RenderFunc = [&]() {
		auto mat = Importer::GetMaterial("EnvironmentDiffuseMaterial");
		mat->SetTexture("environmentMap", Skybox::skyboxTexture.lock());
		Material::projMatrix = camera->GetProjMatrix();

		for (unsigned int i = 0; i < 6; ++i)
		{
			GIdiffuseBuffer.lock()->SetFrameBufferMipmapAndFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, IOstate::Write);

			Material::viewMatrix = captureViews[i];
			mat->SubmitData(false, true, true, false);
			Importer::GetMesh("Plane")->Draw();
		}
	};

	RenderPreset();
	RenderFunc();
}

void SpecularPrefilterPass::Pass(Camera* camera) {
	auto RenderPreset = [&]() {
		glViewport(0, 0, camera->width, camera->width);
	};

	auto RenderFunc = [&]() {
		auto mat = Importer::GetMaterial("SpecularPrefilterMaterial");

		mat->SetTexture("environmentMap", Skybox::skyboxTexture.lock());
		for (int mipLevel = 0; mipLevel < maxMipLevel; ++mipLevel)
		{
			int mipWidth = camera->width * std::pow(0.5, mipLevel);
			int mipHeight = camera->width * std::pow(0.5, mipLevel);
			camera->SetSize(mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = static_cast<float>(mipLevel) / static_cast<float>(maxMipLevel - 1);
			mat->SetFloat("roughness", roughness);
			for (int i = 0; i < 6; ++i)
			{
				GIspecularPrefilter.lock()->
					SetFrameBufferMipmapAndFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipLevel, IOstate::Write);

				Material::viewMatrix = captureViews[i];
				mat->SubmitData(false, true, true, false);
				Importer::GetMesh("Plane")->Draw();
			}
		}
	};

	RenderPreset();
	RenderFunc();
}

void SpecularLUTPass::Pass(Camera* camera) {
	auto RenderPreset = [&]() {
		glViewport(0, 0, camera->width, camera->height);
	};

	auto RenderFunc = [&]() {
		Importer::GetMaterial("SpecularLUTMaterial")->SubmitData();
		Importer::GetMesh("Plane")->Draw();
	};

	RenderPreset();
	RenderFunc();
}