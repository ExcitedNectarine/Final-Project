#include "Shader.h"

namespace ENG
{
	Shader::Shader() {}
	Shader::Shader(const std::string& vertex, const std::string& fragment) { create(vertex, fragment); }

	void Shader::create(const std::string& vertex, const std::string& fragment)
	{
		const GLchar* cvertex = vertex.c_str();
		const GLchar* cfragment = fragment.c_str();

		// Create vertex shader.
		GLuint vertex_id = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_id, 1, &cvertex, NULL);
		glCompileShader(vertex_id);
		compileErrorCheck(vertex_id);

		// Craate fragment shader.
		GLuint fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_id, 1, &cfragment, NULL);
		glCompileShader(fragment_id);
		compileErrorCheck(fragment_id);

		// Create shader program.
		id = glCreateProgram();
		glAttachShader(id, vertex_id);
		glAttachShader(id, fragment_id);

		// Attribute binding.
		glBindAttribLocation(id, 0, "in_position");
		glBindAttribLocation(id, 1, "in_uv");
		glBindAttribLocation(id, 2, "in_normal");
		glBindAttribLocation(id, 3, "in_tangent");
		glBindAttribLocation(id, 4, "in_bitangent");

		glLinkProgram(id);
		linkErrorCheck();

		// Clean up.
		glDetachShader(id, vertex_id);
		glDetachShader(id, fragment_id);
		glDeleteShader(vertex_id);
		glDeleteShader(fragment_id);

		glUseProgram(id);
	}

	void Shader::setUniform(const std::string& name, const glm::mat4& value)
	{
		GLint uniform_id = glGetUniformLocation(id, name.c_str());
		glUseProgram(id);
		glUniformMatrix4fv(uniform_id, 1, GL_FALSE, &value[0][0]);
		glUseProgram(NULL);
	}

	void Shader::setUniform(const std::string& name, const glm::vec3& value)
	{
		GLint uniform_id = glGetUniformLocation(id, name.c_str());
		glUseProgram(id);
		glUniform3fv(uniform_id, 1, &value[0]);
		glUseProgram(NULL);
	}

	void Shader::setUniform(const std::string& name, const float value)
	{
		GLint uniform_id = glGetUniformLocation(id, name.c_str());
		glUseProgram(id);
		glUniform1f(uniform_id, value);
		glUseProgram(NULL);
	}

	void Shader::setUniform(const std::string& name, const int value)
	{
		GLint uniform_id = glGetUniformLocation(id, name.c_str());
		glUseProgram(id);
		glUniform1i(uniform_id, value);
		glUseProgram(NULL);
	}

	void Shader::bind() { glUseProgram(id); }
	void Shader::unbind() { glUseProgram(NULL); }
	void Shader::cleanUp() { glDeleteProgram(id); }

	void Shader::compileErrorCheck(const GLuint shader_id)
	{
		GLint success = 0;
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			GLint max = 0;
			glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max);
			std::vector<GLchar> log(max);
			glGetShaderInfoLog(shader_id, max, &max, &log[0]);

			throw std::exception(&log[0]);
		}
	}

	void Shader::linkErrorCheck()
	{
		GLint success = 0;
		glGetProgramiv(id, GL_LINK_STATUS, &success);

		if (!success)
		{
			GLint max = 0;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &max);
			std::vector<GLchar> log(max);
			glGetProgramInfoLog(id, max, &max, &log[0]);

			throw std::exception(&log[0]);
		}
	}
}