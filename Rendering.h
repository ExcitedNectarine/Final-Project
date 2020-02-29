#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Transform.h"
#include "Resources.h"

namespace ENG
{
	namespace CS
	{
		struct Model : ECSComponent<Model>
		{
			std::string mesh, texture, shader;
		};
	}

	void drawModels(Entities& entities, Resources& resources);
}