#pragma once

namespace ENG
{
	class Application;
	struct Script
	{
		virtual void start(Application& app) {}
		virtual void update(Application& app) {}
		virtual void end(Application& app) {}

		EntityID id;
	};
}