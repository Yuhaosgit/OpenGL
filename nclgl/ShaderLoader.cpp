#include "ShaderLoader.h"
#include <filesystem>
#include <memory>

std::unordered_map<std::string, GLuint> ShaderLoader::shaderObjectIDs;
GLint ShaderLoader::shaderValid;
std::vector<std::string> ShaderLoader::path;

void PrintCompileLog(GLuint object);
GLuint AutoGetShaderType(const std::string& fileName);

GLuint shaderTypes[5] = {
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER
};

void ShaderLoader::DeleteShaderObjects() {
	for (auto& object : shaderObjectIDs) {
		glDeleteShader(object.second);
	}
	shaderObjectIDs.clear();
}

void ShaderLoader::DeleteShaderObject(const std::string& fileName) {
	glDeleteShader(shaderObjectIDs[fileName]);
	shaderObjectIDs.erase(fileName);
}

std::vector<std::string> GetAllFiles(const std::string& path) {
	std::vector<std::string> files;
	for (auto& file : std::filesystem::directory_iterator{ path }) {
		std::string filePath = file.path().string();
		files.emplace_back(filePath.substr(filePath.find_last_of("\\") + 1));
	}
	return files;
}

bool ShaderLoader::LoadShader(const std::string& fileName, ShaderStages shaderStage) {
	if (path.empty())
		path = GetAllFiles("..\\Shaders");

	std::shared_ptr<GLuint> shaderObjectID(new GLuint(0));
	shaderObjectIDs[fileName] = *shaderObjectID.get();

	GenerateShaderObject(fileName, shaderStage);
	return shaderValid;
}

void ShaderLoader::GenerateShaderObject(const std::string& fileName, const ShaderStages& shaderStage) {
	std::cout << "Compiling Shader...\n";

	std::string shaderText;
	if (!LoadShaderFile(fileName, shaderText)) {
		std::cout << "Loading failed!\n";
		shaderValid = false;
		return;
	}

	if (shaderStage == ShaderStages::Automatically) {
		shaderObjectIDs[fileName] = glCreateShader(AutoGetShaderType(fileName));
	}
	else {
		shaderObjectIDs[fileName] = glCreateShader(shaderTypes[shaderStage]);
	}

	const char* chars = shaderText.c_str();
	int textLength = (int)shaderText.length();
	glShaderSource(shaderObjectIDs[fileName], 1, &chars, &textLength);
	glCompileShader(shaderObjectIDs[fileName]);

	glGetShaderiv(shaderObjectIDs[fileName], GL_COMPILE_STATUS, &shaderValid);

	if (!shaderValid) {
		std::cout << "Compiling failed!\n";
		PrintCompileLog(shaderObjectIDs[fileName]);
	}
	else {
		std::cout << "Compiling success!\n\n";
	}

	glObjectLabel(GL_SHADER, shaderObjectIDs[fileName], -1, fileName.c_str());
}

bool ShaderLoader::LoadShaderFile(const std::string& filename, std::string& into) {
	std::ifstream file;
	for (int i = 0; i < path.size(); ++i) {
		file.open(SHADERDIR + path[i]+ '/' + filename);
		if (file.is_open())
			break;
	}
	std::string	textLine;

	std::cout << "Loading shader text from " << filename << "\n\n";

	if (!file.is_open()) {
		std::cout << "ERROR ERROR ERROR ERROR: File " + filename + " does not exist!\n";
		return false;
	}
	int lineNum = 1;
	while (!file.eof()) {
		getline(file, textLine);
		textLine += "\n";
		into += textLine;
		//std::cout << "(" << lineNum << ") :" << textLine;
		++lineNum;
	}
	//std::cout << "\nLoaded shader text!\n\n";
	return true;
}

void PrintCompileLog(GLuint object) {
	int logLength = 0;
	glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength) {
		char* tempData = new char[logLength];
		glGetShaderInfoLog(object, logLength, NULL, tempData);
		std::cout << "Compile Log:\n" << tempData << std::endl;
		delete[] tempData;
	}
}

GLuint AutoGetShaderType(const std::string& fileName) {
	int position = fileName.find('.');
	
	auto ToLower = [](const std::string& target) {
		std::string lowerString;
		for (int i = 0; i < target.size(); ++i) {
			lowerString.push_back(tolower(target[i]));
		}
		return lowerString;
	};

	if (ToLower(fileName.substr(position - 6, 6)) == "vertex") {
		return shaderTypes[0];
	}
	else if (ToLower(fileName.substr(position - 8, 8)) == "fragment") {
		return shaderTypes[1];
	}
	else if (ToLower(fileName.substr(position - 8, 8)) == "geometry") {
		return shaderTypes[2];
	}
	else if (ToLower(fileName.substr(position - 6, 6)) == "domain") {
		return shaderTypes[3];
	}
	else if (ToLower(fileName.substr(position - 4, 4)) == "hull") {
		return shaderTypes[4];
	}
}