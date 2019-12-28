#pragma once

#include "Window.h"
#include "Shader.h"
#include "Entities.h"
#include "Settings.h"

namespace ENG
{
	class Application
	{
	private:
		Window window;
		Entities entities;
		Shader def_shader;
		Settings settings;
	};
}