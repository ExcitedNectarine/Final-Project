#include "Rendering.h"
#include "Core.h"

namespace ENG
{
	namespace CS
	{
		Sprite::Sprite()
			: frame(1),
			frames(1) {}
	}

	void updateRenderer(Core& core)
	{
		glm::mat4 view = glm::inverse(core.renderer.view->get());

		setLights(core, core.resources.shader("default.shdr"));
		core.resources.shader("default.shdr").setUniform("view", view);
		core.resources.shader("skybox.shdr").setUniform("view", glm::mat4(glm::mat3(view)));
	}

	void drawModel(Core& core, CS::Model& m, glm::mat4 t, bool emitter)
	{
		core.resources.shader("default.shdr").setUniform("transform", t);
		core.resources.shader("default.shdr").setUniform("emitter", emitter);
		core.resources.shader("default.shdr").bind();

		core.resources.mesh(m.mesh).bind();
		core.resources.texture(m.texture).bind();

		glDrawArrays(GL_TRIANGLES, 0, core.resources.mesh(m.mesh).vertexCount());
	}

	bool inView(CS::Transform& view, const glm::vec3& pos, const glm::vec3& size)
	{
		glm::vec3 min = pos - size;
		glm::vec3 max = pos + size;

		// All 8 vertices in AABB
		glm::vec3 verts[8] =
		{
			min,
			max,
			{ max.x, min.y, min.z },
			{ min.x, max.y, min.z },
			{ min.x, min.y, max.z },
			{ min.x, max.y, max.z },
			{ max.x, min.y, max.z },
			{ max.x, max.y, min.z }
		};

		for (int i = 0; i < 8; i++)
		{
			if (glm::dot(view.forward(), view.position - verts[i]) > 0)
				return true;
		}

		return false;
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
			distances.emplace_back(glm::distance(core.renderer.view->position, getWorldT(core.entities, id).position), id);
		std::sort(distances.begin(), distances.end());

		int i = 0;

		// Draw opaque models from closest to furthest.
		for (auto& p : distances)
		{
			CS::Model& m = models[p.second];
			CS::Transform t = getWorldT(core.entities, p.second);

			if (m.hud || !inView(*core.renderer.view, t.position, core.resources.mesh(m.mesh).getSize() * t.scale)) continue;
			drawModel(core, m, t.get(), core.entities.hasComponent<ENG::CS::Light>(p.second));
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
			drawModel(core, m, getWorldM(core.entities, id), core.entities.hasComponent<ENG::CS::Light>(id));
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

	void spriteStart()
	{
		std::vector<Vertex2D> verts_2d = {
			Vertex2D({ 0.0f, 1.0f }, { 0.0f, 1.0f }),
			Vertex2D({ 1.0f, 0.0f }, { 1.0f, 0.0f }),
			Vertex2D({ 0.0f, 0.0f }, { 0.0f, 0.0f }),
			Vertex2D({ 0.0f, 1.0f }, { 0.0f, 1.0f }),
			Vertex2D({ 1.0f, 1.0f }, { 1.0f, 1.0f }),
			Vertex2D({ 1.0f, 0.0f }, { 1.0f, 0.0f })
		};

		std::vector<Vertex> verts_3d = {
			Vertex({ -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }),
			Vertex({ 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }),
			Vertex({ -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }),
			Vertex({ -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }),
			Vertex({ 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f }),
			Vertex({ 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f })
		};

		quad_2d.setVertices(verts_2d);
		quad_3d.setVertices(verts_3d);
	}

	void updateSprites(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Sprite>& sprites = core.entities.getPool<CS::Sprite>();

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Sprite>())
			if (sprites[id].billboard)
				transforms[id].rotation = core.renderer.view->rotation;

		for (EntityID id : core.entities.entitiesWith<CS::Sprite>())
		{
			CS::Sprite& s = sprites[id];
			if (s.animated)
			{
				s.timer += core.delta;
				if (s.timer >= s.frame_time)
				{
					s.timer = 0.0f;
					if (++s.frame.x > s.frames.x)
					{
						s.frame.x = 1;
						if (++s.frame.y > s.frames.y)
							s.frame.y = 1;
					}
				}
			}
		}
	}

	/**
	* Draws sprites with Transform2D components.
	*/
	void drawSprites(Core& core)
	{
		ComponentMap<CS::Transform2D>& transforms = core.entities.getPool<CS::Transform2D>();
		ComponentMap<CS::Sprite>& sprites = core.entities.getPool<CS::Sprite>();

		glm::vec2 frame_size;
		glm::vec2 size;
		glm::vec2 huv;
		glm::vec2 luv;

		CS::Transform2D t;
		for (EntityID id : core.entities.entitiesWith<CS::Transform2D, CS::Sprite>())
		{	
			CS::Sprite& s = sprites[id];

			size = core.resources.texture(s.texture).getSize();
			if (s.animated)
			{
				size /= s.frames;

				frame_size.x = 1.0f / s.frames.x;
				frame_size.y = 1.0f / s.frames.y;

				huv = frame_size * glm::vec2(s.frame);
				luv = huv - frame_size;

				quad_2d[0].uv = { luv.x, huv.y };
				quad_2d[1].uv = { huv.x, luv.y };
				quad_2d[2].uv = luv;
				quad_2d[3].uv = { luv.x, huv.y };
				quad_2d[4].uv = huv;
				quad_2d[5].uv = { huv.x, luv.y };
			}

			t = transforms[id];
			t.scale *= size;
			core.resources.shader("sprite.shdr").setUniform("transform", t.get());

			quad_2d.bind();
			core.resources.shader("sprite.shdr").bind();
			core.resources.texture(s.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	/**
	* Draws sprites with the Transform component.
	*/
	void drawSprites3D(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Sprite>& sprites = core.entities.getPool<CS::Sprite>();

		std::vector<std::pair<float, EntityID>> distances;
		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Sprite>())
			distances.emplace_back(glm::distance(core.renderer.view->position, getWorldT(core.entities, id).position), id);
		std::sort(distances.begin(), distances.end());

		glm::vec2 frame_size;
		glm::vec2 size;
		glm::vec2 huv;
		glm::vec2 luv;

		glDisable(GL_CULL_FACE);

		CS::Transform t;
		for (std::vector<std::pair<float, EntityID>>::reverse_iterator it = distances.rbegin(); it != distances.rend(); ++it)
		{
			CS::Sprite& s = sprites[it->second];

			size = core.resources.texture(s.texture).getSize() / 100;
			if (s.animated)
			{
				size /= s.frames;

				frame_size.x = 1.0f / s.frames.x;
				frame_size.y = 1.0f / s.frames.y;

				huv = frame_size * glm::vec2(s.frame);
				luv = huv - frame_size;

				quad_3d[0].uv = { luv.x, huv.y };
				quad_3d[1].uv = { huv.x, luv.y };
				quad_3d[2].uv = luv;
				quad_3d[3].uv = { luv.x, huv.y };
				quad_3d[4].uv = huv;
				quad_3d[5].uv = { huv.x, luv.y };
			}

			t = getWorldT(core.entities, it->second);
			t.scale *= glm::vec3(size, 1.0f);

			core.resources.shader("default.shdr").setUniform("transform", t.get());
			core.resources.shader("default.shdr").setUniform("emitter", core.entities.hasComponent<ENG::CS::Light>(it->second));
			core.resources.shader("default.shdr").bind();

			quad_3d.bind();
			core.resources.texture(s.texture).bind();

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glEnable(GL_CULL_FACE);
	}

	/**
	* Uploads lighting information to shader.
	*/
	void setLights(Core& core, Shader& shader)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Light>& lights = core.entities.getPool<CS::Light>();

		shader.setUniform("ambient", core.renderer.ambient);

		std::vector<EntityID> ents = core.entities.entitiesWith<CS::Transform, CS::Light>();
		for (std::size_t i = 0; i < ents.size(); i++)
		{
			CS::Transform t = getWorldT(core.entities, ents[i]);

			shader.setUniform("lights[" + std::to_string(i) + "].position", t.position);
			shader.setUniform("lights[" + std::to_string(i) + "].colour", lights[ents[i]].colour);
			shader.setUniform("lights[" + std::to_string(i) + "].radius", lights[ents[i]].radius);
		}
	}
}