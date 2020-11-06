#include "Mesh.h"
#include "Output.h"

#include <glm/gtx/string_cast.hpp>

namespace ENG
{
	void Mesh::setVertices(const std::vector<Vertex>& new_vertices)
	{
		vertices = new_vertices;
		create();
		update();
	}

	void Mesh::create()
	{
		// Create buffers.
		glGenBuffers(1, &position_id);
		glGenBuffers(1, &uv_id);
		glGenBuffers(1, &normal_id);
		glGenBuffers(1, &colour_id);

		// Create vertex array.
		glGenVertexArrays(1, &id);
		glBindVertexArray(id);

		// Bind position buffer to vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, position_id);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		// Bind UV buffer to vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, uv_id);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(1);

		// Bind normal buffer to vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, normal_id);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(2);

		// Bind colour buffer to vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, colour_id);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindVertexArray(NULL);
	}

	void Mesh::update()
	{
		std::vector<GLfloat> positions;
		std::vector<GLfloat> uvs;
		std::vector<GLfloat> normals;
		std::vector<GLfloat> colours;

		glm::vec3 min(0.0f);
		glm::vec3 max(0.0f);

		// Add vertices to buffers.
		for (const Vertex& vertex : vertices)
		{
			min = glm::min(vertex.position, min);
			max = glm::max(vertex.position, max);

			positions.push_back(vertex.position.x);
			positions.push_back(vertex.position.y);
			positions.push_back(vertex.position.z);

			uvs.push_back(vertex.uv.x);
			uvs.push_back(vertex.uv.y);

			normals.push_back(vertex.normal.x);
			normals.push_back(vertex.normal.y);
			normals.push_back(vertex.normal.z);

			colours.push_back(vertex.colour.x);
			colours.push_back(vertex.colour.y);
			colours.push_back(vertex.colour.z);
		}

		size = max - min;

		// Upload positions
		glBindBuffer(GL_ARRAY_BUFFER, position_id);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), &positions.at(0), GL_STATIC_DRAW);

		// Upload UVs
		glBindBuffer(GL_ARRAY_BUFFER, uv_id);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), &uvs.at(0), GL_STATIC_DRAW);

		// Upload normals
		glBindBuffer(GL_ARRAY_BUFFER, normal_id);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals.at(0), GL_STATIC_DRAW);

		// Upload colours
		glBindBuffer(GL_ARRAY_BUFFER, colour_id);
		glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(GLfloat), &colours.at(0), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}

	void Mesh::cleanup()
	{
		glDeleteBuffers(1, &position_id);
		glDeleteBuffers(1, &uv_id);
		glDeleteBuffers(1, &normal_id);
		glDeleteBuffers(1, &colour_id);
		glDeleteVertexArrays(1, &id);
	}

	Vertex& Mesh::operator[](const int index)
	{
		dirty = true;
		return vertices[index];
	}

	std::size_t Mesh::vertexCount() { return vertices.size(); }

	void Mesh::bind()
	{
		if (dirty)
		{
			update();
			dirty = false;
		}
		glBindVertexArray(id);
	}
	void Mesh::unbind() { glBindVertexArray(NULL); }
	glm::vec3 Mesh::getSize() { return size; }

	// 2D

	void Mesh2D::setVertices(const std::vector<Vertex2D>& new_vertices)
	{
		vertices = new_vertices;
		create();
		update();
	}

	void Mesh2D::create()
	{
		// Create buffers.
		glGenBuffers(1, &position_id);
		glGenBuffers(1, &uv_id);

		// Create vertex array.
		glGenVertexArrays(1, &id);
		glBindVertexArray(id);

		// Bind position buffer to vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, position_id);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		// Bind UV buffer to vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, uv_id);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindVertexArray(NULL);
	}

	void Mesh2D::update()
	{
		std::vector<GLfloat> positions;
		std::vector<GLfloat> uvs;

		glm::vec2 min(0.0f);
		glm::vec2 max(0.0f);

		// Add vertices to buffers.
		for (const Vertex2D& vertex : vertices)
		{
			min = glm::min(vertex.position, min);
			max = glm::max(vertex.position, max);

			positions.push_back(vertex.position.x);
			positions.push_back(vertex.position.y);

			uvs.push_back(vertex.uv.x);
			uvs.push_back(vertex.uv.y);
		}

		size = max - min;

		// Upload positions
		glBindBuffer(GL_ARRAY_BUFFER, position_id);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), &positions.at(0), GL_STATIC_DRAW);

		// Upload UVs
		glBindBuffer(GL_ARRAY_BUFFER, uv_id);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), &uvs.at(0), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}

	void Mesh2D::cleanup()
	{
		glDeleteBuffers(1, &position_id);
		glDeleteBuffers(1, &uv_id);
		glDeleteVertexArrays(1, &id);
	}

	std::vector<Vertex2D>& Mesh2D::getVertices()
	{
		return vertices;
	}

	Vertex2D& Mesh2D::operator[](const int index)
	{
		dirty = true;
		return vertices[index];
	}

	std::size_t Mesh2D::vertexCount() { return vertices.size(); }

	void Mesh2D::bind()
	{
		if (dirty)
		{
			update();
			dirty = false;
		}
		glBindVertexArray(id);
	}

	void Mesh2D::unbind() { glBindVertexArray(NULL); }
	glm::vec2 Mesh2D::getSize() { return size; }
}