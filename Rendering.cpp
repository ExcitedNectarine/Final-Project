#include "Rendering.h"

namespace ENG
{

	/**
	* Draws model components.
	*/
	void drawModels(Entities& entities)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& models = entities.getPool<CS::Model>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = models[id];

			m.shader->setUniform("transform", transforms[id].get());
			m.shader->bind();
			m.mesh->bind();
			m.texture->bind();
			glDrawArrays(GL_TRIANGLES, 0, m.mesh->vertexCount());
		}
	}

	void drawSprites(Entities& entities, CS::Transform& view)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& sprites = entities.getPool<CS::Sprite>();

		CS::Transform scalar;
		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Sprite>())
		{
			CS::Sprite& sprite = sprites[id];

			scalar.scale = glm::vec3(sprite.texture->getSize(), 1.0f);

			if (sprite.billboard)
				transforms[id].rotation.y = view.rotation.y;

			sprite.shader->setUniform("transform", transforms[id].get() * scalar.get());
			sprite.texture->bind();
			//glDrawArrays(GL_TRIANGLES, 0, quad.vertexCount());
		}
	}

	void drawFrameBuffers(Entities& entities, Resources& resources)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& framebuffers = entities.getPool<CS::FrameBuffer>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::FrameBuffer>())
		{
			framebuffers[id].bind();
			resources.shader("default.shader").setUniform("view", transforms[id].get());
			resources.shader("default.shader").setUniform("view_pos", transforms[id].position);
			resources.shader("skybox.shader").setUniform("view", glm::mat4(glm::mat3(transforms[id].get())));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDepthMask(GL_FALSE);
			resources.shader("skybox.shader").bind();
			resources.mesh("cube.obj").bind();
			glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube.obj").vertexCount());
			glDepthMask(GL_TRUE);

			drawModels(entities);

			framebuffers[id].unbind();
		}
	}
}