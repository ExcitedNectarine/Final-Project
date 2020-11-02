#pragma once

#include <limits>
#include <array>
#include "Transform.h"
#include "Script.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace ENG
{
	namespace CS
	{
		struct BoxCollider : ECSComponent<BoxCollider>
		{
			BoxCollider() : size(1.0f) {}

			glm::vec3 size;
			bool trigger = false;

			bool _collided = false;
		};

		struct Controller : ECSComponent<Controller>
		{
			glm::vec3 velocity;
			bool on_floor = false;
		};

		struct Camera;
	}

	struct IntersectData
	{
		IntersectData() : normal(0.0f) {}

		bool intersects = false;
		float distance = 0.0f;
		glm::vec3 normal;
		EntityID id = 0;
	};

	/**
	* Move controller components through the world, blocking the movement if they encounter a solid
	* collider.
	*/
	void moveControllers(Core& core);

	/**
	* This function returns a vector containing every box collider in the game.
	*/
	std::vector<IntersectData> getIntersectingEntities(Entities& entities, EntityID entity);

	/**
	* Cast a ray into the world, and return the closest intersecting box ID with the intersecion data.
	*/
	IntersectData castRay(Entities& entities, const glm::vec3& r_pos, const glm::vec3& r_dir, const std::vector<EntityID>& ignore);

	/**
	* Checks if two axis-aligned (not rotated) bounding boxes are colliding
	*/
	IntersectData intersectAABBvAABB(const glm::vec3& a_pos, const glm::vec3& a_size, const glm::vec3& b_pos, const glm::vec3& b_size);

	/**
	* Checks if a ray intersects an AABB.
	*/
	IntersectData intersectAABBvRay(const glm::vec3 a_pos, const glm::vec3& a_size, glm::vec3 r_pos, const glm::vec3& r_dir);

	/**
	* Checks if a ray intersects an OBB
	*/
	IntersectData intersectOBBvRay(CS::Transform a_t, glm::vec3 a_size, glm::vec3 r_pos, glm::vec3 r_dir);

	/**
	* Checks if an OBB intersects another OBB, using SAT.
	*/
	IntersectData intersectOBBvOBB(CS::Transform a_t, glm::vec3 a_size, CS::Transform b_t, glm::vec3 b_size);

	/**
	* Checks if an OBB intersects a frustum, using SAT.
	*/
	IntersectData intersectOBBvFrustum(CS::Transform a_t, glm::vec3 a_size, CS::Transform view_t, CS::Camera c);

	std::vector<glm::vec3> getBoxVerts(glm::vec3 size, glm::mat4 t);
	std::vector<glm::vec3> getFrustumVerts(glm::mat4 view, glm::mat4 perspective);
	void findMinMaxAlongAxis(glm::vec3 axis, const std::vector<glm::vec3>& verts, float& min, float& max);
	IntersectData seperatedAxisTest(const std::vector<glm::vec3>& a_verts, const std::vector<glm::vec3>& b_verts, const std::vector<glm::vec3>& axes);
}