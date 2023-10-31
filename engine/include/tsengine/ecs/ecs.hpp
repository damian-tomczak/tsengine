#pragma once

#include "tsengine/utils.hpp"

#include <cstdint>
#include <bitset>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <deque>
#include <memory>
#include <typeindex>

namespace ts
{
inline constexpr auto maxComponents = 32;

using Signature = std::bitset<maxComponents>;

struct IComponent
{
protected:
    inline static uint32_t nextId{};
};

template <typename T>
class Component : public IComponent
{
public:
    static uint32_t getId() { return nextId++; }
};

class Entity
{
    uint32_t id;
    class Registry* mpRegistry;

public:
    Entity(const uint32_t id, Registry* const registry = nullptr) : id{id}, mpRegistry{registry} {}

    uint32_t getId() const { return id; }
    void kill();

    void tag(const std::string& tag);
    bool hasTag(const std::string& tag) const;
    void group(const std::string& group);
    bool belongsToGroup(const std::string& group) const;

    template <typename TComponent, typename ...TArgs> void addComponent(TArgs&& ...args);
    template <typename TComponent> void removeComponent();
    template <typename TComponent> bool hasComponent() const;
    template <typename TComponent> TComponent& getComponent() const;

    auto operator<=>(const Entity& other) const = default;
};

class System
{
private:
    std::vector<Entity> entities;
    Signature componentSignature;

public:
    void addEntityToSystem(const Entity entity) { entities.push_back(entity); }
    void removeEntityFromSystem(const Entity entity);
    std::vector<Entity> getSystemEntities() const { return entities; }
    const Signature& getComponentSignature() const { return componentSignature; }

    template <typename TComponent> void requireComponent();
};

class IPool
{
public:
    virtual void removeEntityFromPool(const uint32_t entityId) = 0;
};

template <typename T>
class Pool : public IPool
{
    std::unordered_map<uint32_t, uint32_t> entityIdToIndex;
    std::unordered_map<uint32_t, uint32_t> indexToEntityId;
    std::vector<T> data;
    uint32_t size{};

public:
    Pool(uint32_t capacity = 100) : data(capacity, T{}) {}

    bool isEmpty() const { return size == 0; }
    uint32_t getSize() const { return size; }
    void reset();
    void set(const uint32_t entityId, const T object);
    void remove(const uint32_t entityId);
    void removeEntityFromPool(const uint32_t entityId) override;
    T& get(const uint32_t entityId);

