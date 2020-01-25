#pragma once

#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

namespace ENG
{
	namespace CS
	{
		struct Model : ECSComponent<Model>
		{
			Mesh* mesh;
			Texture* texture;
			Shader* shader;
		};
	}

	void drawModels(Entities& entities);
}