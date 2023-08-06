#pragma once
#include "../utils/ntshengn_defines.h"
#include "../utils/ntshengn_utils_bimap.h"
#include "components/ntshengn_ecs_transform.h"
#include "components/ntshengn_ecs_renderable.h"
#include "components/ntshengn_ecs_camera.h"
#include "components/ntshengn_ecs_light.h"
#include "components/ntshengn_ecs_rigidbody.h"
#include "components/ntshengn_ecs_sphere_collidable.h"
#include "components/ntshengn_ecs_aabb_collidable.h"
#include "components/ntshengn_ecs_capsule_collidable.h"
#include "components/ntshengn_ecs_audio_emitter.h"
#include "components/ntshengn_ecs_audio_listener.h"
#include "components/ntshengn_ecs_scriptable.h"
#include <bitset>
#include <deque>
#include <array>
#include <unordered_map>
#include <memory>
#include <set>
#include <string>

#define MAX_ENTITIES 4096
#define MAX_COMPONENTS 32

namespace NtshEngn {

	using Entity = uint32_t;
	using Component = uint8_t;
	using ComponentMask = std::bitset<MAX_COMPONENTS>;

	class EntityManager {
	public:
		EntityManager();

		Entity createEntity();
		Entity createEntity(const std::string& name);
		void destroyEntity(Entity entity);

		void setComponents(Entity entity, ComponentMask componentMask);
		ComponentMask getComponents(Entity entity);

		bool entityHasName(Entity entity);
		void setEntityName(Entity entity, const std::string& name);
		std::string getEntityName(Entity entity);
		Entity findEntityByName(const std::string& name);

		const std::set<Entity>& getExistingEntities():

	private:
		std::deque<Entity> m_availableEntities;
		std::set<Entity> m_existingEntities;
		std::array<ComponentMask, MAX_ENTITIES> m_componentMasks;
		Bimap<Entity, std::string> m_entityNames;
		uint32_t m_numberOfEntities = 0;
	};

	class IComponentArray {
	public:
		virtual ~IComponentArray() = default;
		virtual void entityDestroyed(Entity entity) = 0;
	};

	template <typename T>
	class ComponentArray : public IComponentArray {
	public:
		void insertData(Entity entity, T component) {
			NTSHENGN_ASSERT(m_entityToIndex.find(entity) == m_entityToIndex.end());

			m_entityToIndex[entity] = m_validSize;
			m_indexToEntity[m_validSize] = entity;
			m_components[m_validSize] = component;
			m_validSize++;
		}

		void removeData(Entity entity) {
			NTSHENGN_ASSERT(m_entityToIndex.find(entity) != m_entityToIndex.end());

			size_t tmp = m_entityToIndex[entity];
			m_components[tmp] = m_components[m_validSize - 1];
			Entity entityLast = m_indexToEntity[m_validSize - 1];
			m_entityToIndex[entityLast] = tmp;
			m_indexToEntity[tmp] = entityLast;
			m_entityToIndex.erase(entity);
			m_indexToEntity.erase(m_validSize - 1);
			m_validSize--;
		}

		bool hasComponent(Entity entity) {
			return m_entityToIndex.find(entity) != m_entityToIndex.end();
		}

		T& getData(Entity entity) {
			NTSHENGN_ASSERT(m_entityToIndex.find(entity) != m_entityToIndex.end());

			return m_components[m_entityToIndex[entity]];
		}

		void entityDestroyed(Entity entity) override {
			if (m_entityToIndex.find(entity) != m_entityToIndex.end()) {
				removeData(entity);
			}
		}

	private:
		std::array<T, MAX_ENTITIES> m_components;
		std::unordered_map<Entity, size_t> m_entityToIndex;
		std::unordered_map<size_t, Entity> m_indexToEntity;
		size_t m_validSize;
	};

