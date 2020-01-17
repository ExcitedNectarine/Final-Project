#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include "Image.h"

namespace ENG
{
	class CubeMap
	{
	public:
		void create(const std::vector<std::string>& files);
		void bind();
		void unbind();
		void cleanup();

	private:
		GLuint id;
	};
}