#pragma once

#include "Window.h"
#include "Shader.h"
#include "Entities.h"

namespace ENG
{
	class Application
	{
	private:
		Window window;
		Entities entities;
		Shader def_shader;
	};
}