#pragma once
#include <string>
#include <gli/gli.hpp>

struct PNGInfo {
	int width;
	int height;
	int channels;
	unsigned char* data;

	void Release();
};

class ImageLoader {
public:
	static PNGInfo LoadPNG(const std::string& fileName);
};