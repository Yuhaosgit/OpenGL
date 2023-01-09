#pragma once
#include <string>
#include "GI.h"

class Exporter {
public:
	static void ExportCubemap(const std::string& fileName, IBL* ibl);
	static void WriteToFile(const std::string& path, int width, int height, int channels, void* data);
};