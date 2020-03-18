#pragma once

#include "Transform.h"
#include "Shader.h"

namespace ENG
{
	/**
	* Component namespace. All components that can make up an entity are declared here.
	*/
	namespace CS
	{
		struct Light : ECSComponent<Light>
		{
			Light() : colour(1.0f) {}

			glm::vec3 colour;
			float radius = 5.0f;
		};
	}

	void setLights(Entities& entities, Shader& shader);
}