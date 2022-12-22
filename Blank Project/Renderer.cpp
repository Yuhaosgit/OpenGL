#include "Renderer.h"
#include "../nclgl/RenderPass.h"

Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	root = new GameObject("Root");

	Initialize();
	SetEnvironment();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glViewport(0, 0, OGLRenderer::GetWidth(), OGLRenderer::GetHeight());

	init = true;
}

void Renderer::UpdateScene(const float& dt) {
	auto UpdateScript = [&]() {
		for (auto script : BaseScript::scripts) {
			script.lock()->Update();
		}
	};

	camera->UpdateCamera();
	UpdateScript();
}

void Renderer::RenderScene() {
	RenderPass<ShadowPass>().Pass(camera.get());
	RenderPass<GbufferPass>().Pass(camera.get());
	RenderPass<DirectLightPass>().Pass(camera.get());
	RenderPass<CombinePass>().Pass(camera.get());
}
