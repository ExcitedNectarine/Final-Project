#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <GL/glew.h>
#include "Vertex.h"

namespace ENG
{
	class Mesh
	{
	public:
		Mesh();
		~Mesh();
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh(Mesh&& other) = default;
		Mesh& operator=(Mesh &&) = default;

		void setVertices(const std::vector<Vertex>& new_vertices);
		std::size_t vertexCount();
		void bind();
		void unbind();

	private:
		void create(const std::vector<GLfloat>& positions, const std::vector<GLfloat>& uvs, const std::vector<GLfloat>& normals);

	private:
		GLuint id;
		std::vector<Vertex> vertices;
	};
}