#include "Mesh.h"
#include "Matrix2.h"
#include <thread>

using std::string;

Mesh::Mesh()	{
	glGenVertexArrays(1, &arrayObject);

	for(int i = 0; i < MAX_BUFFER; ++i) {
		bufferObject[i] = 0;
	}

	numVertices  = 0;
	type		 = GL_TRIANGLES;

	numIndices		= 0;
	vertices		= nullptr;
	textureCoords	= nullptr;
	normals			= nullptr;
	tangents		= nullptr;
	bitangents		= nullptr;
	indices			= nullptr;
	colours			= nullptr;
	weights			= nullptr;
	weightIndices	= nullptr;
}

Mesh::~Mesh(void)	{
	glDeleteVertexArrays(1, &arrayObject);			//Delete our VAO
	glDeleteBuffers(MAX_BUFFER, bufferObject);		//Delete our VBOs

	delete[]	vertices;
	delete[]	indices;
	delete[]	textureCoords;
	delete[]	tangents;
	delete[]	normals;
	delete[]	colours;
	delete[]	weights;
	delete[]	weightIndices;
}

void Mesh::Draw()	{
	glBindVertexArray(arrayObject);
	if(bufferObject[INDEX_BUFFER]) {
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else{
		glDrawArrays(type, 0, numVertices);
	}
	glBindVertexArray(0);	
}

Mesh* Mesh::GenerateQuad()	{
	Mesh* m = new Mesh();

	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices			= new Vector3[m->numVertices];
	m->textureCoords	= new Vector2[m->numVertices];
	m->colours			= new Vector4[m->numVertices];
	m->normals			= new Vector3[m->numVertices];
	m->tangents			= new Vector3[m->numVertices];

	m->vertices[0] = 	Vector3(-1.0f, 1.0f, 0.0f);
	m->vertices[1] = 	Vector3(-1.0f,	-1.0f, 0.0f);
	m->vertices[2] = 	Vector3(1.0f, 1.0f, 0.0f);
	m->vertices[3] = 	Vector3(1.0f,  -1.0f, 0.0f);

	m->textureCoords[0] = Vector2(0.0f,	1.0f);
	m->textureCoords[1] = Vector2(0.0f,	0.0f);
	m->textureCoords[2] = Vector2(1.0f,	1.0f);
	m->textureCoords[3] = Vector2(1.0f,	0.0f);

	for(int i = 0; i < 4; ++i) {
		m->colours[i] = Vector4(1.0f, 1.0f,1.0f,1.0f);
		m->normals[i] = Vector3(0.0f, 0.0f,-1.0f);
		m->tangents[i] = Vector3(1.0f, 0.0f,0.0f);
	}

	m->BufferData();

	return m; 
}

void UploadVBO(GLuint* VBO, int numberElement, int dataSize, void *data) {
	glGenBuffers(1, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, numberElement * dataSize, data, GL_STATIC_DRAW);
}

void UploadVAO(int attribSize, int attribID, int dataSize, int dataType = GL_FLOAT) {
	glVertexAttribPointer(attribID, attribSize, dataType, GL_FALSE, dataSize, 0);
	glEnableVertexAttribArray(attribID);
}

void Mesh::UploadAttribute(GLuint* VBO_ID, int numElements, int dataSize, int attribSize, int attribID, void* data, const string&debugName) {
	UploadVBO(VBO_ID, numElements, dataSize, data);
	UploadVAO(attribSize, attribID, dataSize);

	glObjectLabel(GL_BUFFER, *VBO_ID, -1, debugName.c_str());
}

void	Mesh::BufferData()	{
	glBindVertexArray(arrayObject);

	////Buffer vertex data
	UploadAttribute(&bufferObject[VERTEX_BUFFER], numVertices, sizeof(Vector3), 3, VERTEX_BUFFER, vertices, "Positions");

	if(textureCoords) {	//Buffer texture data
		UploadAttribute(&bufferObject[TEXTURE_BUFFER], numVertices, sizeof(Vector2), 2, TEXTURE_BUFFER, textureCoords, "TexCoords");
	}

	if (colours) {
		UploadAttribute(&bufferObject[COLOUR_BUFFER], numVertices, sizeof(Vector4), 4, COLOUR_BUFFER, colours, "Colours");
	}

	if (normals) {	//Buffer normal data
		UploadAttribute(&bufferObject[NORMAL_BUFFER], numVertices, sizeof(Vector3), 3, NORMAL_BUFFER, normals, "Normals");
	}

	if (tangents) {	//Buffer tangent data
		UploadAttribute(&bufferObject[TANGENT_BUFFER], numVertices, sizeof(Vector3), 3, TANGENT_BUFFER, tangents, "Tangents");
	}

	if (bitangents) {	//Buffer bitangent data
		UploadAttribute(&bufferObject[BITANGENT_BUFFER], numVertices, sizeof(Vector3), 3, BITANGENT_BUFFER, bitangents, "Bitangents");
	}

	if (weights) {		//Buffer weights data
		UploadAttribute(&bufferObject[WEIGHTVALUE_BUFFER], numVertices, sizeof(Vector4), 4, WEIGHTVALUE_BUFFER, weights, "Weights");
	}

	//Buffer weight indices data...uses a different function since its integers...
	if (weightIndices) {
		UploadVBO(&bufferObject[WEIGHTINDEX_BUFFER], numVertices, sizeof(int) * 4, weightIndices);
		glVertexAttribIPointer(WEIGHTINDEX_BUFFER, 4, GL_INT, sizeof(int)*4, 0); //note the new function...
		glEnableVertexAttribArray(WEIGHTINDEX_BUFFER);

		glObjectLabel(GL_BUFFER, bufferObject[WEIGHTINDEX_BUFFER], -1, "Weight Indices");
	}

	//buffer index data
	if(indices) {
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLuint), indices, GL_STATIC_DRAW);

		glObjectLabel(GL_BUFFER, bufferObject[INDEX_BUFFER], -1, "Indices");
	}
	glBindVertexArray(0);	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	Mesh::MultiThread(std::function<void(int,int)> generateFunc, int threadsNum) {
	vector<std::thread> threads;
	int triangleCount = GetTriCount();
	int dataSize = triangleCount / threadsNum;

	for (int i = 0; i < threadsNum - 1; ++i) {
		threads.push_back(std::thread([=] {generateFunc(dataSize * i, dataSize * (i + 1)); }));
	}
	threads.push_back(std::thread([=] {generateFunc(threadsNum - 1, triangleCount); }));

	for (std::thread& singleThread : threads) {
		if (singleThread.joinable())
			singleThread.join();
	}
}

