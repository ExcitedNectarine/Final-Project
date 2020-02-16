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

	void updatePortals(Entities& entities)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();
		glm::vec3 player_offset;

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			player_offset = transforms[portals[id].player].position - transforms[portals[id].other].position;
			portals[id].camera.position = transforms[id].position + player_offset;
			portals[id].camera.rotation = transforms[portals[id].player].rotation;
		}
	}

	void drawToPortals(Entities& entities, Resources& resources)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			portals[id].framebuffer.bind();

			resources.shader("default.shader").setUniform("view", glm::inverse(portals[portals[id].other].camera.get()));
			resources.shader("default.shader").setUniform("view_pos", portals[portals[id].other].camera.position);
			resources.shader("skybox.shader").setUniform("view", glm::mat4(glm::mat3(glm::inverse(portals[portals[id].other].camera.get()))));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDepthMask(GL_FALSE);
			resources.shader("skybox.shader").bind();
			resources.mesh("cube.obj").bind();
			glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube.obj").vertexCount());
			glDepthMask(GL_TRUE);

			drawModels(entities, resources);

			portals[id].framebuffer.unbind();
		}
	}

	void drawPortals(Entities& entities, Resources& resources)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			resources.shader("default.shader").setUniform("transform", transforms[id].get());
			resources.shader("default.shader").bind();
			resources.mesh("quad.obj").bind();
			portals[id].framebuffer.getTexture().bind();

			glDrawArrays(GL_TRIANGLES, 0, resources.mesh("quad.obj").vertexCount());
		}
	}
}