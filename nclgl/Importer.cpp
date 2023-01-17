#include "Importer.h"
#include "ShaderLoader.h"
#include "Material.h"
#include "Mesh.h"
#include "ImageLoader.h"
#include <ctime>
#include <iostream>
#include <filesystem>
#include <gli/gli.hpp>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::unordered_map<std::string, std::shared_ptr<Mesh>> Importer::MeshSet;
std::unordered_map<std::string, Shader*> Importer::ShaderSet;
std::unordered_map<std::string, std::shared_ptr<Texture>> Importer::TextureSet;
std::unordered_map<std::string, std::shared_ptr<Material>> Importer::MaterialSet;
std::unordered_map<std::string, std::shared_ptr<Prefab>> Importer::PrefabSet;
std::set<std::string> Importer::ShaderFiles;

void Importer::ReleaseAllResources() {
	MeshSet.clear();

	for (auto& shader : ShaderSet) {
		shader.second->DeleteIDs();
		delete shader.second;
	}
	ShaderSet.clear();
	ShaderFiles.clear();

	for (auto& texture : TextureSet) {
		if (texture.second != nullptr)
			texture.second->Delete();
	}
	TextureSet.clear();
	PrefabSet.clear();
}

std::vector<std::string> Importer::FindFiles(const std::string& fileFolder, FileType type) {
	std::vector<std::string> fileNames;
	for (auto& file : std::filesystem::directory_iterator{ fileFolder }) {
		std::string filePath = file.path().string();
		if (type == FileType::DDS) {
			if (filePath.find("dds") != std::string::npos) {
				fileNames.emplace_back(filePath);
			}
		}
		else if (type == FileType::PNG) {
			if (filePath.find("png") != std::string::npos) {
				fileNames.emplace_back(filePath);
			}
		}
		else if (type == FileType::FBX) {
			if (filePath.find("fbx") != std::string::npos) {
				fileNames.emplace_back(filePath);
				break;
			}
		}
	}
	return fileNames;
};

std::string Importer::FindFile(const std::string& fileFolder, const std::string& file) {
	std::string fileName;
	for (auto& currentfile : std::filesystem::directory_iterator{ fileFolder }) {
		std::string currentFilePath = currentfile.path().string();
		if (currentFilePath.find(file) != std::string::npos) {
			fileName = currentFilePath;
		}
	}
	return fileName;
}

void Importer::LoadPrefab(const std::string& name) {
	std::string path = "../Prefab/" + name;

	auto FBXfiles = FindFile(path, ".fbx");
	if (FBXfiles.empty()) {
		std::cout << name << ".fbx " << "dosen't exist";
		return;
	}

	auto prefab = std::make_shared<Prefab>();
	PrefabSet[name] = prefab;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(FBXfiles, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate);

	auto LoadTextures = [&](unsigned int index, Model* model) {
		aiMaterial* fbxMaterial = scene->mMaterials[index];
		aiString name;

		fbxMaterial->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &name);
		if (name.length != 0)
			model->textures.emplace_back(LoadTexture(name.C_Str()));

		fbxMaterial->GetTexture(aiTextureType::aiTextureType_NORMALS, 0, &name);
		if (name.length != 0)
			model->textures.emplace_back(LoadTexture(name.C_Str()));

		fbxMaterial->GetTexture(aiTextureType::aiTextureType_METALNESS, 0, &name);
		if (name.length != 0)
			model->textures.emplace_back(LoadTexture(name.C_Str()));

		fbxMaterial->GetTexture(aiTextureType::aiTextureType_SHININESS, 0, &name);
		if (name.length != 0)
			model->textures.emplace_back(LoadTexture(name.C_Str()));
	};

	if (scene->HasMeshes()) {
		for (int i = 0; i < scene->mNumMeshes; ++i) {
			auto model = std::make_shared<Model>();
			prefab->models.emplace_back(model);
			aiMesh* fbxMesh = scene->mMeshes[i];
			model->mesh = LoadMesh(fbxMesh);
			LoadTextures(fbxMesh->mMaterialIndex, model.get());
		}
	}
}


