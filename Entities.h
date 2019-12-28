#pragma once

#include <memory>
#include <unordered_map>
#include <algorithm>
#include <bitset>
#include <cassert>
#include "Output.h"

namespace ENG
{
	constexpr auto MAX_ENTITIES = 1024;
	constexpr auto MAX_COMPONENTS = 128;

	// An expander, basically allows an instruction to be ran on every template argument.
	// Don't ask me how it works.
	using expand = int[];
	#define FOR_EACH_T(instruction) (void)expand { 0, (instruction, 0)...}
	#define BASE_ASSERT(base, type, text) static_assert(std::is_base_of<base, type>::value, text)

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
		void clear();

		template <typename... ComponentType> EntityID addEntity()
		{
			ids++;
			assert(ids <= MAX_ENTITIES);
			FOR_EACH_T(addComponent<ComponentType>(ids));
			return ids;
		}

		template <typename... RequiredComponent> std::vector<EntityID> entitiesWith()
		{
			ComponentMask mask;
			FOR_EACH_T(mask.flip(RequiredComponent::ID));

			std::vector<EntityID> entities;
			for (auto& pair : has)
				if ((mask & pair.second) == mask) // Fast bitwise AND determines if an entity has the required components
					entities.push_back(pair.first);

			return entities;
		}

		template <typename... ComponentType> void addComponentPools() { FOR_EACH_T(addComponentPool<ComponentType>()); }
		template <typename ComponentType> void addComponentPool()
		{
			BASE_ASSERT(BaseComponent, ComponentType, "'ComponentType' must inherit 'BaseComponent'.");
			OUTPUT("Adding component pool '" + std::string(typeid(ComponentType).name()) + "'");
			table[ComponentType::ID] = std::make_shared<ComponentPool<ComponentType>>();
		}

		template <typename ComponentType> ComponentType& addComponent(const EntityID id)
		{
			BASE_ASSERT(BaseComponent, ComponentType, "'ComponentType' must inherit 'BaseComponent'.");
			OUTPUT("Adding component '" + std::string(typeid(ComponentType).name()) + "' to entity ID " + std::to_string(id));
			has[id][ComponentType::ID] = 1;
			ComponentMap<ComponentType>& pool = getPool<ComponentType>();
			pool.insert({ id, ComponentType() });
			return pool.at(id);
		}

		template <typename ComponentType> ComponentType removeComponent(const EntityID id)
		{
			BASE_ASSERT(BaseComponent, ComponentType, "'ComponentType' must inherit 'BaseComponent'.");
			has[id][ComponentType::ID] = 0;
			ComponentMap<ComponentType>& pool = getPool<ComponentType>();
			ComponentType c = pool.at(id);
			pool.erase(id);
			return c;
		}

		template <typename ComponentType> ComponentType& getComponent(const EntityID id)
		{
			BASE_ASSERT(BaseComponent, ComponentType, "'ComponentType' must inherit 'BaseComponent'.");
			ComponentMap<ComponentType>& pool = getPool<ComponentType>();
			return pool.at(id);
		}

		template <typename ComponentType> inline bool hasComponent(const EntityID id)
		{
			BASE_ASSERT(BaseComponent, ComponentType, "'ComponentType' must inherit 'BaseComponent'.");
			return has[id][ComponentType::ID];
		}

		template <typename ComponentType> inline ComponentMap<ComponentType>& getPool()
		{
			BASE_ASSERT(BaseComponent, ComponentType, "'ComponentType' must inherit 'BaseComponent'.");
			return (*std::static_pointer_cast<ComponentPool<ComponentType>>(table[ComponentType::ID])).pool;
		}

	private:
		EntityID ids = 0;
		std::unordered_map<EntityID, ComponentMask> has;
		std::unordered_map<ComponentID, std::shared_ptr<BaseComponentPool>> table;
	};
}