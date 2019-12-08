#pragma once

#include <string>
#include <array>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <stb_image.h>

namespace ENG
{
	class CubeMap
	{
	public:
		void create(const std::array<std::string, 6>& files);
		void bind();
		void unbind();
		void cleanup();

	private:
		GLuint id;
	};
}