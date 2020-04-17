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
		void create();
		void update();
		void cleanup();
		Vertex& operator[](const int index);
		std::size_t vertexCount();
		void bind();
		void unbind();
		glm::vec3 getSize();

	private:
		GLuint id, position_id, uv_id, normal_id;
		bool dirty = false;
		std::vector<Vertex> vertices;
		glm::vec3 size;
	};

	class Mesh2D
	{
	public:
		void setVertices(const std::vector<Vertex2D>& new_vertices);
		void create();
		void update();
		void cleanup();
		Vertex2D& operator[](const int index);
		std::size_t vertexCount();
		void bind();
		void unbind();
		glm::vec2 getSize();

	private:
		GLuint id, position_id, uv_id;
		bool dirty = false;
		std::vector<Vertex2D> vertices;
		glm::vec2 size;
	};
}