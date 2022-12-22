/******************************************************************************
Class:Shader
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:VERY simple class to encapsulate GLSL shader loading, linking,
and binding. Useful additions to this class would be overloaded functions to
replace the glUniformxx functions in external code, and possibly a map to store
uniform names and their resulting bindings. 

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "OGLRenderer.h"

#define SHADER_MAX 5

class Texture2D;
class Shader	{
public:
	Shader(const std::string& vertex, const std::string& fragment, const std::string& geometry = "", const std::string& domain = "", const std::string& hull = "");
	~Shader(void) {}
	void	DeleteIDs();

	GLuint  GetProgram() { return programID;}

	void SubmitFloat(const float& value, const std::string& name);
	void SubmitFloat2(const float* value, const std::string& name);
	void SubmitInt(const int& value, const std::string& name);
	void SubmitBool (const bool& value, const std::string& name);
	void SubmitVector2(const Vector2& value, const std::string& name);
	void SubmitVector3(const Vector3& value, const std::string& name);
	void SubmitVector4(const Vector4& value, const std::string& name);
	void SubmitMatrix4(const Matrix4& value, const std::string& name);
protected:
	void BindShaderToProgram();
	void SetDefaultAttributes();
	void LinkProgram();

	GLuint	programID;
	GLint	programValid;

	std::string shaders[SHADER_MAX];
};

