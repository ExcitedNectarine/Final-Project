#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace ENG
{
	class FrameBuffer
	{
	public:
		FrameBuffer();
		~FrameBuffer();

	private:
		GLuint id;
	};
}