#include "Core.h"

namespace ENG
{
	Core::Core(const std::string& setting_file)
	{
		ENG::audioInit();

		settings.load(setting_file);

		glm::ivec2 window_size(settings.geti("width"), settings.geti("height"));

		camera.fov = 90.0f;
		camera.aspect = static_cast<float>(window_size.x) / window_size.y;

		window.create(window_size, settings.get("title"));

		resources.loadMeshes(splitText(readTextFile(settings.get("meshes")), '\n'));
		resources.loadTextures(splitText(readTextFile(settings.get("textures")), '\n'));
		resources.loadSounds(splitText(readTextFile(settings.get("sounds")), '\n'));
		resources.loadShaders(splitText(readTextFile(settings.get("shaders")), '\n'));

		skybox.create(splitText(readTextFile(settings.get("skybox")), '\n'));
		skybox.bind();

		resources.shader("default.shdr").setUniform("projection", camera.get());
		resources.shader("unshaded.shdr").setUniform("projection", camera.get());
		resources.shader("skybox.shdr").setUniform("projection", camera.get());

		entities.addComponentPools<
			CS::Transform,
			CS::Model,
			CS::Light,
			CS::Script,
			CS::BoxCollider,
			CS::Controller,
			CS::Portal,
			CS::Camera
		>();
	}

	void Core::run()
	{
		scriptStart(entities, *this);
		startPortals(entities, window.getSize());

		double current = 0.0, last = 0.0;
		while (!window.shouldClose())
		{
			current = glfwGetTime();
			delta = static_cast<float>(current - last);
			last = current;

			scriptUpdate(entities, *this);
			moveControllers(entities, *this);

			updatePortals(entities);
			drawToPortals(entities, resources);

			setLights(entities, resources.shader("default.shdr"));
			resources.shader("default.shdr").setUniform("view", glm::inverse(view->get()));
			resources.shader("default.shdr").setUniform("view_pos", view->position);
			resources.shader("unshaded.shdr").setUniform("view", glm::inverse(view->get()));
			resources.shader("skybox.shdr").setUniform("view", glm::mat4(glm::mat3(glm::inverse(view->get()))));

			window.clear({ 0.0f, 0.0f, 0.0f, 0.0f });
			
			drawSkybox(resources);
			drawModels(entities, resources, view->position);
			drawPortals(entities, resources, camera, glm::inverse(view->get()));
			drawModelsToHUD(entities, resources, view->position);

			window.display();

			glfwPollEvents();
		}

		scriptEnd(entities, *this);
	}
}