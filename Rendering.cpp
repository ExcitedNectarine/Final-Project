#include "Rendering.h"

namespace ENG
{
	/**
	* Draws model components.
	*/
	void drawModels(Entities& entities, Resources& resources)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& models = entities.getPool<CS::Model>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = models[id];

			resources.shader(m.shader).setUniform("transform", transforms[id].get());
			resources.shader(m.shader).bind();
			resources.mesh(m.mesh).bind();
			resources.texture(m.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, resources.mesh(m.mesh).vertexCount());
		}
	}

	void drawScreens(Entities& entities, Resources& resources)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& screens = entities.getPool<CS::Screen>();
		auto& framebuffers = entities.getPool<CS::FrameBuffer>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Screen>())
		{
			CS::Screen& s = screens[id];

			resources.shader(s.shader).setUniform("transform", transforms[id].get());
			resources.shader(s.shader).bind();
			resources.mesh(s.mesh).bind();
			framebuffers[s.framebuffer_id].getTexture().bind();

			glDrawArrays(GL_TRIANGLES, 0, resources.mesh(s.mesh).vertexCount());
		}
	}

	void drawToFrameBuffers(Entities& entities, Resources& resources)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& framebuffers = entities.getPool<CS::FrameBuffer>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::FrameBuffer>())
		{
			framebuffers[id].bind();
			resources.shader("default.shader").setUniform("view", glm::inverse(transforms[id].get()));
			resources.shader("default.shader").setUniform("view_pos", transforms[id].position);
			resources.shader("skybox.shader").setUniform("view", glm::mat4(glm::mat3(glm::inverse(transforms[id].get()))));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDepthMask(GL_FALSE);
			resources.shader("skybox.shader").bind();
			resources.mesh("cube.obj").bind();
			glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube.obj").vertexCount());
			glDepthMask(GL_TRUE);

			drawModels(entities, resources);

			framebuffers[id].unbind();
		}
	}
}