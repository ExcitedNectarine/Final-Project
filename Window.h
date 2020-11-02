#pragma once

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include "Output.h"

namespace ENG
{
	/**
	* Simple window class, provides basic window control and input.
	*/
	class Window
	{
	public:
		Window();
		Window(const glm::ivec2& size, const std::string& title);
		
		// Window funcs
		void create(const glm::ivec2& size, const std::string& title);
		void clear(const glm::vec4& colour);
		void display();
		bool shouldClose();
		void close();
		glm::ivec2 getSize();
		glm::ivec2 getPosition();

		// Input funcs
		glm::dvec2 getMousePos();
		void lockMouse(const bool lock);
		bool isKeyPressed(int key);
		bool isKeyPressedOnce(int key);
		bool isMouseButtonPressed(int button);
		bool isMouseButtonPressedOnce(int button);

	private:
		GLFWwindow* window;
		glm::dvec2 mouse_pos;
		glm::ivec2 size;
		glm::ivec2 position;
	};
}