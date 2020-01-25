#pragma once

#include <array>
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
			};
		}

		struct SphereCollider : BaseCollider, ECSComponent<SphereCollider>
		{
			float radius = 1.0f;
		};

		struct BoxCollider : BaseCollider, ECSComponent<BoxCollider>
		{
			BoxCollider() : size(1.0f) {}

			glm::vec3 size;
		};

		struct Controller : ECSComponent<Controller>
		{
			glm::vec3 velocity;
		};
	}

	void moveControllers(Entities& entities);
	void testCollisions(Entities& entities, Application& app);
	bool OBBcollision(CS::Transform& a, const glm::vec3& a_size, CS::Transform& b, const glm::vec3& b_size);
}