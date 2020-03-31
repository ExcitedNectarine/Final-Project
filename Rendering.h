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
		glm::vec3 ambient;
	};

	namespace CS
	{
		/**
		* Model component used for drawing a mesh with a texture to the screen.
		*/
		struct Model : ECSComponent<Model>
		{
			std::string mesh = "cube.obj";
			std::string texture = "notexture.png";

			bool shaded = true;
			bool hud = false;
		};

		/**
		* Sprite component can be used for 2D or 3D sprites. Also supports simple
		* sprite animation.
		*/
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

		struct Light : ECSComponent<Light>
		{
			Light() : colour(1.0f) {}

			glm::vec3 colour;
			float radius = 5.0f;
		};
	}

	namespace
	{
		Mesh2D quad_2d;
		Mesh quad_3d;
	}

	void updateRenderer(Core& core);

	void drawModels(Core& core);
	void drawModelsToHUD(Core& core);
	void drawSkybox(Resources& resources);

	void spriteStart();
	void updateSprites(Core& core);
	void drawSprites(Core& core);
	void drawSprites3D(Core& core);

	/**
	* Uploads lighting information to shader.
	*/
	void setLights(Entities& entities, Shader& shader);
}