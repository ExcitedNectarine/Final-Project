#pragma once

#include <array>
#include "Transform.h"
#include "Script.h"

#include "Output.h"
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

		struct Controller : ECSComponent<Controller>
		{
			glm::vec3 velocity;
			bool collided = false;
			bool on_floor = false;
		};
	}

	void moveControllers(Entities& entities, Core& core);

	EntityID castRay(Entities& entities, const glm::vec3& r_pos, const glm::vec3& r_dir, EntityID ignore);
	bool OBBcollision(CS::Transform& a, const glm::vec3& a_size, CS::Transform& b, const glm::vec3& b_size);
	bool AABBcollision(glm::vec3 a_pos, const glm::vec3& a_size, glm::vec3 b_pos, const glm::vec3& b_size);
	bool RayAABBcollision(glm::vec3 a_pos, const glm::vec3& a_size, glm::vec3 r_pos, const glm::vec3& r_dir, float& t);
}