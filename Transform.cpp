#include "Transform.h"

namespace ENG
{
	namespace CS
	{
		Transform::Transform()
			: position(0.0f),
			rotation(glm::vec3(0.0f)),
			scale(1.0f) {}

		glm::mat4 Transform::get()
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);

			transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			return glm::scale(transform, scale);
		}

		glm::vec3 Transform::forward()
		{
			glm::mat4 transform = get();
			return { transform[2].x, transform[2].y, transform[2].z };
		}

		glm::vec3 Transform::right()
		{
			glm::mat4 transform = get();
			return { transform[0].x, transform[0].y, transform[0].z };
		}
	}

	glm::mat4 getWorld(Entities& entities, EntityID id)
	{
		static auto& pool = entities.getPool<CS::Transform>();

		if (pool[id].parent == 0) // Root transform
			return pool[id].get();

		return getWorld(entities, pool[id].parent) * pool[id].get();
	}
}