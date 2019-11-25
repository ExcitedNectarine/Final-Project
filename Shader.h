#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace ENG
{
	class Shader
	{
	public:
		Shader(const std::string& vertex, const std::string& fragment);
		void bind();
		void unbind();
		void setUniform(const std::string& name, const glm::mat4& value);
		void setUniform(const std::string& name, const glm::vec3& value);
		void setUniform(const std::string& name, const float value);
		void setUniform(const std::string& name, const int value);

	private:
		void compileErrorCheck(const GLuint shader_id);
		void linkErrorCheck();

	private:
		GLuint id;
	};
}