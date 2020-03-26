#include "Rendering.h"
#include "Core.h"

namespace ENG
{
	/**
	* Draws model components.
	*/
	void drawModels(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Model>& models = core.entities.getPool<CS::Model>();

		std::map<float, EntityID> distances;
		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Model>())
			distances[glm::distance(core.view->position, transforms[id].position)] = id;

		for (auto& p : distances)
		{
			CS::Model& m = models[p.second];

			if (m.hud || m.transparent) continue;
			if (m.billboard) transforms[p.second].rotation = core.view->rotation;
			if (m.shaded)
			{
				core.resources.shader("default.shdr").setUniform("transform", getWorldT(core.entities, p.second));
				core.resources.shader("default.shdr").bind();
			}
			else
			{
				core.resources.shader("unshaded.shdr").setUniform("transform", getWorldT(core.entities, p.second));
				core.resources.shader("unshaded.shdr").bind();
			}

			core.resources.mesh(m.mesh).bind();
			core.resources.texture(m.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, core.resources.mesh(m.mesh).vertexCount());
		}

		for (std::map<float, EntityID>::reverse_iterator it = distances.rbegin(); it != distances.rend(); ++it)
		{
			CS::Model& m = models[it->second];

			if (m.hud || !m.transparent) continue;
			if (m.billboard) transforms[it->second].rotation = core.view->rotation;
			if (m.shaded)
			{
				core.resources.shader("default.shdr").setUniform("transform", getWorldT(core.entities, it->second));
				core.resources.shader("default.shdr").bind();
			}
			else
			{
				core.resources.shader("unshaded.shdr").setUniform("transform", getWorldT(core.entities, it->second));
				core.resources.shader("unshaded.shdr").bind();
			}

			core.resources.mesh(m.mesh).bind();
			core.resources.texture(m.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, core.resources.mesh(m.mesh).vertexCount());
		}
	}

	void drawModelsToHUD(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Model>& models = core.entities.getPool<CS::Model>();

		glClear(GL_DEPTH_BUFFER_BIT);

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = models[id];

			if (!m.hud) continue;
			if (m.shaded)
			{
				core.resources.shader("default.shdr").setUniform("transform", getWorldT(core.entities, id));
				core.resources.shader("default.shdr").bind();
			}
			else
			{
				core.resources.shader("unshaded.shdr").setUniform("transform", getWorldT(core.entities, id));
				core.resources.shader("unshaded.shdr").bind();
			}

			core.resources.mesh(m.mesh).bind();
			core.resources.texture(m.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, core.resources.mesh(m.mesh).vertexCount());
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
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<CS::Light>& lights = entities.getPool<CS::Light>();

		std::vector<EntityID> ents = entities.entitiesWith<CS::Transform, CS::Light>();
		for (std::size_t i = 0; i < ents.size(); i++)
		{
			shader.setUniform("lights[" + std::to_string(i) + "].position", decompose(getWorldT(entities, ents[i])).position);
			shader.setUniform("lights[" + std::to_string(i) + "].colour", lights[ents[i]].colour);
			shader.setUniform("lights[" + std::to_string(i) + "].radius", lights[ents[i]].radius);
		}
	}
}