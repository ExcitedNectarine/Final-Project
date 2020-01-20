#pragma once

#include "Transform.h"

namespace ENG
{
	namespace CS
	{
		struct AABB : ENG::ECSComponent<AABB>
		{
			AABB() : size(1.0f, 1.0f, 1.0f) {}

			glm::vec3 size;
			bool hit = false;
			ENG::EntityID hit_id;
			bool solid = true;
		};

		struct Collider : ENG::ECSComponent<Collider>
		{
		};
	}

	void AABBCollision(ENG::Entities& entities);
}