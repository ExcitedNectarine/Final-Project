#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Transform.h"
#include "Resources.h"
#include "FrameBuffer.h"

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

		struct Sprite : ECSComponent<Sprite>
		{
			Texture* texture;
			Shader* shader;
			bool billboard = false;
			bool animated = false;
		};

		struct FrameBuffer : ECSComponent<FrameBuffer>, ENG::FrameBuffer {};
	}

	void drawModels(Entities& entities);
	void drawSprites(Entities& entities, CS::Transform& view);
	void drawFrameBuffers(Entities& entities, Resources& resources);
}