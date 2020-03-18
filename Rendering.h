#pragma once

#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Transform.h"
#include "Resources.h"
#include "FrameBuffer.h"

namespace ENG
{
	namespace CS
	{
		struct Model : ECSComponent<Model>
		{
			std::string mesh = "cube.obj";
			std::string texture = "notexture.png";

			bool hud = false;
		};

		struct Camera : ECSComponent<Camera>
		{
			FrameBuffer frame;

			float aspect;
			float fov = 90.0f;
			float near = 0.1f;
			float far = 500.0f;

			glm::mat4 get();
		};
	}

	void drawModels(Entities& entities, Resources& resources, const glm::vec3& view_pos);
	void drawModelsToHUD(Entities& entities, Resources& resources, const glm::vec3& view_pos);
	void drawToCameras(Entities& entities, Resources& resources);
	void drawSkybox(Resources& resources);
}