/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Wrapper around OpenGL primitives, geometry and related 
OGL functions.

There's a couple of extra functions in here that you didn't get in the tutorial
series, to draw debug normals and tangents. 


-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OGLRenderer.h"
#include <vector>
#include <string>
#include <functional>

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
enum MeshBuffer {
	VERTEX_BUFFER	,
	COLOUR_BUFFER	, 
	TEXTURE_BUFFER	,
	NORMAL_BUFFER	, 
	TANGENT_BUFFER	,
	BITANGENT_BUFFER	,

	WEIGHTVALUE_BUFFER,		//new this year, weight values of vertices
	WEIGHTINDEX_BUFFER,	//new this year, indices of weights

	INDEX_BUFFER	,

	MAX_BUFFER
};

class Mesh	{
	friend class Importer;
	friend class Terrain;
public:	
	Mesh();
	~Mesh(void);

	void Draw();

	static Mesh*	GenerateQuad();

	void	GenerateNormals();
	void	GenerateTangents();

	unsigned int GetTriCount() const {
		int primCount = indices ? numIndices : numVertices;
		return primCount / 3;
	}

	unsigned int GetJointCount() const {
		return (unsigned int)jointNames.size();
	}

	const Matrix4* GetBindPose() const {
		return bindPose;
	}

	const Matrix4* GetInverseBindPose() const {
		return inverseBindPose;
	}
protected:
	void UploadAttribute(GLuint* id, int numElements,
		int dataSize, int attribSize, int attribID, void* pointer, const std::string& debugName);
	void	BufferData();

	void	MultiThread(std::function<void(int, int)>, int threadsNum);
	bool	GetVertexIndicesForTri(unsigned int i, unsigned int& a, unsigned int& b, unsigned int& c) const;

	GLuint	arrayObject;

	GLuint	bufferObject[MAX_BUFFER];

	GLuint	numVertices;
	GLuint	numIndices;
	
	GLuint	type;

	Vector3*		vertices;
	Vector4*		colours;
	Vector2*		textureCoords;
	Vector3*		normals;
	Vector3*		tangents;
	Vector3*		bitangents;

	Vector4*		weights;
	int*			weightIndices;

	unsigned int*	indices;

	Matrix4* bindPose;
	Matrix4* inverseBindPose;

	std::vector<std::string>	jointNames;
	std::vector<int>			jointParents;
};