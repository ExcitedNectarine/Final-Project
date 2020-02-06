#pragma once

#include "Entities.h"

namespace ENG
{
	struct Core;

	/**
	* Base script class that all scripts must inherit from.
	*/
	struct Script
	{
		/**
		* Called just before the main game loop.
		*/
		virtual void start(Core& core) {}

		/**
		* Called every frame.
		*/
		virtual void update(Core& core) {}

		/**
		* Called after the game loop has ended.
		*/
		virtual void end(Core& core) {}

		/**
		* Called if entity collides with another entity
		*/
		virtual void onCollision(Core& core, EntityID hit_id) {}

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
			std::shared_ptr<ENG::Script> script;
		};
	}

	void scriptStart(Entities& entities, Core& core);
	void scriptUpdate(Entities& entities, Core& core);
	void scriptEnd(Entities& entities, Core& core);
}