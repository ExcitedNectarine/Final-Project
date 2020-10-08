#pragma once

#include <limits>
#include <array>
#include "Transform.h"
#include "Script.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>

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

		struct PlaneCollider : ECSComponent<PlaneCollider>
		{
			PlaneCollider() : size(1.0f) {}

			glm::vec3 size;
			bool trigger = false;
		};

		struct Controller : ECSComponent<Controller>
		{
			glm::vec3 velocity;
			bool on_floor = false;
		};
	}

	struct IntersectData
	{
		IntersectData() : normal(0.0f) {}

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

	IntersectData seperatedAxisTest(const std::vector<glm::vec3>& a_verts, const std::vector<glm::vec3>& b_verts, const std::vector<glm::vec3>& axes);
	std::vector<glm::vec3> getBoxVerts(glm::vec3 size, glm::mat4 t);
	std::vector<glm::vec3> getFrustumVerts(glm::mat4 view, glm::mat4 perspective);
	void findMinMaxAlongAxis(glm::vec3 axis, const std::array<glm::vec3, 8>& verts, float& min, float& max);
	IntersectData intersectOBBvOBB(CS::Transform a_t, glm::vec3 a_size, CS::Transform b_t, glm::vec3 b_size);

	struct Camera
	{
		float aspect;
		float fov_y;
		float fov_x;
		float near;
		float far;

		Camera(glm::vec2 size, float fov, float near, float far) : near(near), far(far)
		{
			aspect = size.x / size.y;
			fov_y = fov;
			fov_x = static_cast<float>(glm::degrees(2 * glm::atan(glm::tan(fov_y * 0.5) * aspect)));
		}

		glm::mat4 get()
		{
			return glm::perspective(glm::radians(fov_y), aspect, near, far);
		}
	};

	IntersectData intersectOBBvFrustum(CS::Transform a_t, glm::vec3 a_size, CS::Transform view_t, Camera c);
}