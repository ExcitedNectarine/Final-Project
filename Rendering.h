#pragma once

#include "Transform.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

namespace ENG
{
	namespace CS
	{
		struct Model : ENG::ECSComponent<Model>
		{
			ENG::Mesh* mesh;
			ENG::Texture* texture;
			ENG::Shader* shader;
		};
	}

	void drawModels(ENG::Entities& entities);
}