	class ComponentManager {
	public:
		template <typename T>
		void registerComponent() {
			std::string typeName = std::string(typeid(T).name());

			NTSHENGN_ASSERT(m_componentTypes.find(typeName) == m_componentTypes.end());

			m_componentTypes.insert({ typeName, m_nextComponent });
			m_componentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });
			m_nextComponent++;
		}

		template <typename T>
		Component getComponentId() {
			std::string typeName = std::string(typeid(T).name());

			NTSHENGN_ASSERT(m_componentTypes.find(typeName) != m_componentTypes.end());

			return m_componentTypes[typeName];
		}

		template <typename T>
		void addComponent(Entity entity, T component) {
			getComponentArray<T>()->insertData(entity, component);
		}

		template <typename T>
		void removeComponent(Entity entity) {
			getComponentArray<T>()->removeData(entity);
		}

		template <typename T>
		bool hasComponent(Entity entity) {
			return getComponentArray<T>()->hasComponent(entity);
		}

		template <typename T>
		T& getComponent(Entity entity) {
			return getComponentArray<T>()->getData(entity);
		}

		void entityDestroyed(Entity entity);

	private:
		template <typename T>
		std::shared_ptr<ComponentArray<T>> getComponentArray() {
			std::string typeName = std::string(typeid(T).name());

			NTSHENGN_ASSERT(m_componentTypes.find(typeName) != m_componentTypes.end());

			return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays[typeName]);
		}

	private:
		std::unordered_map<std::string, Component> m_componentTypes;
		std::unordered_map<std::string, std::shared_ptr<IComponentArray>> m_componentArrays;
		Component m_nextComponent = 0;
	};

	class System {
	public:
		virtual void onEntityComponentAdded(Entity entity, Component componentID) { NTSHENGN_UNUSED(entity); NTSHENGN_UNUSED(componentID); }
		virtual void onEntityComponentRemoved(Entity entity, Component componentID) { NTSHENGN_UNUSED(entity); NTSHENGN_UNUSED(componentID); }
		
	public:
		std::set<Entity> entities;
	};

	class SystemManager {
	public:
		template <typename T>
		void registerSystem(System* system) {
			std::string typeName = std::string(typeid(T).name());

			NTSHENGN_ASSERT(m_systems.find(typeName) == m_systems.end());

			m_systems.insert({ typeName, system });
		}

		template <typename T>
		void setComponents(ComponentMask componentMask) {
			std::string typeName = std::string(typeid(T).name());

			NTSHENGN_ASSERT(m_systems.find(typeName) != m_systems.end());

			m_componentMasks.insert({ typeName, componentMask });
		}

		void entityDestroyed(Entity entity, ComponentMask entityComponents);

		void entityComponentMaskChanged(Entity entity, ComponentMask oldEntityComponentMask, ComponentMask newEntityComponentMask, Component componentID);

	private:
		std::unordered_map<std::string, ComponentMask> m_componentMasks;
		std::unordered_map<std::string, System*> m_systems;
	};

	class ECS {
	public:
		void init();

		// Entity
		Entity createEntity();
		Entity createEntity(const std::string& name);
		void destroyEntity(Entity entity);
		void destroyAllEntities();
		void setEntityName(Entity entity, const std::string& name);
		bool entityHasName(Entity entity);
		std::string getEntityName(Entity entity);
		Entity findEntityByName(const std::string& name);

		// Component
		template <typename T>
		void registerComponent() {
			return m_componentManager->registerComponent<T>();
		}

		template <typename T>
		void addComponent(Entity entity, T component) {
			m_componentManager->addComponent<T>(entity, component);
			ComponentMask oldComponents = m_entityManager->getComponents(entity);
			ComponentMask newComponents = oldComponents;
			Component componentID = m_componentManager->getComponentId<T>();
			newComponents.set(componentID, true);
			m_entityManager->setComponents(entity, newComponents);
			m_systemManager->entityComponentMaskChanged(entity, oldComponents, newComponents, componentID);
		}

		template <typename T>
		void removeComponent(Entity entity) {
			ComponentMask oldComponents = m_entityManager->getComponents(entity);
			ComponentMask newComponents = oldComponents;
			Component componentID = m_componentManager->getComponentId<T>();
			newComponents.set(componentID, false);
			m_entityManager->setComponents(entity, newComponents);
			m_systemManager->entityComponentMaskChanged(entity, oldComponents, newComponents, componentID);
			m_componentManager->removeComponent<T>(entity);
		}

		template <typename T>
		bool hasComponent(Entity entity) {
			return m_componentManager->hasComponent<T>(entity);
		}

		template <typename T>
		T& getComponent(Entity entity) {
			return m_componentManager->getComponent<T>(entity);
		}

		template <typename T>
		Component getComponentId() {
			return m_componentManager->getComponentId<T>();
		}

		// System
		template <typename T>
		void registerSystem(System* system) {
			m_systemManager->registerSystem<T>(system);
		}

		template <typename T>
		void setSystemComponents(ComponentMask componentMask) {
			m_systemManager->setComponents<T>(componentMask);
		}
	private:
		std::unique_ptr<EntityManager> m_entityManager;
		std::unique_ptr<ComponentManager> m_componentManager;
		std::unique_ptr<SystemManager> m_systemManager;
	};

}