    T& operator [](const uint32_t index) { return data.at(index); }
    const T& operator [](const uint32_t index) const { return data.at(index); }
};

inline class Registry
{
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems;
    std::unordered_map<std::string, Entity> entityPerTag;
    std::unordered_map<uint32_t, std::string> tagPerEntity;
    std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
    std::unordered_map<uint32_t, std::string> groupPerEntity;
    std::set<Entity> entitiesToBeAdded;
    std::set<Entity> entitiesToBeKilled;
    std::vector<std::shared_ptr<IPool>> componentPools;
    std::vector<Signature> entityComponentSignatures;
    std::deque<uint32_t> freeIds;
    uint32_t numEntities{};

public:
    Registry() = default;

    void update();

    Entity createEntity();
    void killEntity(const Entity entity) { entitiesToBeKilled.insert(entity); };

    void tagEntity(const Entity entity, const std::string& tag);
    bool entityHasTag(const Entity entity, const std::string& tag) const;
    Entity getEntityByTag(const std::string& tag) const { return entityPerTag.at(tag); }
    void removeEntityTag(const Entity entity);

    void groupEntity(const Entity entity, const std::string& group);
    bool entityBelongsToGroup(const Entity entity, const std::string& group) const;
    std::vector<Entity> getEntitiesByGroup(const std::string& group) const;
    void removeEntityGroup(const Entity entity);

    template<typename TComponent, typename ...TArgs> void addComponent(const Entity entity, TArgs&& ...args);
    template<typename TComponent> void removeComponent(const Entity entity);
    template<typename TComponent> bool hasComponent(const Entity entity) const;
    template<typename TComponent> TComponent& getComponent(const Entity entity) const;

    template<typename TSystem, typename ...TArgs> void addSystem(TArgs&& ...args);
    template<typename TSystem> void removeSystem();
    template<typename TSystem> bool hasSystem() const;
    template<typename TSystem> TSystem& getSystem() const;

    void addEntityToSystems(const Entity entity);
    void removeEntityFromSystems(const Entity entity);
} gRegistry;

// Entity

inline void Entity::kill()
{
    mpRegistry->killEntity(*this);
}

inline void Entity::tag(const std::string& tag)
{
    mpRegistry->tagEntity(*this, tag);
}

inline bool Entity::hasTag(const std::string& tag) const
{
    return mpRegistry->entityHasTag(*this, tag);
}

inline void Entity::group(const std::string& group)
{
    mpRegistry->groupEntity(*this, group);
}

inline bool Entity::belongsToGroup(const std::string& group) const
{
    return mpRegistry->entityBelongsToGroup(*this, group);
}

template<typename TComponent, typename ...TArgs>
void Entity::addComponent(TArgs&& ...args)
{
    mpRegistry->addComponent<TComponent>(*this, std::forward<TArgs>(args)...);
};

template<typename TComponent>
void Entity::removeComponent()
{
    mpRegistry->removeComponent<TComponent>(*this);
};

template<typename TComponent>
bool Entity::hasComponent() const
{
    return mpRegistry->hasComponent<TComponent>(*this);
}

template<typename TComponent> TComponent& Entity::getComponent() const
{
    return mpRegistry->getComponent<TComponent>(*this);
}

// Pool

template<typename T>
void Pool<T>::reset()
{
    data.clear();
    entityIdToIndex.clear();
    indexToEntityId.clear();
    size = 0;
}

template<typename T>
void Pool<T>::set(const uint32_t entityId, const T object)
{
    if (entityIdToIndex.find(entityId) != entityIdToIndex.end())
    {
        const auto index = entityIdToIndex[entityId];
        data.at(index) = object;
    }
    else
    {
        const auto index = size;
        entityIdToIndex.emplace(entityId, index);
        indexToEntityId.emplace(index, entityId);
        if (index >= data.capacity())
        {
            data.resize(size * 2);
        }
        data.at(index) = object;
        size++;
    }
}

template<typename T>
void Pool<T>::remove(const uint32_t entityId)
{
    const auto indexOfRemoved = entityIdToIndex[entityId];
    const auto indexOfLast = size - 1;
    data.at(indexOfRemoved) = data[indexOfLast];

    const auto entityIdOfLastElement = indexToEntityId[indexOfLast];
    entityIdToIndex.at(entityIdOfLastElement) = indexOfRemoved;
    indexToEntityId.at(indexOfRemoved) = entityIdOfLastElement;

    entityIdToIndex.erase(entityId);
    indexToEntityId.erase(indexOfLast);

    size--;
}

template<typename T>
void Pool<T>::removeEntityFromPool(const uint32_t entityId)
{
    if (entityIdToIndex.find(entityId) != entityIdToIndex.cend())
    {
        remove(entityId);
    }
}
template<typename T>
T& Pool<T>::get(const uint32_t entityId)
{
    const auto index = entityIdToIndex[entityId];
    return static_cast<T&>(data[index]);
}

// System

inline void System::removeEntityFromSystem(const Entity entity)
{
    entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](const Entity other) {
        return entity == other;
    }), entities.end());
}

template<typename TComponent>
void System::requireComponent()
{
    const auto componentId = Component<TComponent>::getId();
    componentSignature.set(componentId);
}

// Registry

inline void Registry::update()
{
    for (const auto entity : entitiesToBeAdded)
    {
        addEntityToSystems(entity);
    }
    entitiesToBeAdded.clear();

    for (const auto entity : entitiesToBeKilled)
    {
        removeEntityFromSystems(entity);
        entityComponentSignatures[entity.getId()].reset();

        for (const auto pool : componentPools)
        {
            if (pool)
            {
                pool->removeEntityFromPool(entity.getId());
            }
        }

        freeIds.push_back(entity.getId());

        removeEntityTag(entity);
        removeEntityGroup(entity);
    }

    entitiesToBeKilled.clear();
}


inline Entity Registry::createEntity()
{
    uint32_t entityId;

    if (freeIds.empty())
    {
        entityId = numEntities++;
        if (entityId >= entityComponentSignatures.size())
        {
            entityComponentSignatures.resize(entityId + 1);
        }
    }
    else
    {
        entityId = freeIds.front();
        freeIds.pop_front();
    }

    const auto [entity, isQueued] = entitiesToBeAdded.emplace(entityId, nullptr);

    if (!isQueued)
    {
        throw Exception{"Entity couldn't be added to the queue of entities to be added."};
    }

    return *entity;
}

