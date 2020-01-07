#include "Entities.h"

namespace ENG
{
	ComponentID BaseComponent::getID() { return ids++; }
	EntityID Entities::addEntity() { return ids++; }

	void Entities::removeEntity(const EntityID id)
	{
		//assert(has.find(id) != has.end());

		has.erase(id);
		for (auto& pair : table)
			pair.second->remove(id);
	}

	void Entities::clear()
	{
		has.clear();
		table.clear();
	}
}