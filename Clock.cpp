#include "Clock.h"

namespace ENG
{
	float Clock::deltaTime()
	{
		return delta;
	}

	void Clock::update()
	{
		current = glfwGetTime();
		delta = static_cast<float>(current - last);
		last = current;
	}
}