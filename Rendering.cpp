#include "Rendering.h"
#include <glm/gtx/string_cast.hpp>

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

			//float angle = glm::acos(glm::dot(transforms[id].rotation, transforms[portals[id].other].rotation));
			//glm::quat diff = glm::angleAxis(glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
			//glm::vec3 new_direction = diff * transforms[portals[id].player].forward();

			//portals[id].camera.rotation = glm::quat(glm::radians(new_direction));
			//portals[id].camera.rotation = glm::quatLookAt(new_direction, glm::vec3(0.0f, 1.0f, 0.0f));
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
			glm::mat4 view = glm::inverse(portals[portals[id].other].camera.get());

			resources.shader("default.shader").setUniform("view", view);
			resources.shader("default.shader").setUniform("view_pos", portals[portals[id].other].camera.position);
			resources.shader("skybox.shader").setUniform("view", glm::mat4(glm::mat3(view)));

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

	glm::vec2 worldToView(glm::vec3 p, glm::mat4 transform, glm::mat4 view, glm::mat4 projection)
	{
		glm::vec4 clip = projection * (view * (transform * glm::vec4(p, 1.0f)));
		return glm::clamp(glm::vec2(clip / clip.w) / 2.0f + 0.5f, glm::vec2(0.0f), glm::vec2(1.0f));
	}

	void drawPortals(Entities& entities, Resources& resources, glm::mat4 perspective, glm::mat4 view)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		Mesh& quad = resources.mesh("quad.obj");
		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			glm::mat4 transform = transforms[id].get();

			for (int i = 0; i < 6; i++)
				quad[i].uv = worldToView(quad[i].position, transform, view, perspective);

			resources.shader("default.shader").setUniform("transform", transform);
			resources.shader("default.shader").bind();
			quad.bind();
			portals[id].framebuffer.getTexture().bind();

			glDrawArrays(GL_TRIANGLES, 0, quad.vertexCount());
		}
	}
}