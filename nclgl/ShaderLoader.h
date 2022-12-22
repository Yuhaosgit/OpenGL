#pragma once
#include <unordered_map>
#include "../nclgl/OGLRenderer.h"

enum ShaderStages {
	Vertex,
	Fragment,
	Geometry,
	Domain,
	Hull,
	Automatically
};

class ShaderLoader {
public:
	static bool LoadShader(const std::string& fileName, ShaderStages shaderType = ShaderStages::Automatically);
	static std::unordered_map<std::string, GLuint> shaderObjectIDs;

	static void DeleteShaderObjects();
	static void DeleteShaderObject(const std::string& fileName);
private:
	static void	GenerateShaderObject(const std::string& fileName, const ShaderStages& shaderType);
	static bool LoadShaderFile(const std::string& filename, std::string& into);

	static GLint shaderValid;
	static std::vector<std::string> path;
};