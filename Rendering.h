#pragma once

#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Collision.h"
#include "Resources.h"
#include "FrameBuffer.h"

namespace ENG
{
	struct Core;
	struct Renderer
	{
		EntityID view_id = 0;
		CS::Transform* view;
		glm::vec3 ambient;
		bool draw_colliders = false;
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
			std::string shader = "default.shdr";
			bool hud = false;
		};

		/**
		* Sprite component can be used for 2D or 3D sprites. Also supports simple
		* sprite animation.
		*/
		struct Sprite : ECSComponent<Sprite>
		{
			Sprite() : frame(1), frames(1) {}

			std::string texture = "notexture.png";
			bool billboard = true;

			bool animated = false;
			glm::ivec2 frame;
			glm::ivec2 frames;
			float frame_time = 0.0f;
			float timer = 0.0f;
		};

		/**
		* Light component is used as a point light, which has a radius which it illuminates and a colour.
		*/
		struct Light : ECSComponent<Light>
		{
			Light() : colour(1.0f) {}

			glm::vec3 colour;
			float radius = 5.0f;
		};

		/**
		* Camera component represents a 3D camera in the world. Draws to a framebuffer from its perspective.
		*/
		struct Camera : ECSComponent<Camera>
		{
			Camera() {}
			Camera(const glm::vec2& size, const float fov, const float near, const float far);
			glm::mat4 get();

			FrameBuffer frame;
			glm::ivec2 size;
			float aspect;
			float fov_y;
			float fov_x;
			float near;
			float far;
		};

		struct Text : ECSComponent<Text>
		{
			void setText(const std::string& new_text);

			std::string text = "This is a sentence";
			Mesh2D mesh;
		};
	}

	namespace
	{
		Mesh2D quad_2d;
		Mesh quad_3d;
	}

	void drawToCameras(Core& core);
	void drawToScreen(Core& core);

	void updateRenderer(Core& core);
	void drawModels(Core& core);
	void drawModelsToHUD(Core& core);
	void drawSkybox(Resources& resources);
	void spriteStart(Core& core);
	void updateSprites(Core& core);
	void drawSprites(Core& core);
	void drawSprites3D(Core& core);
	void drawColliders(Core& core);
	void renderText(Core& core);

	/**
	* Uploads lighting information to shader.
	*/
	void setLights(Core& core, Shader& shader);
}