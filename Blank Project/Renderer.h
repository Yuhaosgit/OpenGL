#pragma once
#include <vector>
#include "../nclgl/GameObject.h"
#include "../nclgl/FrameBuffer.h"
#include "../nclgl/Importer.h"

class Renderer : public OGLRenderer {
public:
	static Renderer* instance(Window* window = nullptr) {
		static Renderer single(*window);
		return &single;
	}
	~Renderer();
	void RenderScene() override;
	void UpdateScene(const float& dt) override;

private:
	GameObject* root;

	GameObject* Instantiate(std::shared_ptr<Prefab> prefab, Vector3 position = Vector3(0, 0, 0), GameObject* parent = nullptr);
	GameObject* Instantiate(std::shared_ptr<Component> component, Vector3 position = Vector3(0, 0, 0), GameObject* parent = nullptr);
	GameObject* Instantiate(Vector3 position = Vector3(0, 0, 0), GameObject* parent = nullptr);

	Renderer(Window& parent);

	void Initialize();
	void SetEnvironment();

	std::shared_ptr<Camera> camera;
	std::shared_ptr<Terrain> ground;
};