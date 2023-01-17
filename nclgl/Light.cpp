#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "Importer.h"
#include "GameObject.h"

const std::string Light::Type = "Light";
std::vector<std::weak_ptr<Light>> Light::lights;

void Light::AddInstance(std::shared_ptr<Component> component) {
	lights.emplace_back(std::dynamic_pointer_cast<Light>(component));
}

Light::Light() :lightType(LightType::Direct), color(Vector4(1, 1, 1, 1)), range(10) {
	mesh = Importer::GetMesh("Plane");
	material = Importer::GetMaterial("DirectLightMaterial");
}

Light::Light(const Light& copy) : color(copy.color), range(copy.range), Component(copy) {
	lightType = copy.lightType;
	mesh = copy.mesh.lock();
	material = copy.material.lock();
	shadowOpen = copy.shadowOpen;
}

void Light::SetLightType(LightType type) {
	lightType = type;
	if (type == LightType::Direct) {
		mesh = Importer::GetMesh("Plane");
		material = Importer::GetMaterial("DirectLightMaterial");
	}
	else if (type == LightType::Point) {
		mesh = Importer::GetMesh("Cube");
		material = Importer::GetMaterial("PointLightMaterial");
	}
}

void Light::Render() {
	material.lock()->SubmitData();
	mesh.lock()->Draw();
}

void Light::OpenShadow() {
	shadowOpen = true;
	shadowBuffer = std::make_shared<FrameBuffer>();

	if (lightType == LightType::Direct) {
		auto shadowMap = std::make_shared<RenderTexture>
			(ShadowSize, ShadowSize, Dimension::Texture2D, RenderTextureFormat::DEPTH);
		shadowMap->SetMagFilter(FilterMode::LINEAR);
		shadowMap->SetMinFilter(FilterMode::LINEAR);

		shadowBuffer->SetRenderTexture("shadowTex", shadowMap);
	}
}

void Light::CloseShadow() {
	shadowOpen = false;
	shadowBuffer.reset();
}