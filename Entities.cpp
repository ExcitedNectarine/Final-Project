#include "Entities.h"

ComponentID BaseComponent::getID() { return ids++; }
EntityID Entities::addEntity() { return ids++; }

void Entities::removeEntity(const EntityID id)
{
	for (auto& pair : table)
		pair.second->remove(id);
}