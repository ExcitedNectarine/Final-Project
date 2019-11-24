#pragma once

#include <memory>
#include <unordered_map>
#include <algorithm>
#include <bitset>

#define MAX_ENTITIES 1024
#define MAX_COMPONENTS 128

// An expander, basically allows an instruction to be ran on every template argument.
// Don't ask me how it works.
using expand = int[];
#define FOR_EACH_T(instruction) (void)expand { 0, (instruction, 0)...}

// Declaring types
using EntityID = uint16_t;
using ComponentID = uint8_t;
using ComponentMask = std::bitset<MAX_COMPONENTS>;
template <typename ComponentType> using ComponentMap = std::unordered_map<EntityID, ComponentType>;

// Template trickery that assigns a unique ID for each type of component.
static ComponentID ids = 0;
struct BaseComponent { static ComponentID getID(); };
template <typename ComponentType> struct ECSComponent : BaseComponent { static const ComponentID ID; };
template <typename ComponentType> const ComponentID ECSComponent<ComponentType>::ID(BaseComponent::getID());

// Store components in seperate pools.
struct BaseComponentPool { virtual void remove(const EntityID id) {} };
template <typename ComponentType> struct ComponentPool : BaseComponentPool
{
	inline void remove(const EntityID id) { pool.erase(id); }
	ComponentMap<ComponentType> pool;
};

/**
* This class can be thought of as a storage class for every entity in the game. Entities
* are represented by a simple integer ID, that is used to reference components
**/
class Entities
{
public:
	EntityID addEntity();
	void removeEntity(const EntityID id);

	template <typename... ComponentType> EntityID addEntity()
	{
		ids++;
		FOR_EACH_T(addComponent<ComponentType>(ids));
		return ids;
	}

	template <typename... RequiredComponent> std::vector<EntityID> entitiesWith()
	{
		ComponentMask mask;
		FOR_EACH_T(mask.flip(RequiredComponent::ID));

		std::vector<EntityID> entities;
		for (auto& pair : has)
			if ((mask & pair.second) == mask)
				entities.push_back(pair.first);

		return entities;
	}

	template <typename ComponentType> void addComponentPool()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");
		table[ComponentType::ID] = std::make_shared<ComponentPool<ComponentType>>();
	}

	template <typename ComponentType> ComponentType& addComponent(const EntityID id)
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");

		has[id][ComponentType::ID] = 1;
		ComponentMap<ComponentType>& pool = getPool<ComponentType>();
		pool.insert({ id, ComponentType() });
		return pool[id];
	}

	template <typename ComponentType> ComponentType removeComponent(const EntityID id)
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");

		has[id][ComponentType::ID] = 0;
		ComponentMap<ComponentType>& pool = getPool<ComponentType>();
		ComponentType c = pool[id];
		pool.erase(id);
		return c;
	}

	template <typename ComponentType> ComponentType& getComponent(const EntityID id)
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");

		ComponentMap<ComponentType>& pool = getPool<ComponentType>();
		return pool[id];
	}

	template <typename ComponentType> inline bool hasComponent(const EntityID id)
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");

		ComponentMap<ComponentType>& pool = getPool<ComponentType>();
		return pool.find(id) != pool.end();
	}

	template <typename ComponentType> inline ComponentMap<ComponentType>& getPool()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "'ComponentType' must inherit 'BaseComponent'.");
		return (*std::static_pointer_cast<ComponentPool<ComponentType>>(table[ComponentType::ID])).pool;
	}

private:
	EntityID ids = 0;
	std::unordered_map<EntityID, ComponentMask> has;
	std::unordered_map<ComponentID, std::shared_ptr<BaseComponentPool>> table;
};