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
	/**
	* Simple mesh class, simply stores vertices and uploads them to the GPU. 
	*/
	class Mesh
	{
	public:
		void setVertices(const std::vector<Vertex>& new_vertices);
		std::vector<Vertex>& getVertices();
		std::size_t vertexCount();
		void bind();
		void unbind();
		void cleanup();

	private:
		void create(const std::vector<GLfloat>& positions, const std::vector<GLfloat>& uvs, const std::vector<GLfloat>& normals);

	private:
		GLuint id;
		std::vector<Vertex> vertices;
	};
}