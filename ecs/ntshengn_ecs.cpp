#include "ntshengn_ecs.h"
#include <stdexcept>

NtshEngn::EntityManager::EntityManager() {
	for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
		m_availableEntities.push_back(entity);
	}
}

NtshEngn::Entity NtshEngn::EntityManager::createEntity() {
	NTSHENGN_ASSERT(m_numberOfEntities < MAX_ENTITIES);

	Entity id = m_availableEntities.front();
	m_availableEntities.pop_front();
	m_numberOfEntities++;

	m_existingEntities.insert(id);

	return id;
}

NtshEngn::Entity NtshEngn::EntityManager::createEntity(const std::string& name) {
	NTSHENGN_ASSERT(!m_entityNames.exist(name));

	Entity id = createEntity();
	m_entityNames.insert_or_assign(id, name);

	return id;
}

void NtshEngn::EntityManager::destroyEntity(Entity entity) {
	NTSHENGN_ASSERT(entity < MAX_ENTITIES);

	m_componentMasks[entity].reset();
	m_availableEntities.push_front(entity);
	m_numberOfEntities--;

	m_existingEntities.erase(entity);

	if (m_entityNames.exist(entity)) {
		m_entityNames.erase(entity);
	}
}

void NtshEngn::EntityManager::setComponents(Entity entity, ComponentMask componentMask) {
	NTSHENGN_ASSERT(entity < MAX_ENTITIES);

	m_componentMasks[entity] = componentMask;
}

NtshEngn::ComponentMask NtshEngn::EntityManager::getComponents(Entity entity) {
	NTSHENGN_ASSERT(entity < MAX_ENTITIES);

	return m_componentMasks[entity];
}

bool NtshEngn::EntityManager::entityHasName(Entity entity) {
	return m_entityNames.exist(entity);
}

void NtshEngn::EntityManager::setEntityName(Entity entity, const std::string& name) {
	NTSHENGN_ASSERT(!m_entityNames.exist(name));

	m_entityNames.insert_or_assign(entity, name);
}

std::string NtshEngn::EntityManager::getEntityName(Entity entity) {
	NTSHENGN_ASSERT(m_entityNames.exist(entity));

	return m_entityNames[entity];
}

NtshEngn::Entity NtshEngn::EntityManager::findEntityByName(const std::string& name) {
	NTSHENGN_ASSERT(m_entityNames.exist(name));

	return m_entityNames[name];
}

const std::set<Entity>& NtshEngn::EntityManager::getExistingEntities() {
	return m_existingEntities;
}

void NtshEngn::ComponentManager::entityDestroyed(Entity entity) {
	for (const auto& pair : m_componentArrays) {
		const std::shared_ptr<IComponentArray>& componentArray = pair.second;
		componentArray->entityDestroyed(entity);
	}
}

void NtshEngn::SystemManager::entityDestroyed(Entity entity, ComponentMask entityComponents) {
	for (const auto& pair : m_systems) {
		const std::string& type = pair.first;
		System* system = pair.second;
		const ComponentMask systemComponentMask = m_componentMasks[type];
		const ComponentMask entityAndSystemComponentMask = entityComponents & systemComponentMask;

		bool entityInSystem = false;
		for (uint8_t i = 0; i < MAX_COMPONENTS; i++) {
			if (entityAndSystemComponentMask[i]) {
				entityInSystem = true;
				system->onEntityComponentRemoved(entity, i);
			}
		}

		if (entityInSystem) {
			system->entities.erase(entity);
		}
	}
}

void NtshEngn::SystemManager::entityComponentMaskChanged(Entity entity, ComponentMask oldEntityComponentMask, ComponentMask newEntityComponentMask, Component componentID) {
	for (const auto& pair : m_systems) {
		const std::string& type = pair.first;
		System* system = pair.second;
		const ComponentMask systemComponentMask = m_componentMasks[type];
		const ComponentMask oldAndSystemComponentMasks = oldEntityComponentMask & systemComponentMask;
		const ComponentMask newAndSystemComponentMasks = newEntityComponentMask & systemComponentMask;
		if (oldAndSystemComponentMasks != newAndSystemComponentMasks) { // A Component used in the system has been added or removed
			if (newAndSystemComponentMasks.to_ulong() > oldAndSystemComponentMasks.to_ulong()) { // A Component has been added
				system->onEntityComponentAdded(entity, componentID);
				if (oldAndSystemComponentMasks.none()) { // The entity is new in the system
					system->entities.insert(entity);
				}
			}
			else if (newAndSystemComponentMasks.to_ulong() < oldAndSystemComponentMasks.to_ulong()) { // A Component has been removed
				system->onEntityComponentRemoved(entity, componentID);
				if (newAndSystemComponentMasks.none()) { // The entity has no more component for the system
					system->entities.erase(entity);
				}
			}
		}
	}
}

void NtshEngn::ECS::init() {
	m_entityManager = std::make_unique<EntityManager>();
	m_componentManager = std::make_unique<ComponentManager>();
	m_systemManager = std::make_unique<SystemManager>();
}

Entity NtshEngn::ECS::createEntity() {
	Entity newEntity = m_entityManager->createEntity();
	addComponent(newEntity, Transform{});
	
	return newEntity;
}

NtshEngn::Entity NtshEngn::ECS::createEntity(const std::string& name) {
	Entity newEntity = m_entityManager->createEntity(name);
	addComponent(newEntity, Transform{});
	
	return newEntity;
}

void NtshEngn::ECS::destroyEntity(Entity entity) {
	ComponentMask entityComponents = m_entityManager->getComponents(entity);
	m_systemManager->entityDestroyed(entity, entityComponents);
	m_entityManager->destroyEntity(entity);
	m_componentManager->entityDestroyed(entity);
}

void NtshEngn::ECS::destroyAllEntities() {
	while (!m_entityManager->getExistingEntities().empty()) {
		destroyEntity(*m_entityManager->getExistingEntities().rbegin());
	}
}

void NtshEngn::ECS::setEntityName(Entity entity, const std::string& name) {
	m_entityManager->setEntityName(entity, name);
}

bool NtshEngn::ECS::entityHasName(Entity entity) {
	return m_entityManager->entityHasName(entity);
}

std::string NtshEngn::ECS::getEntityName(Entity entity) {
	return m_entityManager->getEntityName(entity);
}

NtshEngn::Entity NtshEngn::ECS::findEntityByName(const std::string& name) {
	return m_entityManager->findEntityByName(name);
}