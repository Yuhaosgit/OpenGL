#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>
#include <functional>

#include "Texture.h"

class Mesh;
class Material;
class aiMesh;

enum class FileType
{
	FBX, DDS, PNG
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
	static std::unordered_map<std::string, std::shared_ptr<Prefab>> PrefabSet;
	static std::unordered_map<std::string, std::shared_ptr<Texture>> TextureSet;
	static std::unordered_map<std::string, std::shared_ptr<Mesh>> MeshSet;
	static std::unordered_map<std::string, Shader*> ShaderSet;
	static std::unordered_map<std::string, std::shared_ptr<Material>> MaterialSet;

public:
	static void ReleaseAllResources();
	//load prefab
	static void LoadPrefab(const std::string& file);
	static std::shared_ptr<Prefab> GetPrefab(const std::string& name);

	//load texture
	static std::string LoadTexture(const std::string& fileName, bool flip = true);
	static std::string LoadCubemap(const std::string& fileName);
	static std::string LoadSpecularGI(const std::string& fileName);
	static std::shared_ptr<Texture> GetTexture(const std::string& name);

	//load mesh
	static std::shared_ptr<Mesh> LoadMesh(aiMesh* assimpMesh);
	static std::shared_ptr<Mesh> GetMesh(const std::string& name);
	
	//load shader
	static void LoadShader(const std::string& name, const std::string& vertex, const std::string& fragment,
		const std::string& geometry = "", const std::string& domain = "", const std::string& hull = "");
	static Shader* GetShader(const std::string& name);

	//load material
	static void SetMaterial(const std::string& name, std::shared_ptr<Material> material);
	static Material* ConstructMaterial
	(std::vector<std::string>& textures, Shader* shader = Importer::ShaderSet["Gbuffer"]);
	static std::shared_ptr<Material> GetMaterial(const std::string& name);

	//functional functions
	static std::vector<std::string> FindFiles(const std::string& fileFolder, FileType type);
	static std::string FindFile(const std::string& fileFolder, const std::string& file);
};