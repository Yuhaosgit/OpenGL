#include "Skybox.h"
#include "Mesh.h"
#include "Material.h"
#include "Importer.h"

const std::string Skybox::Type = "SkyBox";
std::weak_ptr<Skybox> Skybox::skyBox;

void Skybox::AddInstance(std::shared_ptr<Component> component) {
	skyBox = std::dynamic_pointer_cast<Skybox>(component);
}

Skybox::Skybox() {
	mesh = Importer::MeshSet["Plane"];
}


void Skybox::Render() {
	material.lock()->SubmitData();
	mesh.lock()->Draw();
}