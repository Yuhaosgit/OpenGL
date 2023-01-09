#include "GI.h"
#include "GameObject.h"
#include "FrameBuffer.h"
#include "RenderPass.h"
#include "Exporter.h"

const std::string IBL::Type = "IBL";

void IBL::Start() {
	auto camera = std::make_shared<Camera>();
	camera->SetFOV(90);
	int size = FrameBuffer<IndirectDiffuseFBO>().instance()->cubemapResolution;
	camera->SetSize(size, size);

	gameObject->AddComponent(camera);
	gameObject->GetComponent<Transform>()->SetPosition(Vector3(0, 10, 0));
}

void IBL::Update() {
	if (Win32Window::GetKeyboard()->KeyPressed(KeyboardKeys::B)) {
		auto camera = gameObject->GetComponent<Camera>();
		RenderPass<EnvironmentDiffusePass>().Pass(camera.get());
		Exporter::ExportCubemap("../Baked/EnviDiffuse/", this);
	}
}
