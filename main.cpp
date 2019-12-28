#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Window.h"
#include "Transform.h"
#include "Tools.h"
#include "Entities.h"
#include "Resources.h"
#include "CubeMap.h"
#include "FrameBuffer.h"
#include "Settings.h"
#include <glm/gtx/string_cast.hpp>

namespace CS
{
	struct Transform : ENG::Transform, ENG::ECSComponent<Transform> {};

	struct Model : ENG::ECSComponent<Model>
	{
		ENG::Mesh* mesh;
		ENG::Texture* texture;
		ENG::Shader* shader;
	};

	struct Light : ENG::ECSComponent<Light>
	{
		Light() : colour(1.0f, 1.0f, 1.0f), radius(5.0f) {}

		glm::vec3 colour;
		float radius;
	};
}

void drawModels(ENG::Entities& entities)
{
	auto& ts = entities.getPool<CS::Transform>();
	auto& ms = entities.getPool<CS::Model>();

	for (ENG::EntityID entity : entities.entitiesWith<CS::Transform, CS::Model>())
	{
		CS::Model& m = ms[entity];

		m.shader->setUniform("transform", ts[entity].get());
		m.shader->bind();
		m.mesh->bind();
		m.texture->bind();
		glDrawArrays(GL_TRIANGLES, 0, m.mesh->vertexCount());
	}
}

void setLights(ENG::Entities& entities, ENG::Shader& shader)
{
	auto& ts = entities.getPool<CS::Transform>();
	auto& ls = entities.getPool<CS::Light>();

	std::vector<ENG::EntityID> ents = entities.entitiesWith<CS::Transform, CS::Light>();
	for (std::size_t i = 0; i < ents.size(); i++)
	{
		shader.setUniform("lights[" + std::to_string(i) + "].position", ts[ents[i]].position);
		shader.setUniform("lights[" + std::to_string(i) + "].colour", ls[ents[i]].colour);
		shader.setUniform("lights[" + std::to_string(i) + "].radius", ls[ents[i]].radius);
	}
}

void run()
{
	ENG::Settings s("Resources/settings.set");

	glm::ivec2 window_size(s.geti("width"), s.geti("height"));
	glm::mat4 projection = glm::perspective(90.0f, static_cast<float>(window_size.x) / window_size.y, 0.1f, 500.0f);

	ENG::Window window(window_size, "ENG");
	ENG::Entities entities;
	ENG::Resources resources;

	ENG::Shader def_shader(
		"Resources/Shaders/default.vert",
		"Resources/Shaders/default.frag"
	);

	ENG::Shader skybox_shader(
		"Resources/Shaders/skybox.vert",
		"Resources/Shaders/skybox.frag"
	);

	ENG::Shader fb_shader(
		"Resources/Shaders/framebuffer.vert",
		"Resources/Shaders/framebuffer.frag"
	);

	ENG::CubeMap cubemap;
	cubemap.create({
		"Resources/Textures/right.png",
		"Resources/Textures/left.png",
		"Resources/Textures/up.png",
		"Resources/Textures/down.png",
		"Resources/Textures/back.png",
		"Resources/Textures/front.png"
		});

	resources.loadMeshes({
		"Resources/Meshes/cube.obj",
		"Resources/Meshes/lorry.obj",
		"Resources/Meshes/car.obj"
		//"Resources/Meshes/skull.obj"
		});
	resources.loadTextures({
		"Resources/Textures/lorry.jpg",
		"Resources/Textures/rock.png",
		"Resources/Textures/skull.jpg"
		});

	def_shader.setUniform("projection", projection);
	def_shader.setUniform("ambient", { 0.2f, 0.2f, 0.2f });

	skybox_shader.setUniform("projection", projection);

	entities.addComponentPools<CS::Transform, CS::Model, CS::Light>();

	for (int i = -5; i < 5; i++)
	{
		auto e = entities.addEntity();
		auto& m = entities.addComponent<CS::Model>(e);
		m.mesh = &resources.mesh("lorry.obj");
		m.texture = &resources.texture("lorry.jpg");
		m.shader = &def_shader;

		auto& t = entities.addComponent<CS::Transform>(e);
		t.position = { i * 5.0f, 0.0f, -5.0f };
		t.rotation = { -90.0f, 0.0f, -90.0f };
		t.scale *= 0.5f;
	}

	auto p = entities.addEntity<CS::Transform, CS::Light>();
	auto& pview = entities.getComponent<CS::Transform>(p);
	def_shader.setUniform("view_pos", pview.position);
	def_shader.setUniform("view", glm::inverse(pview.get()));
	skybox_shader.setUniform("view", glm::mat4(glm::mat3(glm::inverse(pview.get()))));

	while (!window.shouldClose())
	{
		if (window.isKeyPressed(GLFW_KEY_ESCAPE))
			window.close();

		if (window.isKeyPressed(GLFW_KEY_SPACE))
			entities.clear();

		setLights(entities, def_shader);

		def_shader.setUniform("view_pos", pview.position);
		def_shader.setUniform("view", glm::inverse(pview.get()));
		skybox_shader.setUniform("view", glm::mat4(glm::mat3(glm::inverse(pview.get()))));
		
		window.clear({ 0.0f, 0.0f, 0.0f, 0.0f });

		// draw skybox
		glDepthMask(GL_FALSE);
		cubemap.bind();
		skybox_shader.bind();
		resources.mesh("cube.obj").bind();
		glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube.obj").vertexCount());
		glDepthMask(GL_TRUE);

		drawModels(entities);
		
		window.display();

		glfwPollEvents();
	}
}

int main()
{
	try
	{
		run();
	}
	catch (const std::exception& e)
	{
		OUTPUT_ERROR(e.what());
		PAUSE_CONSOLE;
	}

	return 0;
}