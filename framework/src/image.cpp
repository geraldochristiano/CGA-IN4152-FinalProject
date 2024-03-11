#include "image.h"
// Suppress warnings in third-party code.
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
DISABLE_WARNINGS_POP()
#include <cassert>
#include <exception>
#include <iostream>
#include <string>


// write image to a file
void Image::writeBitmapToFile(const std::filesystem::path& filePath) {
    std::string filePathString = filePath.string();
    stbi_write_bmp(filePathString.c_str(), width, height, channels, pixels.data());
}

// Image constructor, create image from file
Image::Image(const std::filesystem::path& filePath)
{
	if (!std::filesystem::exists(filePath)) {
		std::cerr << "Texture file " << filePath << " does not exist!" << std::endl;
		throw std::exception();
	}

	const auto filePathStr = filePath.string(); // Create l-value so c_str() is safe.
	stbi_uc* stbPixels = stbi_load(filePathStr.c_str(), &width, &height, &channels, STBI_default);

	if (!stbPixels) {
		std::cerr << "Failed to read texture " << filePath << " using stb_image.h" << std::endl;
		throw std::exception();
	}

	for (size_t i = 0UL; i < width * height * channels; i++) { pixels.emplace_back(stbPixels[i]); }

	stbi_image_free(stbPixels);
}
