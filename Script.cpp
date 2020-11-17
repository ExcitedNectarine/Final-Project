#include "Script.h"
#include "Core.h"

namespace ENG
{
	void scriptStart(Core& core)
	{
		ComponentMap<CS::Script>& scripts = core.entities.getPool<CS::Script>();

		for (ENG::EntityID id : core.entities.entitiesWith<CS::Script>())
		{
			scripts[id].script->id = id;
			scripts[id].script->start(core);
		}
	}

	void scriptUpdate(Core& core)
	{
		ComponentMap<CS::Script>& scripts = core.entities.getPool<CS::Script>();

		for (ENG::EntityID id : core.entities.entitiesWith<CS::Script>())
			scripts[id].script->update(core);
	}

	void scriptLateUpdate(Core& core)
	{
		ComponentMap<CS::Script>& scripts = core.entities.getPool<CS::Script>();

		for (ENG::EntityID id : core.entities.entitiesWith<CS::Script>())
			scripts[id].script->lateUpdate(core);
	}

	void scriptEnd(Core& core)
	{
		ComponentMap<CS::Script>& scripts = core.entities.getPool<CS::Script>();

		for (ENG::EntityID id : core.entities.entitiesWith<CS::Script>())
			scripts[id].script->end(core);
	}
}