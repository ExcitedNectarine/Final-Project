#pragma once

#include "Entities.h"

namespace ENG
{
	class Application;

	/**
	* Base script class that all scripts must inherit from.
	*/
	struct Script
	{
		/**
		* Called just before the main game loop.
		*/
		virtual void start(Application& app) {}

		/**
		* Called every frame.
		*/
		virtual void update(Application& app) {}

		/**
		* Called after the game loop has ended.
		*/
		virtual void end(Application& app) {}

		/**
		* Called if entity collides with another entity
		*/
		virtual void onCollision(Application& app, EntityID hit_id) {}

		EntityID id;
	};

	namespace CS
	{
		/**
		* Script component, used for adding scripts to entities.
		*/
		struct Script : ECSComponent<Script>
		{
			// Because the entity manager stores components by value, we have to use one level of
			// indirection and store a pointer to the script.
			std::unique_ptr<ENG::Script> script;
		};
	}

	void scriptStart(Entities& entities, Application& app);
	void scriptUpdate(Entities& entities, Application& app);
	void scriptEnd(Entities& entities, Application& app);
}