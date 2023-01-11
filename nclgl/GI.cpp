#include "GI.h"
#include "GameObject.h"
#include "FrameBuffer.h"
#include "RenderPass.h"
#include "Exporter.h"

const std::string IBL::Type = "IBL";

void IBL::Start() {
	auto camera = std::make_shared<Camera>();
	camera->SetFOV(90);

	gameObject->AddComponent(camera);
	gameObject->GetComponent<Transform>()->SetPosition(Vector3(0, 10, 0));
}

void IBL::Update() {
	if (Win32Window::GetKeyboard()->KeyPressed(KeyboardKeys::B)) {
		auto camera = gameObject->GetComponent<Camera>();
		auto buffer = FrameBuffer<EnvironmentMapFBO>().instance();
		int size = buffer->diffuseResolution;
		camera->SetSize(size, size);

		RenderPass<EnvironmentDiffusePass>().Pass(camera.get());
		Exporter::ExportCubemap("../Baked/EnviDiffuse/", buffer->environmentTarget.get());
	}

	if (Win32Window::GetKeyboard()->KeyPressed(KeyboardKeys::N)) {
		auto camera = gameObject->GetComponent<Camera>();
		auto EnviMapbuffer = FrameBuffer<EnvironmentMapFBO>().instance();
		int size = EnviMapbuffer->specularResolution;
		camera->SetSize(size, size);

		EnviMapbuffer->GenerateMipmap(size, size);
		RenderPass<SpecularPrefilterPass>().Pass(camera.get());
		for (int i = 0; i < RenderPass<SpecularPrefilterPass>().Instance()->maxMipLevel; ++i) {
			std::string path = "../Baked/Specular/Prefilter/mipmap" + std::to_string(i) + "/";
			Exporter::ExportCubemap(path, EnviMapbuffer->environmentTarget.get(), i);
		}
		//LUT
		auto LUTbuffer = FrameBuffer<SpecularLUTFBO>().instance();
		camera->SetSize(LUTbuffer->LUTResolution, LUTbuffer->LUTResolution);
		RenderPass<SpecularLUTPass>().Pass(camera.get());
		Exporter::ExportTexture2D("../Baked/Specular/", "LUT.png", LUTbuffer->LUT_Target.get());
	}
}
