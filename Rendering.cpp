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

			if (m.shaded)
			{
				resources.shader("default.shdr").setUniform("transform", getWorldT(entities, id));
				resources.shader("default.shdr").bind();
			}
			else
			{
				resources.shader("unshaded.shdr").setUniform("transform", getWorldT(entities, id));
				resources.shader("unshaded.shdr").bind();
			}

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

			if (m.shaded)
			{
				resources.shader("default.shdr").setUniform("transform", getWorldT(entities, id));
				resources.shader("default.shdr").bind();
			}
			else
			{
				resources.shader("unshaded.shdr").setUniform("transform", getWorldT(entities, id));
				resources.shader("unshaded.shdr").bind();
			}

			resources.mesh(m.mesh).bind();
			resources.texture(m.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, resources.mesh(m.mesh).vertexCount());
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

	/**
	* Uploads lighting information to shader.
	*/
	void setLights(Entities& entities, Shader& shader)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& lights = entities.getPool<CS::Light>();

		std::vector<EntityID> ents = entities.entitiesWith<CS::Transform, CS::Light>();
		for (std::size_t i = 0; i < ents.size(); i++)
		{
			shader.setUniform("lights[" + std::to_string(i) + "].position", decompose(getWorldT(entities, ents[i])).position);
			shader.setUniform("lights[" + std::to_string(i) + "].colour", lights[ents[i]].colour);
			shader.setUniform("lights[" + std::to_string(i) + "].radius", lights[ents[i]].radius);
		}
	}
}