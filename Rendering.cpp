#include "Rendering.h"
#include "Core.h"

namespace ENG
{
	namespace CS
	{
		Camera::Camera(const glm::vec2& size, const float fov, const float near, const float far) { create(size, fov, near, far); }
		void Camera::create(const glm::vec2& size, const float fov, const float near, const float far)
		{
			aspect = size.x / size.y;
			fov_y = fov;
			fov_x = static_cast<float>(glm::degrees(2 * glm::atan(glm::tan(fov_y * 0.5) * aspect)));
			this->near = near;
			this->far = far;
			this->size = size;
		}

		glm::mat4 Camera::get() { return glm::perspective(glm::radians(fov_y), aspect, near, far); }

		void Text::setText(const std::string& new_text, Core& core)
		{
			if (text == new_text) return;
			text = new_text;

			// SETTING UP FOR MESH GENERATION
			std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+.?!%:()',";
			glm::ivec2 frames(12, 4);
			glm::vec2 frame_size(1.0f / frames.x, 1.0f / frames.y);
			glm::vec2 luv;
			glm::vec2 huv;

			std::map<char, glm::vec2> char_frames;
			int count = 0;
			for (int y = frames.y; y > 0; y--)
				for (int x = 1; x <= frames.x; x++)
					char_frames[tolower(alphabet[count++])] = { x, y };

			// Mesh generation
			std::vector<Vertex2D> vertices;
			for (std::size_t i = 0; i < text.length(); i++)
			{
				// for each character, make it lwoercase
				char c = tolower(text[i]);

				if (c == ' ')
					huv = luv = glm::vec2(0.0f); // no texture for space
				else
				{
					huv = frame_size * char_frames[c]; // higher and lower uvs
					luv = huv - frame_size;
				}

				// quad for current character
				std::vector<Vertex2D> verts_2d = {
					Vertex2D({ i, 1.0f }, luv),
					Vertex2D({ 1.0f + i, 0.0f }, huv),
					Vertex2D({ i, 0.0f }, { luv.x, huv.y }),
					Vertex2D({ i, 1.0f }, luv),
					Vertex2D({ 1.0f + i, 1.0f }, { huv.x, luv.y }),
					Vertex2D({ 1.0f + i, 0.0f }, huv)
				};

				// Add quad to vertex array.
				vertices.insert(vertices.end(), verts_2d.begin(), verts_2d.end());
			}

			mesh.setVertices(vertices);
		}
	}

	void drawToCameras(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Camera>& cameras = core.entities.getPool<CS::Camera>();

		std::vector<EntityID> draw_order = core.entities.entitiesWith<CS::Transform, CS::Camera>();
		std::sort(draw_order.begin(), draw_order.end(), [cameras](EntityID a, EntityID b) { return cameras.at(a).order <= cameras.at(b).order; });

		EntityID main = core.renderer.view_id;
		for (EntityID id : draw_order)
		{
			// Camera is on no layers, render nothing.
			if (cameras[id].layers.none()) continue;

			cameras[id].frame.bind();

			core.renderer.view_id = id;
			updateRenderer(core, glm::inverse(getWorldM(core.entities, id)), cameras[id].get());

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawSkybox(core.resources);
			drawModels(core, cameras[id].layers);
			drawColliders(core);
			drawSprites3D(core);
			renderText3D(core);

			cameras[id].frame.unbind();
		}
		core.renderer.view_id = main;
	}

	void drawToScreen(Core& core)
	{
		CS::Transform2D t;
		t.scale *= core.window.getSize();
		
		core.resources.shader2D("postprocess.shdr").setUniform("transform", t.get());
		quad_2d.bind();
		core.resources.shader2D("postprocess.shdr").bind();
		core.entities.getComponent<CS::Camera>(core.renderer.view_id).frame.getTexture().bind();

		glDrawArrays(GL_TRIANGLES, 0, 6);

		drawSprites(core);
		renderText(core);
		drawModelsToHUD(core);
	}

