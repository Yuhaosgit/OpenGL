#pragma once
#include <vector>
#include "../nclgl/GameObject.h"
#include "../nclgl/Importer.h"
#include "../nclgl/Win32Window.h"
#include "../nclgl/Pipeline.h"

class Engine : public OGLRenderer {
public:
	static Engine* instance(Win32Window* window = nullptr) {
		static Engine single(*window);
		return &single;
	}
	~Engine();
	void RenderScene() override;
	void UpdateScene(const float& dt) override;

private:
	GameObject* root;
	DeferredPipeline* deferredPipeline;

	GameObject* Instantiate(std::shared_ptr<Prefab> prefab, Vector3 position = Vector3(0, 0, 0), GameObject* parent = nullptr);
	GameObject* Instantiate(std::shared_ptr<Component> component, Vector3 position = Vector3(0, 0, 0), GameObject* parent = nullptr);
	GameObject* Instantiate(Vector3 position = Vector3(0, 0, 0), GameObject* parent = nullptr);

	Engine(Win32Window& parent);

	void Initialize();
	void SetEnvironment();

	std::shared_ptr<Camera> camera;
	std::shared_ptr<Terrain> ground;
};