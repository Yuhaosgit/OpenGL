#include <iostream>
#include <random>
#include <ctime>

#include "Renderer.h"
#include "PositionData.h"

Renderer::~Renderer() {
	Importer::ReleaseAllResources();
	delete root;
}

GameObject* Renderer::Instantiate(std::shared_ptr<Prefab> prefab, Vector3 position, GameObject* parent) {
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

GameObject* Renderer::Instantiate(std::shared_ptr<Component> component, Vector3 position, GameObject* parent) {
	GameObject* instance = new GameObject();
	if (parent == nullptr)
		parent = root;
	parent->GetComponent<Transform>()->AddChild(instance->GetComponent<Transform>());
	instance->GetComponent<Transform>()->SetPosition(position);
	instance->AddComponent(component);

	return instance;
}

GameObject* Renderer::Instantiate(Vector3 position, GameObject* parent) {
	GameObject* instance = new GameObject();
	if (parent == nullptr)
		parent = root;
	parent->GetComponent<Transform>()->AddChild(instance->GetComponent<Transform>());
	instance->GetComponent<Transform>()->SetPosition(position);

	return instance;
}


void Renderer::Initialize() {
	auto ImportPrefab = []() {
		Importer::LoadPrefab("Cube");
		Importer::LoadPrefab("Plane");
		Importer::LoadPrefab("Pegasus statue");
	};

	auto ImportShader = []() {
		Importer::LoadShader("Shadow", "ShadowVertex.glsl", "ShadowFragment.glsl");
		Importer::LoadShader("Combine", "CombineVertex.glsl", "CombineFragment.glsl");
		Importer::LoadShader("OIT", "CombineVertex.glsl", "OITFragment.glsl");
		Importer::LoadShader("Present", "CombineVertex.glsl", "PresentFragment.glsl");
		Importer::LoadShader("Water", "Water_vertex.glsl", "Water_fragment.glsl");
		Importer::LoadShader("Decal", "UnlitVertex.glsl", "UnlitFragment.glsl");
		Importer::LoadShader("DirectLight", "CombineVertex.glsl", "DirectLightFragment.glsl");
		Importer::LoadShader("PointLight", "UnlitVertex.glsl", "PointLightFragment.glsl");
		Importer::LoadShader("Gbuffer", "DefaultGbufferVertex.glsl", "DefaultGbufferFragment.glsl");
		Importer::LoadShader("Debug", "DebugVertex.glsl", "DebugFragment.glsl");
	};

	auto CreateCommonMaterial = []() {
		auto shadowMatertial = std::make_shared<Material>();
		shadowMatertial->shader = Importer::ShaderSet["Shadow"];
		Importer::SetMaterial("ShadowMaterial", shadowMatertial);

		//auto decalMaterial = std::make_shared<Material>();
		//decalMaterial->shader = Importer::ShaderSet["Decal"];
		//decalMaterial->SetTexture("stencilDepthTex", FrameBuffer<GeometryFBO>::instance()->stencilDepthTarget);
		//decalMaterial->SetTexture("normals", FrameBuffer<GeometryFBO>::instance()->normalTarget);
		//decalMaterial->SetTexture("targetTex", Importer::TextureSet["doge.png"]);
		//decalMaterial->SetTexture("targetNormalTex", Importer::TextureSet["dogeNormal.png"]);
		//Importer::SetMaterial("DecalMaterial", decalMaterial);

		auto directLightMaterial = std::make_shared<Material>();
		directLightMaterial->shader = Importer::ShaderSet["DirectLight"];
		directLightMaterial->SetTexture("normTex", FrameBuffer<GeometryFBO>::instance()->normalTarget);
		directLightMaterial->SetTexture("depthTex", FrameBuffer<GeometryFBO>::instance()->stencilDepthTarget);
		directLightMaterial->SetTexture("shadowTex", FrameBuffer<ShadowFBO>::instance()->depthTarget);
		Importer::SetMaterial("DirectLightMaterial", directLightMaterial);

		auto pointLightMaterial = std::make_shared<Material>();
		pointLightMaterial->shader = Importer::ShaderSet["PointLight"];
		pointLightMaterial->SetTexture("normTex", FrameBuffer<GeometryFBO>::instance()->normalTarget);
		pointLightMaterial->SetTexture("depthTex", FrameBuffer<GeometryFBO>::instance()->stencilDepthTarget);
		Importer::SetMaterial("PointLightMaterial", pointLightMaterial);

		auto combineMaterial = std::make_shared<Material>();
		combineMaterial->shader = Importer::ShaderSet["Combine"];
		combineMaterial->SetTexture("diffuseTex", FrameBuffer<GeometryFBO>::instance()->colorTarget);
		combineMaterial->SetTexture("diffuseLight", FrameBuffer<LightFBO>::instance()->diffuseTarget);
		combineMaterial->SetTexture("specularLight", FrameBuffer<LightFBO>::instance()->specularTareget);
		combineMaterial->SetTexture("depthTex", FrameBuffer<GeometryFBO>::instance()->stencilDepthTarget);
		Importer::SetMaterial("CombineMaterial", combineMaterial);
	};

	ImportPrefab();
	ImportShader();
	CreateCommonMaterial();
}

void Renderer::SetEnvironment() {
	auto CreateDefaultCamera = [&]() {
		camera = std::make_shared<Camera>();
		GameObject* mainCamera = Instantiate(camera);

		std::shared_ptr<CameraMove> FPSMove = std::make_shared<CameraMove>();
		mainCamera->AddComponent(FPSMove);

		std::shared_ptr<Skybox> skybox = std::make_shared<Skybox>("Daytime");
		mainCamera->AddComponent(skybox);

		mainCamera->GetComponent<Transform>()->Translate(Vector3(-8, 15, 2.3));
	};

	auto CreateGround = [&]() {
		ground = std::make_shared<Terrain>("Terrain.png", 1, 0.1, 4);
		Instantiate(ground);
	};

	auto CreateObjects = [&]() {
		auto cur = Instantiate(Importer::PrefabSet["Pegasus statue"], ground->GetHeight(0, 0));
		cur->GetComponent<Transform>()->SetScale(Vector3(10,10,10));
	};

	auto CreateLights = [&]() {
		auto directLight = Instantiate(std::make_shared<Light>());
		directLight->GetComponent<Light>()->shadowOpen = true;
		directLight->GetComponent<Transform>()->SetRotate(Quaternion::AxisAngleToQuaterion(45, -70, 0));
	};

	CreateGround();
	CreateDefaultCamera();
	CreateObjects();
	CreateLights();
}