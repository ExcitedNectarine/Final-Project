#pragma once

#include "Window.h"
#include "Clock.h"
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
		Clock clock;
		Entities entities;
		Resources resources;
		Settings settings;
		Renderer renderer;
	};
}