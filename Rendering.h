#pragma once

#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Transform.h"
#include "Resources.h"
#include "FrameBuffer.h"

namespace ENG
{
	struct Core;

	namespace CS
	{
		struct Model : ECSComponent<Model>
		{
			std::string mesh = "cube.obj";
			std::string texture = "notexture.png";

			bool shaded = true;
			bool transparent = false;
			bool hud = false;
			bool billboard = false;
		};

		struct Light : ECSComponent<Light>
		{
			Light() : colour(1.0f) {}

			glm::vec3 colour;
			float radius = 5.0f;
		};
	}

	void drawModels(Core& core);
	void drawModelsToHUD(Core& core);
	void drawSkybox(Resources& resources);

	/**
	* Uploads lighting information to shader.
	*/
	void setLights(Entities& entities, Shader& shader);
}