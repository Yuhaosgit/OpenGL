#pragma once
#include <string>
#include <gli/gli.hpp>
#include <vector>

class UncompressedImage {
public:
	UncompressedImage(const std::string& fileName, int channel, bool flip);
	~UncompressedImage();

	int GetWidth() {
		return width; 
	}
	int GetHeight() {
		return height; 
	}
	unsigned char* GetData() {
		return data;
	}
	bool Empty() {
		return data == nullptr;
	}

private:
	int width = 0;
	int height = 0;
	unsigned char* data;
};

class CompressedImage {
public:
	CompressedImage(const std::string& fileName, bool flip);
	~CompressedImage();

	int GetWidth(int in_mipMapLevel) {
		if (in_mipMapLevel > mipMapLevel) {
			throw "This mipmap level exceed in the texture";
		}

		return image.extent().x;
	}

	int GetHeight(int in_mipMapLevel) {
		if (in_mipMapLevel > mipMapLevel) {
			throw "This mipmap level exceed in the texture";
		}
		return image.extent().y;
	}

	void* GetData(int in_mipMapLevel = 0) {
		if (in_mipMapLevel > mipMapLevel) {
			throw "This mipmap level exceed in the texture";
		}
		return image.data(0, 0, in_mipMapLevel);
	}

	gli::gl::internal_format GetFormat(bool sRGB) {
		if (sRGB)
			return TransformFormat_SRGB(format.Internal);
		return format.Internal;
	}

	int GetMipmapLevel() {
		return mipMapLevel;
	}

	GLsizei GetDataSize(int in_mipMapLevel) {
		return static_cast<GLsizei>(image.size(in_mipMapLevel));
	}

	bool Empty() {
		return image.empty();
	}
private:
	int mipMapLevel = 0;

	gli::texture image;
	gli::gl::format format;
	gli::gl::internal_format TransformFormat_SRGB(gli::gl::internal_format in_format);
};