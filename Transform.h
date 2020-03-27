#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
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

			EntityID parent = 0;
		};

		struct Transform2D : ECSComponent<Transform2D>
		{
			Transform2D();
			glm::mat4 get();

			glm::vec2 position;
			glm::vec2 origin;
			glm::vec2 scale;
			float rotation = 0.0f;
		};
	}

	/**
	* Returns an entities world transformation.
	*/
	glm::mat4 getWorldT(Entities& entities, EntityID id);

	/**
	* Convert transformation matrix into a transform component.
	*/
	CS::Transform decompose(const glm::mat4& t);
}