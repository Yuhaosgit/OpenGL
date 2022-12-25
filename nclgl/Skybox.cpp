#include "Skybox.h"
#include "Importer.h"

#define Path "../Skybox/"

const std::string Skybox::Type = "SkyBox";

Skybox::Skybox(const std::string& fileName) {
	auto texName = Importer::LoadCubemap(Path + fileName);
	skyboxTexture = std::dynamic_pointer_cast<TextureCube>(Importer::TextureSet[texName]);
}

Skybox::Skybox(std::shared_ptr<TextureCube> cubeMap)
{
	skyboxTexture = cubeMap;
}