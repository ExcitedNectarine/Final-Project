#pragma once

#include "Transform.h"
#include "Script.h"

namespace ENG
{
	namespace CS
	{
		namespace
		{
			struct BaseCollider
			{
				bool solid = true;
				glm::vec3 prev_position;
			};
		}

		struct SphereCollider : BaseCollider, ENG::ECSComponent<SphereCollider>
		{
			float radius = 1.0f;
		};

		struct BoxCollider : BaseCollider, ENG::ECSComponent<BoxCollider>
		{
			BoxCollider() : size(1.0f, 1.0f, 1.0f) {}

			glm::vec3 size;
		};
	}

	void testCollisions(ENG::Entities& entities, ENG::Application& app);
	bool AABBcollision(const glm::vec3& a_pos, const glm::vec3& a_size, const glm::vec3& b_pos, const glm::vec3& b_size);
}