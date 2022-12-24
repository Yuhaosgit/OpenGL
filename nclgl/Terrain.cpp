#include "Terrain.h"
#include "Importer.h"
#include "Material.h"
#include <iostream>
#include <random>
#include <thread>

const std::string Terrain::Type = "Terrain";
std::vector<std::weak_ptr<Terrain>> Terrain::terrains;

void Terrain::AddInstance(std::shared_ptr<Component> component) {
	terrains.emplace_back(std::dynamic_pointer_cast<Terrain>(component));
	MeshRender::meshRenders.emplace_back(std::dynamic_pointer_cast<MeshRender>(component));
}

Terrain::Terrain(const std::string& name, const float& stretchFlat, const float& stretchHeight, const float& texScale) {
	int channels;
	auto path = Importer::FindFile("../Terrain", name);

	GLubyte* data = SOIL_load_image(path.c_str(), &width, &height, &channels, 1);

	if (!data) {
		std::cout << "Heightmap  can't load  file!\n";
		return;
	}

	auto terrainMesh = std::make_shared<Mesh>();
	Importer::MeshSet["Terrain"] = terrainMesh;

	mesh = terrainMesh;
	int numVertices = width * height;

	mesh.lock()->numVertices = numVertices;
	mesh.lock()->numIndices = (width - 1) * (height - 1) * 6;

	mesh.lock()->vertices = new Vector3[numVertices];
	mesh.lock()->textureCoords = new Vector2[numVertices];
	mesh.lock()->indices = new GLuint[mesh.lock()->numIndices];

	Vector3 vertex_scale = Vector3(stretchFlat, stretchHeight, stretchFlat);
	Vector2 texture_scale = Vector2(1.0f / texScale, 1.0f / texScale);

	//generate vertices
	int dataSize = height / 8;
	vector<std::thread> threads;

	auto CalculateVertices = [&](Vector3 vertex_scale, Vector2 texture_scale, int start, int end, GLubyte* data) {
		for (int z = start; z < end; ++z) {
			for (int x = 0; x < width; ++x) {
				int offset = z * height + x;
				mesh.lock()->vertices[offset] = Vector3(x - width / 2, data[offset], z - height / 2) * vertex_scale;
				mesh.lock()->textureCoords[offset] = Vector2(x - width / 2, z- height / 2) * texture_scale;
			}
		}
	};

	for (int i = 0; i < 8; ++i) {
		int start = dataSize * i;
		int end = dataSize * (i + 1);
		threads.push_back(std::thread([=]
			{CalculateVertices(vertex_scale, texture_scale, dataSize * i, dataSize * (i + 1), data); }));
	}

	for (std::thread& singleThread : threads) {
		if (singleThread.joinable())
			singleThread.join();
	}

	SOIL_free_image_data(data);


	auto CalculateTriangles = [&]() {
		int i = 0;

		for (int z = 0; z < height - 1; ++z) {
			for (int x = 0; x < width - 1; ++x) {
				int a = (z * height) + x;
				int b = (z * height) + (x + 1);
				int c = ((z + 1) * height) + (x + 1);
				int d = ((z + 1) * height) + x;

				mesh.lock()->indices[i++] = a;
				mesh.lock()->indices[i++] = d;
				mesh.lock()->indices[i++] = c;

				mesh.lock()->indices[i++] = a;
				mesh.lock()->indices[i++] = c;
				mesh.lock()->indices[i++] = b;
			}
		}
	};

	CalculateTriangles();
	mesh.lock()->GenerateNormals();
	mesh.lock()->GenerateTangents();

	mesh.lock()->BufferData();
	
	auto textures = Importer::FindFiles("../Terrain", FileType::Image);
	for (std::string& texture : textures) {
		texture = Importer::LoadTexture(texture);
	}
	material.reset(Importer::ConstructMaterial(textures));
}


Vector3 Terrain::GetHeight(int xPos, int zPos) const {
	int x = -width / 2, z = -height / 2;
	for (int i = 0; i < width; ++i) {
		if (xPos <= mesh.lock()->vertices[i].x) {
			x = i;
			break;
		}
	}

	for (int i = 0; i < height; ++i) {
		if (zPos <= mesh.lock()->vertices[width * i].z) {
			z = i;
			break;
		}
	}

	unsigned int index = z * width + x;
	return mesh.lock()->vertices[index];
}