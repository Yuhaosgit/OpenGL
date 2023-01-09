#include "Skybox.h"
#include "Importer.h"

std::weak_ptr<TextureCube> Skybox::skyboxTexture;

Skybox::Skybox(const std::string& fileName) {
	auto texName = Importer::LoadCubemap("../Skybox/" + fileName);
	skyboxTexture = std::dynamic_pointer_cast<TextureCube>(Importer::GetTexture(texName));
}

Skybox::Skybox(std::shared_ptr<TextureCube> cubeMap)
{
	skyboxTexture = cubeMap;
}