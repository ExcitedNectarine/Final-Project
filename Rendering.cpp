#include "Rendering.h"
#include "Core.h"
#include <glm/gtx/string_cast.hpp>

namespace ENG
{
	void updateModels(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Model>& models = core.entities.getPool<CS::Model>();

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Model>())
			if (models[id].billboard)
				transforms[id].rotation = core.view->rotation;
	}

	void drawModel(Core& core, CS::Model& m, glm::mat4 t)
	{
		if (m.shaded)
		{
			core.resources.shader("default.shdr").setUniform("transform", t);
			core.resources.shader("default.shdr").bind();
		}
		else
		{
			core.resources.shader("unshaded.shdr").setUniform("transform", t);
			core.resources.shader("unshaded.shdr").bind();
		}

		core.resources.mesh(m.mesh).bind();
		core.resources.texture(m.texture).bind();

		glDrawArrays(GL_TRIANGLES, 0, core.resources.mesh(m.mesh).vertexCount());
	}

	/**
	* Draws model components.
	*/
	void drawModels(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Model>& models = core.entities.getPool<CS::Model>();

		std::vector<std::pair<float, EntityID>> distances;
		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Model>())
			distances.emplace_back(glm::distance(core.view->position, transforms[id].position), id);
		std::sort(distances.begin(), distances.end());

		// Draw opaque models from closest to furthest.
		for (auto& p : distances)
		{
			CS::Model& m = models[p.second];
			if (m.hud || m.transparent) continue;
			drawModel(core, m, getWorldT(core.entities, p.second));
		}

		// Draw transparent models from furthest to closest.
		for (std::vector<std::pair<float, EntityID>>::reverse_iterator it = distances.rbegin(); it != distances.rend(); ++it)
		{
			CS::Model& m = models[it->second];
			if (m.hud || !m.transparent) continue;
			drawModel(core, m, getWorldT(core.entities, it->second));
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
			drawModel(core, m, getWorldT(core.entities, id));
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

	Mesh2D quad;
	void spriteStart()
	{
		std::vector<Vertex2D> verts = {
			Vertex2D({0.0f, 1.0f}, {0.0f, 0.0f}),
			Vertex2D({1.0f, 0.0f}, {1.0f, 1.0f}),
			Vertex2D({0.0f, 0.0f}, {0.0f, 1.0f}),
			Vertex2D({0.0f, 1.0f}, {0.0f, 0.0f}),
			Vertex2D({1.0f, 1.0f}, {1.0f, 0.0f}),
			Vertex2D({1.0f, 0.0f}, {1.0f, 1.0f})
		};
		quad.setVertices(verts);
	}

	void drawSprites(Core& core)
	{
		ComponentMap<CS::Transform2D>& transforms = core.entities.getPool<CS::Transform2D>();
		ComponentMap<CS::Sprite>& sprite = core.entities.getPool<CS::Sprite>();

		CS::Transform2D t;
		for (EntityID id : core.entities.entitiesWith<CS::Transform2D, CS::Sprite>())
		{
			t = transforms[id];
			t.scale *= core.resources.texture(sprite[id].texture).getSize();
			core.resources.shader("sprite.shdr").setUniform("transform", t.get());

			core.resources.shader("sprite.shdr").bind();
			quad.bind();
			core.resources.texture(sprite[id].texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
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