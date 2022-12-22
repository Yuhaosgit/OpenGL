#include "Shader.h"
#include "Mesh.h"
#include "ShaderLoader.h"
#include "Texture.h"
#include <iostream>

using std::string;
using std::cout;
using std::ifstream;

void AttachShaderToProgram(const GLuint& programID, const std::string& fileName);
void PrintLinkLog(GLuint program);

string ShaderNames[SHADER_MAX] = {
	"Vertex",
	"Fragment",
	"Geometry",
	"Tess. Control",
	"Tess. Eval"
};

Shader::Shader(const string& vertex, const string& fragment, const string& geometry, const string& domain, const string& hull)	{
	shaders[Vertex]		= vertex;
	shaders[Fragment]	= fragment;
	shaders[Geometry]	= geometry;
	shaders[Domain]		= domain;
	shaders[Hull]		= hull;

	programID = glCreateProgram();
	BindShaderToProgram();
}

void AttachShaderToProgram(const GLuint& programID, const std::string& fileName) {
	glAttachShader(programID, ShaderLoader::shaderObjectIDs[fileName]);
}

void PrintLinkLog(GLuint program) {
	int logLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength) {
		char* tempData = new char[logLength];
		glGetProgramInfoLog(program, logLength, NULL, tempData);
		std::cout << "Link Log:\n" << tempData << std::endl;
		delete[] tempData;
	}
}


void Shader::BindShaderToProgram() {
	for (int i = 0; i < SHADER_MAX; ++i) {
		if (!shaders[i].empty()) {
			AttachShaderToProgram(programID, shaders[i]);
		}
	}
	SetDefaultAttributes();
	LinkProgram();
	PrintLinkLog(programID);
}

void Shader::LinkProgram()	{
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &programValid);
}

void Shader::SetDefaultAttributes()	{
	glBindAttribLocation(programID, VERTEX_BUFFER,  "position");
	glBindAttribLocation(programID, COLOUR_BUFFER,  "colour");
	glBindAttribLocation(programID, NORMAL_BUFFER,  "normal");
	glBindAttribLocation(programID, TANGENT_BUFFER, "tangent");
	glBindAttribLocation(programID, BITANGENT_BUFFER, "bitangent");
	glBindAttribLocation(programID, TEXTURE_BUFFER, "texCoord");

	glBindAttribLocation(programID, WEIGHTVALUE_BUFFER, "jointWeights");
	glBindAttribLocation(programID, WEIGHTINDEX_BUFFER, "jointIndices");
}

void Shader::DeleteIDs() {
	if (!programID) {
		return;
	}
	for (int i = 0; i < SHADER_MAX; ++i) {
		if (!shaders[i].empty()) {
			glDetachShader(programID, ShaderLoader::shaderObjectIDs[shaders[i]]);
			ShaderLoader::DeleteShaderObject(shaders[i]);
		}
	}
	glDeleteProgram(programID);
	programID = 0;
}

void Shader::SubmitFloat(const float& value, const std::string& name) {
	glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void Shader::SubmitFloat2(const float* value, const std::string& name) {
	glUniform2f(glGetUniformLocation(programID, name.c_str()), value[0], value[1]);
}
void Shader::SubmitInt(const int& value, const std::string& name) {
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}
void Shader::SubmitBool(const bool& value, const std::string& name) {
	glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}
void Shader::SubmitVector2(const Vector2& value, const std::string& name) {
	glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, (float*)&value);
}
void Shader::SubmitVector3(const Vector3& value, const std::string& name) {
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, (float*)&value);
}
void Shader::SubmitVector4(const Vector4& value, const std::string& name) {
	glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, (float*)&value);
}
void Shader::SubmitMatrix4(const Matrix4& mat, const std::string& name) {
	glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, mat.values);
}

