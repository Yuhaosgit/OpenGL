#include "Decal.h"
#include "Material.h"
#include "Importer.h"
#include "Mesh.h"

const std::string Decal::Type = "Decal";
std::vector<std::weak_ptr<Decal>> Decal::decals;

void Decal::AddInstance(std::shared_ptr<Component> component) {
	decals.emplace_back(std::dynamic_pointer_cast<Decal>(component));
}

Decal::Decal() {
	mesh = Importer::MeshSet["Cube"];
	material = Importer::MaterialSet["DecalMaterial"];
}

void Decal::Render() {
	material.lock()->SubmitData();
	mesh.lock()->Draw();
}