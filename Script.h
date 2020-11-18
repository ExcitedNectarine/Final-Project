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
		* Calling just before rendering. Any code for movement should be done in update.
		*/
		virtual void lateUpdate(Core& core) {}

		/**
		* Called after the game loop has ended.
		*/
		virtual void end(Core& core) {}

		/**
		* Called if entity enters trigger.
		*/
		virtual void onTriggerEnter(Core& core, EntityID entered_id) {}

		/**
		* Called if entity exits trigger.
		*/
		virtual void onTriggerExit(Core& core, EntityID exited_id) {}

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

	void scriptStart(Core& core);
	void scriptUpdate(Core& core);
	void scriptLateUpdate(Core& core);
	void scriptEnd(Core& core);
}