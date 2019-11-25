#include "Entities.h"

namespace ENG
{
	ComponentID BaseComponent::getID() { return ids++; }
	EntityID Entities::addEntity() { return ids++; }

	void Entities::removeEntity(const EntityID id)
	{
		for (auto& pair : table)
			pair.second->remove(id);
	}
}