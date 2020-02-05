#pragma once

#include "Window.h"
#include "Resources.h"
#include "Settings.h"
#include "CubeMap.h"
#include "Components.h"

namespace ENG
{
	/**
	* Main core struct, all game objects and entities are used in this class.
	*/
	struct Core
	{
		Core(const std::string& setting_file);

		/**
		* Run main game loop.
		*/
		void run();

		Window window;
		Entities entities;
		Resources resources;
		Settings settings;
		CubeMap skybox;

		CS::Transform view;
		glm::mat4 perspective;
		glm::mat4 orthographic;

		float delta;
	};
}