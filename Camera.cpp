#include "Camera.h"

namespace ENG
{
	namespace CS
	{
		glm::mat4 Camera::get() { return glm::perspective(fov, aspect, near, far); }
	}
}