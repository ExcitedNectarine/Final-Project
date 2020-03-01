#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Entities.h"

namespace ENG
{
	namespace CS
	{
		/**
		* Transform component, represents a transform in the world.
		*/
		struct Transform : ECSComponent<Transform>
		{
			Transform();
			
			glm::mat4 get();
			glm::vec3 forward();
			glm::vec3 right();

			glm::vec3 position;
			glm::vec3 rotation;
			glm::vec3 scale;
		};
	}
}