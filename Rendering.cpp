#include "Rendering.h"
#include "Core.h"

namespace ENG
{
	namespace CS
	{
		Camera::Camera(const glm::vec2& size, const float fov, const float near, const float far) : size(size), near(near), far(far)
		{
			aspect = size.x / size.y;
			fov_y = fov;
			fov_x = static_cast<float>(glm::degrees(2 * glm::atan(glm::tan(fov_y * 0.5) * aspect)));
		}

		glm::mat4 Camera::get()
		{
			return glm::perspective(glm::radians(fov_y), aspect, near, far);
		}
	}

	void drawToCameras(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Camera>& cameras = core.entities.getPool<CS::Camera>();

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Camera>())
		{
			cameras[id].frame.bind();

			glm::mat4 view = glm::inverse(getWorldM(core.entities, id));
			core.resources.shader("default.shdr").setUniform("view", view);
			core.resources.shader("skybox.shdr").setUniform("view", glm::mat4(glm::mat3(view)));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawSkybox(core.resources);
			drawModels(core);
			drawColliders(core);
			drawSprites3D(core);
			drawModelsToHUD(core);
			drawSprites(core);

			cameras[id].frame.unbind();
		}
	}

	void drawToScreen(Core& core)
	{
		// WRITE NEW SHADER THAT DRAWS SINGLE IMAGE, THE MAIN CAMERAS VIEW. ALSO ALLOWS POST PROCESSING
		std::vector<Vertex2D> verts_2dd = {
			Vertex2D({ 0.0f, 1.0f }, { 0.0f, 1.0f }),
			Vertex2D({ 1.0f, 0.0f }, { 1.0f, 0.0f }),
			Vertex2D({ 0.0f, 0.0f }, { 0.0f, 0.0f }),
			Vertex2D({ 0.0f, 1.0f }, { 0.0f, 1.0f }),
			Vertex2D({ 1.0f, 1.0f }, { 1.0f, 1.0f }),
			Vertex2D({ 1.0f, 0.0f }, { 1.0f, 0.0f })
		};

		Mesh2D m;
		m.setVertices(verts_2dd);
		m.bind();

		core.resources.shader("postprocess.shdr").bind();
		core.resources.texture("notexture.png").bind();

		glDrawArrays(GL_TRIANGLES, 0, 6);
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

	/**
	* Draws model components.
	*/
	void drawModels(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Model>& models = core.entities.getPool<CS::Model>();

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = models[id];
			CS::Transform t = getWorldT(core.entities, id);
			if (m.hud || !intersectOBBvFrustum(t, core.resources.mesh(m.mesh).getSize() * t.scale, *core.renderer.view, core.camera).intersects) continue;
			drawModel(core, m, t.get(), core.entities.hasComponent<ENG::CS::Light>(id));
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
		resources.texture("skybox.png").bind();
		glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube_inverted.obj").vertexCount());

		glDepthMask(GL_TRUE);
	}

	void spriteStart(Core& core)
	{
		// Create primitives
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

		// Create the framebuffers that each camera will draw to.
		//ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		//ComponentMap<CS::Camera>& cameras = core.entities.getPool<CS::Camera>();

		//for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Camera>())
		//	cameras[id].frame.create(cameras[id].size);
	}

	void updateSprites(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Sprite>& sprites = core.entities.getPool<CS::Sprite>();

		// Make sprites that billboard always face the camera.
		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Sprite>())
			if (sprites[id].billboard)
				transforms[id].rotation = core.renderer.view->rotation;

		// Work out which frame each sprite is on.
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

	void drawColliders(Core& core)
	{
		if (!core.renderer.draw_colliders)
			return;

		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::BoxCollider>& boxes = core.entities.getPool<CS::BoxCollider>();

		Mesh& cube = core.resources.mesh("cube.obj");
		cube.bind();

		glm::vec3 solid_colour(1.0f, 0.0f, 0.0f);
		glm::vec3 trigger_colour(0.0f, 0.0f, 1.0f);

		core.resources.shader("colliders.shdr").setUniform("projection", core.perspective);
		core.resources.shader("colliders.shdr").setUniform("view", glm::inverse(core.renderer.view->get()));

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::BoxCollider>())
		{
			CS::Transform t = getWorldT(core.entities, id);
			t.scale *= boxes[id].size;

			core.resources.shader("colliders.shdr").setUniform("transform", t.get());
			core.resources.shader("colliders.shdr").setUniform("colour", boxes[id].trigger ? trigger_colour : solid_colour);
			core.resources.shader("colliders.shdr").bind();

			glDrawArrays(GL_LINE_STRIP, 0, cube.vertexCount());
		}
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

			// Scale light radius by average of scale
			float scale_average = (t.scale.x + t.scale.y + t.scale.z) / 3.0f;

			shader.setUniform("lights[" + std::to_string(i) + "].position", t.position);
			shader.setUniform("lights[" + std::to_string(i) + "].colour", lights[ents[i]].colour);
			shader.setUniform("lights[" + std::to_string(i) + "].radius", lights[ents[i]].radius * scale_average);
		}
	}
}