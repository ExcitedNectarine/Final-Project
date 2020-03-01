#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Entities.h"

namespace ENG
{
	namespace CS
	{
		struct Camera : ECSComponent<Camera>
		{
			float aspect;
			float fov;
			float near;
			float far;

			glm::mat4 get();
		};
	}
}