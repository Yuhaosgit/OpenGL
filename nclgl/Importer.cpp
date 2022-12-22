#include "Importer.h"
#include "ShaderLoader.h"
#include "Material.h"
#include "TextureLoader.h"
#include "Mesh.h"
#include <ctime>
#include <iostream>
#include <filesystem>
#include <gli/gli.hpp>
#include <algorithm>

#define Path "../Prefab/"

std::unordered_map<std::string, std::shared_ptr<Mesh>> Importer::MeshSet;
std::unordered_map<std::string, Shader*> Importer::ShaderSet;
std::unordered_map<std::string, Texture2D> Importer::TextureSet;
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
		texture.second.Delete();
	}
	TextureSet.clear();
	PrefabSet.clear();
}

std::vector<std::string> Importer::FindFiles(const std::string& fileFolder, FileType type) {
	std::vector<std::string> fileNames;
	for (auto& file : std::filesystem::directory_iterator{ fileFolder }) {
		std::string filePath = file.path().string();
		if (type == FileType::Image) {
			if (filePath.find("dds") != std::string::npos) {
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
	std::string path = Path + name;
	auto prefab = std::make_shared<Prefab>();
	PrefabSet[name] = prefab;

	auto FBXfiles = FindFile(path, ".fbx");
	if (!FBXfiles.empty()) {
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
	auto Texture = gli::flip(tex);
	if (Texture.empty())
		return 0;

	gli::gl GL(gli::gl::PROFILE_GL33);
	gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
	GLenum Target = GL.translate(Texture.target());
	GLuint TextureName = 0;
	glGenTextures(1, &TextureName);
	glBindTexture(Target, TextureName);
	glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
	glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);

	glm::tvec3<GLsizei> const Extent(Texture.extent());
	GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

	switch (Texture.target())
	{
	case gli::TARGET_1D:
		glTexStorage1D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal, Extent.x);
		break;
	case gli::TARGET_1D_ARRAY:
	case gli::TARGET_2D:
	case gli::TARGET_CUBE:
		glTexStorage2D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal,
			Extent.x, Texture.target() == gli::TARGET_2D ? Extent.y : FaceTotal);
		break;
	case gli::TARGET_2D_ARRAY:
	case gli::TARGET_3D:
	case gli::TARGET_CUBE_ARRAY:
		glTexStorage3D(
			Target, static_cast<GLint>(Texture.levels()), Format.Internal,
			Extent.x, Extent.y,
			Texture.target() == gli::TARGET_3D ? Extent.z : FaceTotal);
		break;
	default:
		assert(0);
		break;
	}

	for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
		for (std::size_t Face = 0; Face < Texture.faces(); ++Face)
			for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
			{
				GLsizei const LayerGL = static_cast<GLsizei>(Layer);
				glm::tvec3<GLsizei> Extent(Texture.extent(Level));
				Target = gli::is_target_cube(Texture.target())
					? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
					: Target;

				switch (Texture.target())
				{
				case gli::TARGET_1D:
					if (gli::is_compressed(Texture.format()))
						glCompressedTexSubImage1D(
							Target, static_cast<GLint>(Level), 0, Extent.x,
							Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
							Texture.data(Layer, Face, Level));
					else
						glTexSubImage1D(
							Target, static_cast<GLint>(Level), 0, Extent.x,
							Format.External, Format.Type,
							Texture.data(Layer, Face, Level));
					break;
				case gli::TARGET_1D_ARRAY:
				case gli::TARGET_2D:
				case gli::TARGET_CUBE:
					if (gli::is_compressed(Texture.format()))
						glCompressedTexSubImage2D(
							Target, static_cast<GLint>(Level),
							0, 0,
							Extent.x,
							Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
							Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
							Texture.data(Layer, Face, Level));
					else
						glTexSubImage2D(
							Target, static_cast<GLint>(Level),
							0, 0,
							Extent.x,
							Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
							Format.External, Format.Type,
							Texture.data(Layer, Face, Level));
					break;
				case gli::TARGET_2D_ARRAY:
				case gli::TARGET_3D:
				case gli::TARGET_CUBE_ARRAY:
					if (gli::is_compressed(Texture.format()))
						glCompressedTexSubImage3D(
							Target, static_cast<GLint>(Level),
							0, 0, 0,
							Extent.x, Extent.y,
							Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
							Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
							Texture.data(Layer, Face, Level));
					else
						glTexSubImage3D(
							Target, static_cast<GLint>(Level),
							0, 0, 0,
							Extent.x, Extent.y,
							Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
							Format.External, Format.Type,
							Texture.data(Layer, Face, Level));
					break;
				default: assert(0); break;
				}
			}
	glBindTexture(GL_TEXTURE_2D, 0);
	std::string name = fileName.substr(fileName.find_last_of("\\") + 1);
	TextureSet[name] = TextureName;
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
			material->SetTexture2D("diffuseTex", Importer::TextureSet[textures[i]]);
		}
		else if (texture.find("normal") != std::string::npos) {
			material->SetTexture2D("normalTex", Importer::TextureSet[textures[i]]);
		}
		else if (texture.find("metallic") != std::string::npos) {
			material->SetTexture2D("metallicTex", Importer::TextureSet[textures[i]]);
		}
		else if (texture.find("roughness") != std::string::npos) {
			material->SetTexture2D("roughnessTex", Importer::TextureSet[textures[i]]);
		}
	}
	return material;
}
