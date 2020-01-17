#pragma once

#include "Window.h"
#include "Entities.h"
#include "Resources.h"
#include "Settings.h"
#include "Shader.h"
#include "Components.h"
#include "CubeMap.h"
#include <glm/gtc/matrix_transform.hpp>

namespace ENG
{
	/**
	* Main application class, all game objects and entities are used in this class.
	*/
	class Application
	{
	public:
		Application(const std::string& setting_file);

		/**
		* Run main game loop.
		*/
		void run();

		void scriptStart();
		void scriptUpdate();
		void scriptEnd();
		void setView(const glm::mat4& view) { this->view = view; }

		Window& getWindow() { return window; }
		Entities& getEntities() { return entities; }
		Settings& getSettings() { return settings; }
		Resources& getResources() { return resources; }
		Shader& getShader() { return def_shader; }

		float getDeltaTime() { return static_cast<float>(delta); }

	private:
		Window window;
		Entities entities;
		Resources resources;
		Settings settings;
		Shader def_shader;
		Shader skybox_shader;
		CubeMap skybox;

		glm::mat4 view;

		double delta;
	};
}