inline void Registry::tagEntity(const Entity entity, const std::string& tag)
{
    entityPerTag.emplace(tag, entity);
    tagPerEntity.emplace(entity.getId(), tag);
}

inline bool Registry::entityHasTag(const Entity entity, const std::string& tag) const
{
    if (tagPerEntity.find(entity.getId()) == tagPerEntity.end())
    {
        return false;
    }

    return entityPerTag.find(tag)->second == entity;
}

inline void Registry::removeEntityTag(const Entity entity)
{
    const auto taggedEntity = tagPerEntity.find(entity.getId());

    if (taggedEntity != tagPerEntity.end())
    {
        auto tag = taggedEntity->second;
        entityPerTag.erase(tag);
        tagPerEntity.erase(taggedEntity);
    }
}

inline void Registry::groupEntity(Entity entity, const std::string& group)
{
    entitiesPerGroup.emplace(group, std::set<Entity>());
    entitiesPerGroup[group].emplace(entity);
    groupPerEntity.emplace(entity.getId(), group);
}

inline bool Registry::entityBelongsToGroup(const Entity entity, const std::string& group) const
{
    if (entitiesPerGroup.find(group) == entitiesPerGroup.end())
    {
        return false;
    }

    const auto groupEntities = entitiesPerGroup.at(group);
    return groupEntities.find(entity.getId()) != groupEntities.end();
}

inline std::vector<Entity> Registry::getEntitiesByGroup(const std::string& group) const
{
    const auto& setOfEntities = entitiesPerGroup.at(group);
    return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
}

inline void Registry::removeEntityGroup(const Entity entity)
{
    const auto groupedEntity = groupPerEntity.find(entity.getId());

    if (groupedEntity != groupPerEntity.end())
    {
        const auto group = entitiesPerGroup.find(groupedEntity->second);

        if (group != entitiesPerGroup.end())
        {
            auto entityInGroup = group->second.find(entity);
            if (entityInGroup != group->second.end())
            {
                group->second.erase(entityInGroup);
            }
        }

        groupPerEntity.erase(groupedEntity);
    }
}

template<typename TSystem, typename ...TArgs>
void Registry::addSystem(TArgs&& ...args)
{
    const auto newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template<typename TSystem>
void Registry::removeSystem()
{
    const auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template<typename TSystem>
bool Registry::hasSystem() const
{
    const auto system = systems.find(std::type_index(typeid(TSystem)));
    return system != systems.end();
}

template<typename TSystem>
TSystem& Registry::getSystem() const
{
    const auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template<typename TComponent, typename ...TArgs>
void Registry::addComponent(const Entity entity, TArgs&& ...args)
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();

    if (componentId >= componentPools.size())
    {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools.at(componentId))
    {
        const auto newComponentPool = std::make_shared<Pool<TComponent>>();
        componentPools.at(componentId) = newComponentPool;
    }

    const auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    TComponent newComponent{std::forward<TArgs>(args)...};

    componentPool->set(entityId, newComponent);

    entityComponentSignatures[entityId].set(componentId);
}

template<typename TComponent>
void Registry::removeComponent(const Entity entity)
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();

    const auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    componentPool->remove(entityId);

    entityComponentSignatures.at(entityId).set(componentId, false);
}

template<typename TComponent>
bool Registry::hasComponent(const Entity entity) const
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();
    return entityComponentSignatures.at(entityId).test(componentId);
}

template<typename TComponent>
TComponent& Registry::getComponent(const Entity entity) const
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();
    const auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return componentPool->get(entityId);
}

inline void Registry::addEntityToSystems(const Entity entity)
{
    const auto entityId = entity.getId();

    const auto& entityComponentSignature = entityComponentSignatures[entityId];

    for (auto& system : systems)
    {
        const auto& systemComponentSignature = system.second->getComponentSignature();

        bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

        if (isInterested)
        {
            system.second->addEntityToSystem(entity);
        }
    }
}

inline void Registry::removeEntityFromSystems(const Entity entity)
{
    for (auto system : systems)
    {
        system.second->removeEntityFromSystem(entity);
    }
}
}