std::shared_ptr<Mesh> Importer::LoadMesh(aiMesh* assimpMesh) {
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    int verticesNum = assimpMesh->mNumVertices;
    if (assimpMesh->HasPositions()) {
        mesh->numVertices = verticesNum;
        mesh->vertices = new Vector3[verticesNum];
        memcpy(mesh->vertices, assimpMesh->mVertices, verticesNum * sizeof(Vector3));
    }

    if (assimpMesh->HasNormals()) {
        mesh->normals = new Vector3[verticesNum];
        memcpy(mesh->normals, assimpMesh->mNormals, verticesNum * sizeof(Vector3));
    }

    if (assimpMesh->HasTangentsAndBitangents()) {
        mesh->tangents = new Vector3[verticesNum];
        mesh->bitangents = new Vector3[verticesNum];
        memcpy(mesh->tangents, assimpMesh->mTangents, verticesNum * sizeof(Vector3));
        memcpy(mesh->bitangents, assimpMesh->mBitangents, verticesNum * sizeof(Vector3));
    }

	if (assimpMesh->HasTextureCoords(0)) {
		mesh->textureCoords = new Vector2[verticesNum];
		for (int i = 0; i < verticesNum; ++i) {
			mesh->textureCoords[i].x = assimpMesh->mTextureCoords[0][i].x;
			mesh->textureCoords[i].y = assimpMesh->mTextureCoords[0][i].y;
		}
    }

    if (assimpMesh->HasVertexColors(0)) {
        mesh->colours = new Vector4[verticesNum];
        memcpy(mesh->colours, assimpMesh->mColors[0], verticesNum * sizeof(Vector4));
    }

    if (assimpMesh->HasFaces() && assimpMesh->mFaces->mNumIndices == 3) {
        int faceNum = assimpMesh->mNumFaces;
        mesh->numIndices = faceNum * 3;
        mesh->indices = new unsigned int[mesh->numIndices];
        int k = 0;
        for (int i = 0; i < faceNum; ++i) {
            for (int j = 0; j < 3; ++j) {
                mesh->indices[k++] = assimpMesh->mFaces[i].mIndices[j];
            }
        }
    }
    mesh->BufferData();
    MeshSet[assimpMesh->mName.C_Str()] = mesh;
	return mesh;
}

void Importer::LoadShader(const std::string& name, const std::string& vertex, const std::string& fragment,
    const std::string& geometry, const std::string& domain, const std::string& hull) {

	auto InsertShader = [](const std::string& file) {
		if (ShaderFiles.find(file) == ShaderFiles.end() && file != "") {
			ShaderLoader::LoadShader(file);
			ShaderFiles.insert(file);
		}
	};
	InsertShader(vertex);
	InsertShader(fragment);
	InsertShader(geometry);
	InsertShader(domain);
	InsertShader(hull);

    ShaderSet.insert({ name, new Shader(vertex, fragment, geometry, domain, hull) });
}

void Importer::SetMaterial(const std::string& name, std::shared_ptr<Material> material) {
	MaterialSet[name] = material;
}

std::string Importer::LoadTexture(const std::string& fileName) {
	gli::texture tex = gli::load(fileName);
	if (tex.empty()) {
		std::cout << "The texture: " + fileName + " does not exist";
		return "No File";
	}
	std::string name = fileName.substr(fileName.find_last_of("\\") + 1);

	auto Texture = gli::flip(tex);
	gli::gl GL(gli::gl::PROFILE_GL33);
	gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
	GLenum Target = GL.translate(Texture.target());
	GLuint TextureName = 0;

	glm::tvec3<GLsizei> const Extent(Texture.extent());

	auto IsColorTexture = [](std::string& str)->bool {
		for (auto& c : str)
			c = tolower(c);
		if (str.find("color") != std::string::npos) {
			return true;
		}
		return false;
	};

	auto TransformFormat_SRGB = [](gli::gl::internal_format in_format)->gli::gl::internal_format {
		if (in_format == gli::gl::internal_format::INTERNAL_RGB_DXT1) {
			return gli::gl::internal_format::INTERNAL_SRGB_DXT1;
		}
		else if (in_format == gli::gl::internal_format::INTERNAL_RGBA_DXT1) {
			return gli::gl::internal_format::INTERNAL_SRGB_ALPHA_DXT1;
		}
		else if (in_format == gli::gl::internal_format::INTERNAL_RGBA_DXT3) {
			return gli::gl::internal_format::INTERNAL_SRGB_ALPHA_DXT3;
		}
		else if (in_format == gli::gl::internal_format::INTERNAL_RGBA_DXT5) {
			return gli::gl::internal_format::INTERNAL_SRGB_ALPHA_DXT5;
		}
	};

	auto format = IsColorTexture(name) ? TransformFormat_SRGB(Format.Internal) : Format.Internal;
	auto texture = std::make_shared<Texture2D>(Extent.x, Extent.y);

	for (int level = 0; level < Texture.levels(); ++level) {
		texture->SetPixelData(Texture.data(0, 0, level), static_cast<GLsizei>(Texture.size(level)), level, format);
	}

	TextureSet[name] = texture;
    return name;
}

