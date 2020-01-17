#include "CubeMap.h"

namespace ENG
{
	void CubeMap::create(const std::vector<std::string>& files)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		Image image;
		for (GLuint i = 0; i < 6; i++)
		{
			image = loadImage(files[i].c_str());
			if (!image.pixels)
				throw std::exception("apples");

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, image.size.x, image.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);
			image.cleanup();
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	void CubeMap::bind() { glBindTexture(GL_TEXTURE_CUBE_MAP, id); }
	void CubeMap::unbind() { glBindTexture(GL_TEXTURE_CUBE_MAP, id); }
	void CubeMap::cleanup() { glDeleteTextures(1, &id); }
}