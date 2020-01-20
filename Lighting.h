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
		struct Light : ENG::ECSComponent<Light>
		{
			Light() : colour(1.0f, 1.0f, 1.0f), radius(5.0f) {}

			glm::vec3 colour;
			float radius;
		};
	}

	void setLights(ENG::Entities& entities, ENG::Shader& shader);
}