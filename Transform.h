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
		* Transform component, represents a transform in 3D space. Has a position,
		* rotation and scale, all in local space.
		*/
		struct Transform : ECSComponent<Transform>
		{
			Transform();
			
			/**
			* Returns the 4x4 transformation matrix.
			*/
			glm::mat4 get();

			/**
			* Returns the forward vector (Z axis).
			*/
			glm::vec3 forward();

			/**
			* Returns the up vector (Y axis).
			*/
			glm::vec3 up();

			/**
			* Returns the right vector (X axis).
			*/
			glm::vec3 right();

			glm::vec3 position;
			glm::vec3 rotation;
			glm::vec3 scale;

			EntityID parent = 0;
		};

		/**
		* 2D Transform component, represents a transform in 2D space. Has a position,
		* origin, scale and rotation, all in local space.
		*/
		struct Transform2D : ECSComponent<Transform2D>
		{
			Transform2D();

			/**
			* Returns the 4x4 transformation matrix.
			*/
			glm::mat4 get();

			glm::vec2 position;
			glm::vec2 origin;
			glm::vec2 scale;
			float rotation = 0.0f;
		};
	}

	/**
	* Returns an entity's world transformation.
	*/
	glm::mat4 getWorldM(Entities& entities, EntityID id);

	/**
	* Returns an entity's decomposed world transformation.
	*/
	CS::Transform getWorldT(Entities& entities, EntityID id);

	/**
	* Converts a 4x4 transformation matrix into a transform component.
	*/
	CS::Transform decompose(const glm::mat4& t);
}