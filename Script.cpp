#include "Script.h"

namespace ENG
{
	void scriptStart(ENG::Entities& entities, Application& app)
	{
		auto& scripts = entities.getPool<CS::Script>();

		for (ENG::EntityID id : entities.entitiesWith<CS::Script>())
		{
			scripts[id].script->id = id;
			scripts[id].script->start(app);
		}
	}

	void scriptUpdate(ENG::Entities& entities, Application& app)
	{
		auto& scripts = entities.getPool<CS::Script>();

		for (ENG::EntityID id : entities.entitiesWith<CS::Script>())
			scripts[id].script->update(app);
	}

	void scriptEnd(ENG::Entities& entities, Application& app)
	{
		auto& scripts = entities.getPool<CS::Script>();

		for (ENG::EntityID id : entities.entitiesWith<CS::Script>())
			scripts[id].script->end(app);
	}
}