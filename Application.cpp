#include "Application.h"

namespace ENG
{
	Application::Application(const std::string& setting_file)
	{
		ENG::audioInit();

		settings.load(setting_file);

		glm::ivec2 window_size(settings.geti("width"), settings.geti("height"));
		glm::mat4 projection = glm::perspective(90.0f, static_cast<float>(window_size.x) / window_size.y, 0.1f, 500.0f);
		window.create(window_size, settings.get("title"));

		def_shader.create("Resources/Shaders/default.vert", "Resources/Shaders/default.frag");
		def_shader.setUniform("projection", projection);

		skybox_shader.create("Resources/Shaders/skybox.vert", "Resources/Shaders/skybox.frag");
		skybox_shader.setUniform("projection", projection);

		resources.loadMeshes(splitText(readTextFile(settings.get("meshes")), '\n'));
		resources.loadTextures(splitText(readTextFile(settings.get("textures")), '\n'));
		resources.loadSounds(splitText(readTextFile(settings.get("sounds")), '\n'));
		skybox.create(splitText(readTextFile(settings.get("skybox")), '\n'));

		entities.addComponentPools<CS::Transform, CS::Model, CS::Light, CS::Script, CS::SphereCollider, CS::BoxCollider>();
	}

	void Application::run()
	{
		scriptStart(entities, *this);

		double current = 0.0, last = 0.0;
		while (!window.shouldClose())
		{
			current = glfwGetTime();
			delta = current - last;
			last = current;

			setLights(entities, def_shader);
			testCollisions(entities, *this);
			scriptUpdate(entities, *this);

			def_shader.setUniform("view", glm::inverse(view));
			skybox_shader.setUniform("view", glm::mat4(glm::mat3(glm::inverse(view))));

			window.clear({ 0.0f, 0.0f, 0.0f, 0.0f });

			glDepthMask(GL_FALSE);
			skybox.bind();
			skybox_shader.bind();
			resources.mesh("cube.obj").bind();
			glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube.obj").vertexCount());
			glDepthMask(GL_TRUE);

			drawModels(entities);
			window.display();
			glfwPollEvents();
		}

		scriptEnd(entities, *this);
	}
}