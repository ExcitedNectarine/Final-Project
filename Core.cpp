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
		camera.near = 0.1f;
		camera.far = 500.0f;

		window.create(window_size, settings.get("title"));

		resources.loadMeshes(splitText(readTextFile(settings.get("meshes")), '\n'));
		resources.loadTextures(splitText(readTextFile(settings.get("textures")), '\n'));
		resources.loadSounds(splitText(readTextFile(settings.get("sounds")), '\n'));
		resources.loadShaders(splitText(readTextFile(settings.get("shaders")), '\n'));

		skybox.create(splitText(readTextFile(settings.get("skybox")), '\n'));
		skybox.bind();

		resources.shader("default.shader").setUniform("projection", camera.get());
		resources.shader("skybox.shader").setUniform("projection", camera.get());

		entities.addComponentPools<
			CS::Transform,
			CS::Model,
			CS::Light,
			CS::Script,
			CS::BoxCollider,
			CS::Controller,
			CS::Portal
		>();
	}

	void Core::run()
	{
		scriptStart(entities, *this);
		startPortals(entities);

		double current = 0.0, last = 0.0;
		while (!window.shouldClose())
		{
			current = glfwGetTime();
			delta = static_cast<float>(current - last);
			last = current;

			scriptUpdate(entities, *this);
			updatePortals(entities);

			testCollisions(entities, *this);
			setLights(entities, resources.shader("default.shader"));

			window.clear({ 0.0f, 0.0f, 0.0f, 0.0f });

			drawToPortals(entities, resources);

			resources.shader("default.shader").setUniform("view", glm::inverse(view.get()));
			resources.shader("default.shader").setUniform("view_pos", view.position);
			resources.shader("skybox.shader").setUniform("view", glm::mat4(glm::mat3(glm::inverse(view.get()))));

			glDepthMask(GL_FALSE);
			resources.shader("skybox.shader").bind();
			resources.mesh("cube.obj").bind();
			glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube.obj").vertexCount());
			glDepthMask(GL_TRUE);

			drawModels(entities, resources);
			drawPortals(entities, resources, camera.get(), glm::inverse(view.get()));

			window.display();
			glfwPollEvents();
		}

		scriptEnd(entities, *this);
	}
}