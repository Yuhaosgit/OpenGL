#include "Engine.h"
#include "../nclgl/RenderPass.h"
#include "../nclgl/UI.h"

Engine::Engine(Win32Window& parent) :OGLRenderer(parent) {
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
	deferredPipeline = new DeferredPipeline(camera);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Engine::~Engine() {
	Importer::ReleaseAllResources();
	delete root;
	delete deferredPipeline;
	UI::terminate();
}

void Engine::UpdateScene(const float& dt) {
	auto UpdateScript = [&]() {
		for (auto script : BaseScript::scripts) {
			script.lock()->Update();
		}
	};

	UpdateScript();
	camera->UpdateCamera();
}

void Engine::RenderScene() {
	deferredPipeline->Render();
	UI::Render();
}
