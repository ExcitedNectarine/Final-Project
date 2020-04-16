#pragma once

#include <limits>
#include <array>
#include "Transform.h"
#include "Script.h"

#include <glm/gtx/string_cast.hpp>

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

		struct PlaneCollider :ECSComponent<PlaneCollider>
		{
			PlaneCollider() : size(1.0f) {}

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

	struct IntersectData
	{
		bool intersects = false;
		float distance = 0.0f;
		glm::vec3 normal;
	};

	/**
	* Move controller components through the world, blocking the movement if they encounter a solid
	* collider.
	*/
	void moveControllers(Core& core);

	/**
	* Cast a ray into the world, and return the closest intersecting box ID.
	*/
	EntityID castRay(Entities& entities, const glm::vec3& r_pos, const glm::vec3& r_dir, EntityID ignore, float& t);

	///

	/**
	* Checks if two axis-aligned (not rotated) bounding boxes are colliding
	*/
	IntersectData intersectAABBvAABB(const glm::vec3& a_pos, const glm::vec3& a_size, const glm::vec3& b_pos, const glm::vec3& b_size);

	/**
	* Checks if a ray intersects an AABB.
	*/
	IntersectData intersectAABBvRay(glm::vec3 a_pos, const glm::vec3& a_size, glm::vec3 r_pos, const glm::vec3& r_dir);

	/**
	* Checks if an AABB intersects a plane.
	*/
	IntersectData intersectAABBvPlane(glm::vec3 b_pos, glm::vec3 b_size, glm::vec3 p_pos, glm::vec3 p_norm, glm::vec3 p_size);

	/**
	* Checks if a ray intersects a plane.
	*/
	IntersectData intersectPlaneVRay(glm::vec3 p_pos, glm::vec3 p_norm, glm::vec3 r_pos, glm::vec3 r_dir);
}