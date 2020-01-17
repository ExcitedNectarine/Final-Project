#pragma once

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

		EntityID id;
	};
}