#pragma once

#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Transform.h"
#include "Resources.h"
#include "FrameBuffer.h"

namespace ENG
{
	struct Core;
	
	struct Renderer
	{
		Mesh quad_3d;
		Mesh2D quad_2d;
	};

	namespace CS
	{
		struct Model : ECSComponent<Model>
		{
			std::string mesh = "cube.obj";
			std::string texture = "notexture.png";

			bool shaded = true;
			bool transparent = false;
			bool hud = false;
		};

		struct Light : ECSComponent<Light>
		{
			Light() : colour(1.0f) {}

			glm::vec3 colour;
			float radius = 5.0f;
		};

		struct Sprite : ECSComponent<Sprite>
		{
			Sprite();

			std::string texture = "notexture.png";
			bool billboard = true;
			bool shaded = true;

			bool animated = false;
			glm::ivec2 frame;
			glm::ivec2 frames;
			float frame_time = 0.0f;
			float timer = 0.0f;
		};
	}

	void updateSprites(Core& core);
	void drawModels(Core& core);
	void drawModelsToHUD(Core& core);
	void drawSkybox(Resources& resources);

	void spriteStart();
	void drawSprites(Core& core);
	void drawSprites3D(Core& core);

	/**
	* Uploads lighting information to shader.
	*/
	void setLights(Entities& entities, Shader& shader);
}