#include "Mesh.h"

namespace ENG
{
	void Mesh::setVertices(const std::vector<Vertex>& new_vertices)
	{
		vertices = new_vertices;

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

		create(positions, uvs, normals);
	}

	void Mesh::create(const std::vector<GLfloat>& positions, const std::vector<GLfloat>& uvs, const std::vector<GLfloat>& normals)
	{
		// Create position buffer.
		GLuint position_buffer;
		glGenBuffers(1, &position_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), &positions.at(0), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);

		// Create UV buffer.
		GLuint uv_buffer;
		glGenBuffers(1, &uv_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GLfloat), &uvs.at(0), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);

		// Create normal buffer.
		GLuint normal_buffer;
		glGenBuffers(1, &normal_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals.at(0), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);

		// Create vertex array.
		glGenVertexArrays(1, &id);
		glBindVertexArray(id);

		// Bind position buffer to vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);

		// Bind UV buffer to vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);

		// Bind normal buffer to vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, NULL);

		glBindVertexArray(NULL);
	}

	std::size_t Mesh::vertexCount() { return vertices.size(); }
	void Mesh::bind() { glBindVertexArray(id); }
	void Mesh::unbind() { glBindVertexArray(NULL); }
	void Mesh::cleanup() { glDeleteVertexArrays(1, &id); }
}