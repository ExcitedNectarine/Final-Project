#pragma once

#include "Window.h"
#include "Entities.h"
#include "Resources.h"
#include "Settings.h"
#include "Transform.h"
#include "Rendering.h"

namespace ENG
{
	/**
	* Main core struct, all game objects and entities are used in this class.
	*/
	struct Core
	{
		Window window;
		Entities entities;
		Resources resources;
		Settings settings;
		Renderer renderer;
		CS::Camera camera;
		glm::mat4 perspective;
		glm::mat4 orthographic;

		float delta;
	};
}