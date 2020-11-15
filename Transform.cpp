#include "Transform.h"

namespace ENG
{
	namespace CS
	{
		Transform::Transform()
			: position(0.0f),
			rotation(0.0f),
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
			return glm::vec3(transform[2]);
		}

		glm::vec3 Transform::up()
		{
			glm::mat4 transform = get();
			return glm::vec3(transform[1]);
		}

		glm::vec3 Transform::right()
		{
			glm::mat4 transform = get();
			return glm::vec3(transform[0]);
		}

		Transform2D::Transform2D()
			: position(0.0f),
			origin(0.0f),
			scale(1.0f) {}

		glm::mat4 Transform2D::get()
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position - origin, 0.0f));

			transform = glm::translate(transform, glm::vec3(-origin, 0.0f));
			transform = glm::rotate(transform, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
			transform = glm::translate(transform, glm::vec3(origin, 0.0f));

			return glm::scale(transform, glm::vec3(scale, 1.0f));
		}
	}

	/**
	* Returns an entity's world transformation.
	*/
	glm::mat4 getWorldM(Entities& entities, EntityID id)
	{
		static auto& pool = entities.getPool<CS::Transform>();

		if (!entities.hasComponent<CS::Transform>(id))
			return glm::mat4(1.0f);

		if (pool[id].parent == 0) // Root transform
			return pool[id].get();

		return getWorldM(entities, pool[id].parent) * pool[id].get();
	}

	/**
	* Returns an entity's decomposed world transformation.
	*/
	CS::Transform getWorldT(Entities& entities, EntityID id)
	{
		return decompose(getWorldM(entities, id));
	}

	/**
	* Convert transformation matrix into a transform component.
	*/
	CS::Transform decompose(const glm::mat4& t)
	{
		CS::Transform d;

		d.position = t[3];
		
		glm::extractEulerAngleYXZ(t, d.rotation.y, d.rotation.x, d.rotation.z);
		d.rotation = glm::degrees(d.rotation);

		d.scale.x = glm::length(t[0]);
		d.scale.y = glm::length(t[1]);
		d.scale.z = glm::length(t[2]);

		return d;
	}
}