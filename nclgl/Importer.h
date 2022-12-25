#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>
#include <functional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Texture.h"

class Mesh;
class Material;

enum class FileType
{
	FBX, Image
};

struct Model {
	std::weak_ptr<Mesh> mesh;
	std::vector<std::string> textures;
	std::string name;
};

struct Prefab {
	std::vector<std::shared_ptr<Model>> models;
};

class Importer {
private:
	static std::set<std::string> ShaderFiles;

public:
	static void ReleaseAllResources();

	static std::unordered_map<std::string, Shader*> ShaderSet;
	static std::unordered_map<std::string, std::shared_ptr<Mesh>> MeshSet;
	static std::unordered_map<std::string, std::shared_ptr<Material>> MaterialSet;
	static std::unordered_map<std::string, std::shared_ptr<Texture>> TextureSet;
	static std::unordered_map<std::string, std::shared_ptr<Prefab>> PrefabSet;

	static void LoadPrefab(const std::string& file);

	static std::string LoadTexture(const std::string& fileName);
	static std::string LoadCubemap(const std::string& fileName);

	static void LoadShader(const std::string& name, const std::string& vertex, const std::string& fragment,
		const std::string& geometry="", const std::string& domain = "", const std::string& hull = "");

	static std::shared_ptr<Mesh> LoadMesh(aiMesh* assimpMesh);

	static Material* ConstructMaterial
	(std::vector<std::string>& textures, Shader* shader = Importer::ShaderSet["Gbuffer"]);

	static void SetMaterial(const std::string& name, std::shared_ptr<Material> material);

	static std::vector<std::string> FindFiles(const std::string& fileFolder, FileType type);
	static std::string FindFile(const std::string& fileFolder, const std::string& file);
};