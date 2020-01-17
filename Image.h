#pragma once

#include <string>
#include <glm/glm.hpp>
#include <stb_image.h>

namespace ENG
{
	/**
	* Stores image data, to be passed to textures.
	*/
	struct Image
	{
		void cleanup();

		glm::ivec2 size;
		int channels;
		unsigned char* pixels;
	};

	Image loadImage(const std::string& filename);
}