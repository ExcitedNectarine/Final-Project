#pragma once

#include <array>
#include "Transform.h"
#include "Script.h"

namespace ENG
{
	namespace CS
	{
		struct BoxCollider : ECSComponent<BoxCollider>
		{
			BoxCollider() : size(1.0f) {}

			glm::vec3 size;
			bool solid = true;
		};

		struct Controller : ECSComponent<Controller>
		{
			glm::vec3 velocity;
			bool collided = false;
			bool on_floor = false;
		};
	}

	/**
	* Move controller components through the world, blocking the movement if they encounter a solid
	* collider.
	*/
	void moveControllers(Entities& entities, Core& core);

	/**
	* Cast a ray into the world, and return the closest intersecting box ID.
	*/
	EntityID castRay(Entities& entities, const glm::vec3& r_pos, const glm::vec3& r_dir, EntityID ignore, float& t);

	/**
	* Checks if two axis-aligned (not rotated) bounding boxes are colliding
	*/
	bool intersectAABBvAABB(glm::vec3 a_pos, const glm::vec3& a_size, glm::vec3 b_pos, const glm::vec3& b_size);

	/**
	* Checks if a ray intersects an AABB.
	*/
	bool intersectAABBvRay(glm::vec3 a_pos, const glm::vec3& a_size, glm::vec3 r_pos, const glm::vec3& r_dir, float& t);
}