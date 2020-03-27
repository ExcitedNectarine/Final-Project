#include "Mesh.h"

namespace ENG
{
	void Mesh::setVertices(const std::vector<Vertex>& new_vertices)
	{
		dirty = true;
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

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
		glBindVertexArray(NULL);
	}

	void Mesh::update()
	{
		std::vector<GLfloat> positions;
		std::vector<GLfloat> uvs;
		std::vector<GLfloat> normals;

		// Add vertices to buffers.
		for (const Vertex& vertex : vertices)
		{
			positions.push_back(vertex.position.x);
			positions.push_back(vertex.position.y);
			positions.push_back(vertex.position.z);

			uvs.push_back(vertex.uv.x);
			uvs.push_back(vertex.uv.y);

			normals.push_back(vertex.normal.x);
			normals.push_back(vertex.normal.y);
			normals.push_back(vertex.normal.z);
		}

		// Upload positions
		glBindBuffer(GL_ARRAY_BUFFER, position_id);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), &positions.at(0), GL_STATIC_DRAW);

		// Upload UVs
		glBindBuffer(GL_ARRAY_BUFFER, uv_id);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), &uvs.at(0), GL_STATIC_DRAW);

		// Upload normals
		glBindBuffer(GL_ARRAY_BUFFER, normal_id);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals.at(0), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, NULL);
	}

	void Mesh::cleanup()
	{
		glDeleteBuffers(1, &position_id);
		glDeleteBuffers(1, &uv_id);
		glDeleteBuffers(1, &normal_id);
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

	// 2D

	void Mesh2D::setVertices(const std::vector<Vertex2D>& new_vertices)
	{
		dirty = true;
		vertices = new_vertices;
		update();
		create();
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

		// Add vertices to buffers.
		for (const Vertex2D& vertex : vertices)
		{
			positions.push_back(vertex.position.x);
			positions.push_back(vertex.position.y);

			uvs.push_back(vertex.uv.x);
			uvs.push_back(vertex.uv.y);
		}

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

	Vertex2D Mesh2D::operator[](const int index)
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
}