#pragma once

#include "Entities.h"
#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Script.h"

/**
* Component namespace. All components that can make up an entity are declared here.
*/
namespace CS
{
	struct Transform : ENG::Transform, ENG::ECSComponent<Transform> {};

	struct Model : ENG::ECSComponent<Model>
	{
		ENG::Mesh* mesh;
		ENG::Texture* texture;
		ENG::Shader* shader;
	};

	struct Light : ENG::ECSComponent<Light>
	{
		Light() : colour(1.0f, 1.0f, 1.0f), radius(5.0f) {}

		glm::vec3 colour;
		float radius;
	};

	struct AABB : ENG::ECSComponent<AABB>
	{
		AABB() : size(1.0f, 1.0f, 1.0f) {}

		glm::vec3 size;
		bool hit = false;
		ENG::EntityID hit_id;
	};

	/**
	* Script component, used for adding scripts to entities.
	*/
	struct Script : ENG::ECSComponent<Script>
	{
		// Because the entity manager stores components by value, we have to use one level of
		// indirection and store a pointer to the script.
		std::unique_ptr<ENG::Script> script;
	};
}

void drawModels(ENG::Entities& entities);
void setLights(ENG::Entities& entities, ENG::Shader& shader);
void AABBCollision(ENG::Entities& entities);