	void startRenderer(Core& core)
	{
		std::vector<Vertex2D> verts_2d = {
			Vertex2D({ 0.0f, 1.0f }, { 0.0f, 0.0f }),
			Vertex2D({ 1.0f, 0.0f }, { 1.0f, 1.0f }),
			Vertex2D({ 0.0f, 0.0f }, { 0.0f, 1.0f }),
			Vertex2D({ 0.0f, 1.0f }, { 0.0f, 0.0f }),
			Vertex2D({ 1.0f, 1.0f }, { 1.0f, 0.0f }),
			Vertex2D({ 1.0f, 0.0f }, { 1.0f, 1.0f })
		};

		std::vector<Vertex> verts_3d = {
			Vertex({ -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }),
			Vertex({ 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }),
			Vertex({ -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }),
			Vertex({ -1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }),
			Vertex({ 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }),
			Vertex({ 1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f })
		};

		quad_2d.setVertices(verts_2d);
		quad_3d.setVertices(verts_3d);

		glm::vec2 window_size(core.window.getSize());
		core.resources.shader2D("postprocess.shdr").setUniform("projection", glm::ortho(0.0f, window_size.x, window_size.y, 0.0f));
		core.resources.shader2D("sprite.shdr").setUniform("projection", glm::ortho(0.0f, window_size.x, window_size.y, 0.0f));

		// Create the framebuffers that each camera will draw to.
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Camera>& cameras = core.entities.getPool<CS::Camera>();

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Camera>())
		{
			if (cameras[id].size == glm::ivec2(0))
				cameras[id].create(core.window.getSize(), core.settings.getf("fov"), 0.1f, 250.0f);
			cameras[id].frame.create(cameras[id].size);
		}
	}

	void updateRenderer(Core& core, glm::mat4 view, glm::mat4 projection)
	{
		for (auto& shader : core.resources.allShaders3D())
		{
			setLights(core, shader.second);
			shader.second.setUniform("projection", projection);

			if (shader.first == "skybox.shdr")
				shader.second.setUniform("view", glm::mat4(glm::mat3(view)));
			else
				shader.second.setUniform("view", view);
		}
	}

	void drawModel(Mesh& mesh, Texture& texture, Shader& shader, glm::mat4 t, bool emitter)
	{
		shader.setUniform("transform", t);
		shader.setUniform("emitter", emitter);
		shader.bind();

		mesh.bind();
		texture.bind();

		glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
	}

	/**
	* Draws model components.
	*/
	void drawModels(Core& core, std::bitset<MAX_LAYERS> layers)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Model>& models = core.entities.getPool<CS::Model>();
		ComponentMap<CS::Camera>& cameras = core.entities.getPool<CS::Camera>();

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Model>())
		{
			CS::Model& m = models[id];

			// model is on no layers, draw nothing
			if (m.layers.none()) continue;

			// if one of m.layers bits is 1 and the same bit is 1 in layers, draw
			bool draw = false;
			for (std::size_t i = 0; i < MAX_LAYERS; i++)
			{
				if (m.layers[i] && layers[i])
				{
					draw = true;
					break;
				}
			}
			if (!draw) continue;

			CS::Transform t = getWorldT(core.entities, id);
			if (m.hud || !intersectOBBvFrustum(t, core.resources.mesh(m.mesh).getSize() * t.scale, getWorldT(core.entities, core.renderer.view_id), cameras[core.renderer.view_id]).intersects) continue;

			if (!m.backface_culling) glDisable(GL_CULL_FACE);

			drawModel(
				core.resources.mesh(m.mesh),
				m.camera_output == 0 ? core.resources.texture(m.texture) : cameras[m.camera_output].frame.getTexture(),
				core.resources.shader3D(m.shader),
				getWorldM(core.entities, id),
				core.entities.hasComponent<ENG::CS::Light>(id)
			);

			glEnable(GL_CULL_FACE);
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

			drawModel(
				core.resources.mesh(m.mesh),
				core.resources.texture(m.texture),
				core.resources.shader3D(m.shader),
				getWorldM(core.entities, id),
				core.entities.hasComponent<ENG::CS::Light>(id)
			);
		}
	}

	void drawSkybox(Resources& resources)
	{
		glDepthMask(GL_FALSE);
		
		resources.shader3D("skybox.shdr").bind();
		resources.mesh("cube_inverted.obj").bind();
		resources.texture("skybox.png").bind();
		glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube_inverted.obj").vertexCount());

		glDepthMask(GL_TRUE);
	}

	void updateSprites(Core& core)
	{
		ComponentMap<CS::Sprite>& sprites = core.entities.getPool<CS::Sprite>();

		// Work out which frame each sprite is on.
		for (EntityID id : core.entities.entitiesWith<CS::Sprite>())
		{
			CS::Sprite& s = sprites[id];
			if (s.animated)
			{
				s.timer += core.clock.deltaTime();
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

				quad_2d[0].uv = luv;
				quad_2d[1].uv = huv;
				quad_2d[2].uv = { luv.x, huv.y };
				quad_2d[3].uv = luv;
				quad_2d[4].uv = { huv.x, luv.y };
				quad_2d[5].uv = huv;
			}

			t = transforms[id];
			t.scale *= size;
			core.resources.shader2D("sprite.shdr").setUniform("transform", t.get());

			quad_2d.bind();
			core.resources.shader2D("sprite.shdr").bind();
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
			distances.emplace_back(glm::distance(getWorldT(core.entities, core.renderer.view_id).position, getWorldT(core.entities, id).position), id);
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
			if (s.billboard) t.rotation = getWorldT(core.entities, core.renderer.view_id).rotation;

			core.resources.shader3D("default.shdr").setUniform("transform", t.get());
			core.resources.shader3D("default.shdr").setUniform("emitter", core.entities.hasComponent<ENG::CS::Light>(it->second));
			core.resources.shader3D("default.shdr").bind();

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

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::BoxCollider>())
		{
			CS::Transform t = getWorldT(core.entities, id);
			t.scale *= boxes[id].size;

			core.resources.shader3D("colliders.shdr").setUniform("transform", t.get());
			core.resources.shader3D("colliders.shdr").setUniform("colour", boxes[id].trigger ? trigger_colour : solid_colour);
			core.resources.shader3D("colliders.shdr").bind();

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

	void renderText(Core& core)
	{
		ComponentMap<CS::Transform2D>& transforms = core.entities.getPool<CS::Transform2D>();
		ComponentMap<CS::Text>& texts = core.entities.getPool<CS::Text>();

		for (EntityID id : core.entities.entitiesWith<CS::Transform2D, CS::Text>())
		{
			CS::Transform2D t = transforms[id];
			t.scale *= core.resources.texture("font.png").getSize() / glm::ivec2(12, 3);

			core.resources.shader2D("sprite.shdr").setUniform("transform", t.get());
			texts[id].mesh.bind();
			core.resources.shader2D("sprite.shdr").bind();
			core.resources.texture("font.png").bind();

			glDrawArrays(GL_TRIANGLES, 0, texts[id].mesh.vertexCount());
		}
	}

	void renderText3D(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Text>& texts = core.entities.getPool<CS::Text>();

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Text>())
		{
		}
	}
}