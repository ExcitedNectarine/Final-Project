#include "Core.h"

namespace ENG
{
	Core::Core(const std::string& setting_file)
	{
		ENG::audioInit();

		settings.load(setting_file);

		glm::ivec2 window_size(settings.geti("width"), settings.geti("height"));
		glm::mat4 projection = glm::perspective(90.0f, static_cast<float>(window_size.x) / window_size.y, 0.1f, 500.0f);
		window.create(window_size, settings.get("title"));

		resources.loadMeshes(splitText(readTextFile(settings.get("meshes")), '\n'));
		resources.loadTextures(splitText(readTextFile(settings.get("textures")), '\n'));
		resources.loadSounds(splitText(readTextFile(settings.get("sounds")), '\n'));
		resources.loadShaders(splitText(readTextFile(settings.get("shaders")), '\n'));

		skybox.create(splitText(readTextFile(settings.get("skybox")), '\n'));

		resources.shader("default.shader").setUniform("projection", projection);
		resources.shader("skybox.shader").setUniform("projection", projection);

		entities.addComponentPools<CS::Transform, CS::Model, CS::Light, CS::Script, CS::SphereCollider, CS::BoxCollider, CS::Controller>();
	}

	void Core::run()
	{
		scriptStart(entities, *this);

		double current = 0.0, last = 0.0;
		while (!window.shouldClose())
		{
			current = glfwGetTime();
			delta = static_cast<float>(current - last);
			last = current;

			setLights(entities, resources.shader("default.shader"));
			testCollisions(entities, *this);
			moveControllers(entities);
			scriptUpdate(entities, *this);

			resources.shader("default.shader").setUniform("view", glm::inverse(view));
			resources.shader("skybox.shader").setUniform("view", glm::mat4(glm::mat3(glm::inverse(view))));

			window.clear({ 0.0f, 0.0f, 0.0f, 0.0f });

			glDepthMask(GL_FALSE);
			skybox.bind();
			resources.shader("skybox.shader").bind();
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