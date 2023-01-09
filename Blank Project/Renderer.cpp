#include "Renderer.h"
#include "../nclgl/RenderPass.h"
#include "../nclgl/UI.h"

Renderer::Renderer(Win32Window& parent) :OGLRenderer(parent) {
	root = new GameObject("Root");
	UI::initialization();

	Initialize();
	SetEnvironment();
	auto ScriptStart = [&]() {
		for (auto script : BaseScript::scripts) {
			script.lock()->Start();
		}
	};
	ScriptStart();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer() {
	Importer::ReleaseAllResources();
	delete root;
	UI::terminate();
}

void Renderer::UpdateScene(const float& dt) {
	auto UpdateScript = [&]() {
		for (auto script : BaseScript::scripts) {
			script.lock()->Update();
		}
	};

	UpdateScript();
	camera->UpdateCamera();
}

void Renderer::RenderScene() {
	RenderPass<ShadowPass>().Pass(camera.get());
	RenderPass<GbufferPass>().Pass(camera.get());
	RenderPass<DirectLightPass>().Pass(camera.get());
	RenderPass<CombinePass>().Pass(camera.get());

	UI::Render();
}
