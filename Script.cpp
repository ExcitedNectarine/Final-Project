#include "Script.h"

namespace ENG
{
	void scriptStart(ENG::Entities& entities, Core& core)
	{
		auto& scripts = entities.getPool<CS::Script>();

		for (ENG::EntityID id : entities.entitiesWith<CS::Script>())
		{
			scripts[id].script->id = id;
			scripts[id].script->start(core);
		}
	}

	void scriptUpdate(ENG::Entities& entities, Core& core)
	{
		auto& scripts = entities.getPool<CS::Script>();

		for (ENG::EntityID id : entities.entitiesWith<CS::Script>())
			scripts[id].script->update(core);
	}

	void scriptEnd(ENG::Entities& entities, Core& core)
	{
		auto& scripts = entities.getPool<CS::Script>();

		for (ENG::EntityID id : entities.entitiesWith<CS::Script>())
			scripts[id].script->end(core);
	}
}