void	Mesh::GenerateNormals() {
	if (!normals) {
		normals = new Vector3[numVertices];
	}
	auto GenerateNormal = [&](int start, int end) {
		for (int i = start; i < end; ++i) {
			unsigned int a = indices[(i * 3)];
			unsigned int b = indices[(i * 3) + 1];
			unsigned int c = indices[(i * 3) + 2];

			Vector3 normal = Vector3::Cross((vertices[b] - vertices[a]), (vertices[c] - vertices[a]));

			normals[a] += normal;
			normals[b] += normal;
			normals[c] += normal;
		}
	};
	//MultiThread(std::bind(&Mesh::GenerateNormal, this, std::placeholders::_1, std::placeholders::_2), 12);
	GenerateNormal(0, GetTriCount());

	for (int i = 0; i < numVertices; ++i) {
		normals[i].Normalise();
	}
}

void Mesh::GenerateTangents() {
	if(!textureCoords) {
		return;
	}

	if(!tangents)
		tangents = new Vector3[numVertices];
	if (!bitangents)
		bitangents = new Vector3[numVertices];

	memset(tangents, 0, sizeof(Vector3) * numVertices);
	int triCount = GetTriCount();

	auto CalculateTangentandBitangent = [&](int a, int b, int c, Vector3& tangent, Vector3& bitangent) {
		Vector3 edge1 = vertices[b] - vertices[a];
		Vector3 edge2 = vertices[c] - vertices[a];

		Vector2 deltaUV1 = textureCoords[b] - textureCoords[a];
		Vector2 deltaUV2 = textureCoords[c] - textureCoords[a];

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

		tangent.Normalise();
		bitangent.Normalise();
	};

	auto GenerateTangentandBitangent = [&](int start, int end) {
		for (int i = start; i < end; ++i) {
			unsigned int a = indices[(i * 3)];
			unsigned int b = indices[(i * 3) + 1];
			unsigned int c = indices[(i * 3) + 2];

			Vector3 tangent, bitangent;

			CalculateTangentandBitangent(a, b, c, tangent, bitangent);

			tangents[a] += tangent;
			tangents[b] += tangent;
			tangents[c] += tangent;

			bitangents[a] += bitangent;
			bitangents[b] += bitangent;
			bitangents[c] += bitangent;
		}
	};

	GenerateTangentandBitangent(0, triCount);
	//MultiThread(std::bind(&Mesh::GenerateTangent, this, std::placeholders::_1, std::placeholders::_2), 12);
}

bool	Mesh::GetVertexIndicesForTri(unsigned int i, unsigned int& a, unsigned int& b, unsigned int& c) const {
	unsigned int triCount = GetTriCount();
	if (i >= triCount) {
		return false;
	}
	
	if (numIndices > 0) {
		a = indices[(i * 3)];
		b = indices[(i * 3) + 1];
		c = indices[(i * 3) + 2];
	}
	else {
		a = (i * 3);
		b = (i * 3) + 1;
		c = (i * 3) + 2;
	}
	return true;
}