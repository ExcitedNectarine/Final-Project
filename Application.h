#pragma once

#include "Window.h"
#include "Resources.h"
#include "Settings.h"
#include "CubeMap.h"
#include "Components.h"

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