#include <iostream>
#include <random>
#include <ctime>

#include "Engine.h"
#include "PositionData.h"
#include "../nclgl/Skybox.h"

GameObject* Engine::Instantiate(std::shared_ptr<Prefab> prefab, Vector3 position, GameObject* parent) {
	GameObject* instance = new GameObject();
	if (parent == nullptr)
		parent = root;

	parent->GetComponent<Transform>()->AddChild(instance->GetComponent<Transform>());
	instance->GetComponent<Transform>()->SetPosition(position);

	auto AttachMeshRender = [](GameObject* target,std::shared_ptr<Model> model) {
		auto meshRender = std::make_shared<MeshRender>();
		target->AddComponent(meshRender);

		meshRender->mesh = model->mesh;
		meshRender->material.reset(Importer::ConstructMaterial(model->textures));
	};
	
	if (prefab->models.size() == 1) {
		AttachMeshRender(instance, prefab->models[0]);
	}
	else {
		for (auto& model : prefab->models) {
			GameObject* child = new GameObject();
 			instance->GetComponent<Transform>()->AddChild(child->GetComponent<Transform>());
			AttachMeshRender(child, model);
		}
	}

	return instance;
}

GameObject* Engine::Instantiate(std::shared_ptr<Component> component, Vector3 position, GameObject* parent) {
	GameObject* instance = new GameObject();
	if (parent == nullptr)
		parent = root;
	parent->GetComponent<Transform>()->AddChild(instance->GetComponent<Transform>());
	instance->GetComponent<Transform>()->SetPosition(position);
	instance->AddComponent(component);

	return instance;
}

GameObject* Engine::Instantiate(Vector3 position, GameObject* parent) {
	GameObject* instance = new GameObject();
	if (parent == nullptr)
		parent = root;
	parent->GetComponent<Transform>()->AddChild(instance->GetComponent<Transform>());
	instance->GetComponent<Transform>()->SetPosition(position);

	return instance;
}


void Engine::Initialize() {
	auto ImportPrefab = []() {
		Importer::LoadPrefab("Cube");
		Importer::LoadPrefab("Plane");
		Importer::LoadPrefab("Sphere");
		Importer::LoadPrefab("Pegasus statue");
		Importer::LoadPrefab("Lion");
	};

	auto ImportShader = []() {
		Importer::LoadShader("Shadow", "ShadowVertex.glsl", "ShadowFragment.glsl");
		Importer::LoadShader("Combine", "CombineVertex.glsl", "CombineFragment.glsl");
		Importer::LoadShader("DirectLight", "PresentVertex.glsl", "DirectLightFragment.glsl");
		Importer::LoadShader("Gbuffer", "DefaultGbufferVertex.glsl", "DefaultGbufferFragment.glsl");
		Importer::LoadShader("Debug", "DebugVertex.glsl", "DebugFragment.glsl");
		Importer::LoadShader("IBLDiifuse", "CombineVertex.glsl", "IBLDiffuseFragment.glsl");
		Importer::LoadShader("IBLSpecularPrefilter", "CombineVertex.glsl", "IBLSpecularPrefilterFragment.glsl");
		Importer::LoadShader("IBLSpecularLUT", "PresentVertex.glsl", "IBL_LUT_Fragment.glsl");
	};

	auto ImportBakedTexture = []() {
		Importer::LoadCubemap("..\\Baked\\EnviDiffuse");
		Importer::LoadTexture("..\\Baked\\Specular\\LUT.png", false);
		Importer::LoadSpecularGI("..\\Baked\\Specular\\Prefilter");

		Importer::GetTexture("LUT")->SetMinFilter(FilterMode::LINEAR);
	};

	auto CreateCommonMaterial = []() {
		auto shadowMatertial = std::make_shared<Material>();
		shadowMatertial->shader = Importer::GetShader("Shadow");
		Importer::SetMaterial("ShadowMaterial", shadowMatertial);

		auto directLightMaterial = std::make_shared<Material>();
		directLightMaterial->shader = Importer::GetShader("DirectLight");
		Importer::SetMaterial("DirectLightMaterial", directLightMaterial);
	};

	ImportPrefab();
	ImportShader();
	ImportBakedTexture();
	CreateCommonMaterial();
}

void Engine::SetEnvironment() {
	auto CreateDefaultCamera = [&]() {
 		camera = std::make_shared<Camera>();
		GameObject* mainCamera = Instantiate(camera);

		std::shared_ptr<CameraMove> FPSMove = std::make_shared<CameraMove>();
		mainCamera->AddComponent(FPSMove);

		mainCamera->GetComponent<Transform>()->Translate(Vector3(-8, 15, 2.3));
	};

	auto CreateGround = [&]() {
		Skybox skybox("Cloud");

		ground = std::make_shared<Terrain>("Terrain.png", 1, 0.1, 4);
		Instantiate(ground);
	};

	auto CreateObjects = [&]() {
		auto cur = Instantiate(Importer::GetPrefab("Pegasus statue"), ground->GetHeight(0, 0));
		cur->GetComponent<Transform>()->SetScale(Vector3(10,10,10));

		cur = Instantiate(Importer::GetPrefab("Lion"), ground->GetHeight(0, 10));
		cur->GetComponent<Transform>()->SetScale(Vector3(1.5, 1.5, 1.5));
		cur->GetComponent<Transform>()->SetRotate(Vector3(0, -90, 0));

		cur = Instantiate(Importer::GetPrefab("Sphere"), ground->GetHeight(-5, 0));
		cur->GetComponent<Transform>()->SetScale(Vector3(1, 1, 1));
		cur->GetComponent<Transform>()->Translate(Vector3(0, 5, 0));
	};

	auto CreateLights = [&]() {
		auto directLight = Instantiate(std::make_shared<Light>());
		directLight->GetComponent<Light>()->OpenShadow();
		directLight->GetComponent<Transform>()->SetRotate(Quaternion::AxisAngleToQuaterion(60, -100, 0));
	};

	auto GlobalIlluminate = [&]() {

	};

	auto CreatePanel = [&]() {
		auto cur = Instantiate(std::make_shared<Panel>("Debug", Vector2(300, 300), Vector2(1, 1)));
		cur->AddComponent(std::make_shared<ShowFPS>());
	};

	CreateGround();
	CreateDefaultCamera();
	CreateObjects();
	CreateLights();
	CreatePanel();
	GlobalIlluminate();
}