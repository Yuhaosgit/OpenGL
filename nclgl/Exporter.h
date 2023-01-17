#pragma once
#include <string>

class RenderTexture;

class Exporter {
public:
	static void ExportCubemap(const std::string& path, RenderTexture* exportingTexture, int mipLevel = 0);
	static void ExportTexture2D(const std::string& path, const std::string& fileName,RenderTexture* exportingTexture, int mipLevel = 0);
	static void CheckFolderExist(const std::string& path);
};