#pragma once

#include <glm/glm.hpp>

namespace ENG
{
	struct Vertex
	{
		Vertex()
			: position(0.0f),
			uv(0.0f),
			normal(0.0f) {}

		Vertex(const glm::vec3& position, const glm::vec2& uv, const glm::vec3& normal)
			: position(position),
			uv(uv),
			normal(normal) {}

		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	struct Vertex2D
	{
		Vertex2D()
			: position(0.0f),
			uv(0.0f) {}

		Vertex2D(const glm::vec2& position, const glm::vec2& uv)
			: position(position),
			uv(uv) {}

		glm::vec2 position;
		glm::vec2 uv;
	};
}