std::string Importer::LoadCubemap(const std::string& path) {
	auto AddFiles = [path](std::vector<std::string>& fileNames)->bool {
		std::string dir[6] = { "Left", "Right", "Up", "Down", "Front", "Back" };
		for (int i = 0; i < 6; ++i) {
			auto file = FindFile(path, dir[i]);
			if (file.empty()) {
				return false;
			}
			fileNames.emplace_back(file);
		}
		return true;
	};

	std::vector<std::string> fileNames;
	if (!AddFiles(fileNames)) {
		std::cout << path << " Cubemap Files can't be loaded.\n";
		return "Load Fail";
	}

	auto LoadDSS = [&](std::shared_ptr<TextureCube> texture){
		for (int i = 0; i < 6; ++i) {
			//load image information
			gli::texture Texture = (gli::load(fileNames[i]));
			auto tex = gli::flip(Texture);

			gli::gl GL(gli::gl::PROFILE_GL33);
			gli::gl::format const Format = GL.translate(tex.format(), tex.swizzles());
			GLenum Target = GL.translate(tex.target());
			glm::tvec3<GLsizei> const Extent(tex.extent());

			//test if load succeed
			if (tex.empty()) {
				std::cout << "The texture: " + fileNames[i] + " does not exist";
				return nullptr;
			}

			texture->SetSize(Extent.x, Extent.y);
			texture->SetPixelData(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				tex.data(0, 0, 0), (GLsizei)tex.size(0), 0, GL_COMPRESSED_SRGB_S3TC_DXT1_EXT);

			tex.clear();
		}
	};

	auto LoadPNG = [&](std::shared_ptr<TextureCube> texture) {
		for (int i = 0; i < 6; ++i) {
			auto image = ImageLoader::LoadPNG(fileNames[i], 3);

			if (!image.data) {
				std::cout << "The texture: " + fileNames[i] + " does not exist";
				return nullptr;
			}

			texture->SetSize(image.width, image.height);
			texture->SetPixelData(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, image.data,
				0, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
			image.Release();
		}
	};

	auto texture = std::make_shared<TextureCube>(0, 0);
	if (fileNames[0].find("dds") != std::string::npos)
		LoadDSS(texture);
	else if (fileNames[0].find("png") != std::string::npos)
		LoadPNG(texture);

	std::string name = path.substr(path.find_last_of("\\") + 1);
	TextureSet[name] = texture;
	return name;
}

Material* Importer::ConstructMaterial(std::vector<std::string>& textures, Shader* shader) {
	Material* material = new Material();
	material->shader = shader;

	for (int i = 0; i < textures.size(); ++i) {
		std::string texture = textures[i];
		for (auto& c : texture)
			c = tolower(c);

		if (texture.find("color") != std::string::npos) {
			material->SetTexture("colorTex", Importer::TextureSet[textures[i]]);
		}
		else if (texture.find("normal") != std::string::npos) {
			material->SetTexture("normalTex", Importer::TextureSet[textures[i]]);
		}
		else if (texture.find("metallic") != std::string::npos) {
			material->SetTexture("metallicTex", Importer::TextureSet[textures[i]]);
		}
		else if (texture.find("roughness") != std::string::npos) {
			material->SetTexture("roughnessTex", Importer::TextureSet[textures[i]]);
		}
	}
	return material;
}

std::shared_ptr<Prefab> Importer::GetPrefab(const std::string& name) {
	if (PrefabSet.find(name) == PrefabSet.end()) {
		std::cout << "This prefab: " << name << " doesn't exist";
		throw "Resource reading error";
		return nullptr;
	}
	return PrefabSet[name];
}

std::shared_ptr<Texture> Importer::GetTexture(const std::string& name) {
	if (TextureSet.find(name) == TextureSet.end()) {
		return nullptr;
	}
	return TextureSet[name];
}

std::shared_ptr<Mesh> Importer::GetMesh(const std::string& name) {
	if (MeshSet.find(name) == MeshSet.end()) {
		std::cout << "This mesh: " << name << " doesn't exist";
		throw "Resource reading error";
		return nullptr;
	}
	return MeshSet[name];
}

Shader* Importer::GetShader(const std::string& name) {
	if (ShaderSet.find(name) == ShaderSet.end()) {
		std::cout << "This shader: " << name << " doesn't exist";
		throw "Resource reading error";
		return nullptr;
	}
	return ShaderSet[name];
}

std::shared_ptr<Material> Importer::GetMaterial(const std::string& name) {
	if (MaterialSet.find(name) == MaterialSet.end()) {
		std::cout << "This material: " << name << " doesn't exist";
		throw "Resource reading error";
		return nullptr;
	}
	return MaterialSet[name];
}