#include "CubeMap.h"

namespace ENG
{
	void CubeMap::create(const std::array<std::string, 6>& files)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);

		glm::ivec2 size;
		int channels;
		unsigned char* data;

		for (GLuint i = 0; i < 6; i++)
		{
			data = stbi_load(files[i].c_str(), &size.x, &size.y, &channels, 4);
			if (!data)
				throw std::exception("apples");

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
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