#include "Rendering.h"

namespace ENG
{
	namespace CS
	{
		glm::mat4 Camera::get() { return glm::perspective(glm::radians(fov), aspect, near, far); }
	}

	/**
	* Draws model components.
	*/
	void drawModels(Entities& entities, Resources& resources, const glm::vec3& view_pos)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& models = entities.getPool<CS::Model>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = models[id];

			if (m.hud) continue;

			resources.shader("default.shdr").setUniform("transform", getWorldT(entities, id));
			resources.shader("default.shdr").bind();
			resources.mesh(m.mesh).bind();
			resources.texture(m.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, resources.mesh(m.mesh).vertexCount());
		}
	}

	void drawModelsToHUD(Entities& entities, Resources& resources, const glm::vec3& view_pos)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& models = entities.getPool<CS::Model>();

		glClear(GL_DEPTH_BUFFER_BIT);

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = models[id];

			if (!m.hud) continue;

			resources.shader("default.shdr").setUniform("transform", getWorldT(entities, id));
			resources.shader("default.shdr").bind();
			resources.mesh(m.mesh).bind();
			resources.texture(m.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, resources.mesh(m.mesh).vertexCount());
		}
	}

	void drawToCameras(Entities& entities, Resources& resources)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& cameras = entities.getPool<CS::Camera>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Camera>())
		{
			resources.shader("default.shdr").setUniform("projection", cameras[id].get());
			resources.shader("default.shdr").setUniform("view", glm::inverse(getWorldT(entities, id)));
			resources.shader("default.shdr").setUniform("view_pos", transforms[id].position);
			resources.shader("skybox.shdr").setUniform("view", glm::mat4(glm::mat3(glm::inverse(getWorldT(entities, id)))));

			cameras[id].frame.bind();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawSkybox(resources);
			drawModels(entities, resources, transforms[id].position);

			cameras[id].frame.unbind();
		}
	}

	void drawSkybox(Resources& resources)
	{
		glDepthMask(GL_FALSE);
		resources.shader("skybox.shdr").bind();
		resources.mesh("cube_inverted.obj").bind();
		glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube_inverted.obj").vertexCount());
		glDepthMask(GL_TRUE);
	}
}