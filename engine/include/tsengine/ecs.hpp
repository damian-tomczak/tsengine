#pragma once

#include <cstdint>

namespace ts
{
inline constexpr auto maxComponents = 32;

using Signature = std::bitset<maxComponents>;

struct IComponent
{
protected:
    static uint32_t nextId;
};

template <typename T>
class Component : public IComponent
{
public:
    static uint32_t getId() { return nextId++; }
};

class Entity
{
private:
    uint32_t id;

public:
    explicit Entity(uint32_t id) : id{id} {};

    void kill();
    uint32_t getId() const;

    void tag(const std::string& tag);
    bool hasTag(const std::string& tag) const;
    void group(const std::string& group);
    bool belongsToGroup(const std::string& group) const;

    template <typename TComponent, typename ...TArgs> void addComponent(TArgs&& ...args);
    template <typename TComponent> void removeComponent();
    template <typename TComponent> bool hasComponent() const;
    template <typename TComponent> TComponent& getComponent() const;

    auto operator<=>(const Entity& other) const = default;

    class Registry* registry;
};

class System
{
private:
    std::vector<Entity> entities;
    Signature componentSignature;

public:
    void addEntityToSystem(Entity entity);
    void removeEntityFromSystem(Entity entity);
    std::vector<Entity> getSystemEntities() const;
    const Signature& getComponentSignature() const;

    template <typename TComponent> void requireComponent();
};

class IPool
{
public:
    virtual void RemoveEntityFromPool(uint32_t entityId) = 0;
};

template <typename T>
class Pool : public IPool
{
private:
    std::unordered_map<uint32_t, uint32_t> entityIdToIndex;
    std::unordered_map<uint32_t, uint32_t> indexToEntityId;
    std::vector<T> data;
    uint32_t size{};

public:
    Pool(uint32_t capacity = 100) : data(capacity, {}) {}

    bool isEmpty() const { return size == 0; }
    uint32_t getSize() const { return size; }
    void reset();
    void set(uint32_t entityId, T object);
    void remove(const uint32_t entityId);
    void removeEntityFromPool(const uint32_t entityId) override;
    T& get(uint32_t entityId);

    T& operator [](uint32_t index) { return data[index]; }
    T& operator [](uint32_t index) const { return data[index]; }
};

class Registry
{
private:
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
    void killEntity(Entity entity);

    void tagEntity(Entity entity, const std::string& tag);
    bool entityHasTag(Entity entity, const std::string& tag) const;
    Entity getEntityByTag(const std::string& tag) const;
    void removeEntityTag(Entity entity);

    void groupEntity(Entity entity, const std::string& group);
    bool entityBelongsToGroup(Entity entity, const std::string& group) const;
    std::vector<Entity> getEntitiesByGroup(const std::string& group) const;
    void removeEntityGroup(Entity entity);

    template <typename TComponent, typename ...TArgs> void addComponent(Entity entity, TArgs&& ...args);
    template <typename TComponent> void removeComponent(Entity entity);
    template <typename TComponent> bool hasComponent(Entity entity) const;
    template <typename TComponent> TComponent& getComponent(Entity entity) const;

    template <typename TSystem, typename ...TArgs> void addSystem(TArgs&& ...args);
    template <typename TSystem> void removeSystem();
    template <typename TSystem> bool hasSystem() const;
    template <typename TSystem> TSystem& getSystem() const;

    void addEntityToSystems(Entity entity);
    void removeEntityFromSystems(Entity entity);
};

template<typename T>
void Pool<T>::reset()
{
    data.clear();
    entityIdToIndex.clear();
    indexToEntityId.clear();
    size = 0;
}

template<typename T>
void Pool<T>::set(uint32_t entityId, T object)
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
    data[indexOfRemoved] = data[indexOfLast];

    const auto entityIdOfLastElement = indexToEntityId[indexOfLast];
    entityIdToIndex[entityIdOfLastElement] = indexOfRemoved;
    indexToEntityId[indexOfRemoved] = entityIdOfLastElement;

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

template <typename TComponent>
void System::requireComponent()
{
    const auto componentId = Component<TComponent>::GetId();
    componentSignature.set(componentId);
}

template <typename TSystem, typename ...TArgs>
void Registry::addSystem(TArgs&& ...args)
{
    std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>(std::forward<TArgs>(args)...);
    systems.insert(std::make_pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem>
void Registry::removeSystem()
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    systems.erase(system);
}

template <typename TSystem>
bool Registry::hasSystem() const
{
    return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::getSystem() const
{
    auto system = systems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs>
void Registry::addComponent(Entity entity, TArgs&& ...args)
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();

    if (componentId >= componentPools.size())
    {
        componentPools.resize(componentId + 1, nullptr);
    }

    if (!componentPools[componentId])
    {
        std::shared_ptr<Pool<TComponent>> newComponentPool(new Pool<TComponent>());
        componentPools[componentId] = newComponentPool;
    }

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

    TComponent newComponent(std::forward<TArgs>(args)...);

    componentPool->Set(entityId, newComponent);

    entityComponentSignatures[entityId].set(componentId);
}

template <typename TComponent>
void Registry::removeComponent(Entity entity)
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();

    std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    componentPool->Remove(entityId);

    entityComponentSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
bool Registry::hasComponent(Entity entity) const
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();
    return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent& Registry::getComponent(Entity entity) const
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();
    auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
    return componentPool->Get(entityId);
}

template <typename TComponent, typename ...TArgs>
void Entity::addComponent(TArgs&& ...args)
{
    registry->addComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::removeComponent()
{
    registry->removeComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::hasComponent() const
{
    return registry->hasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::getComponent() const
{
    return registry->getComponent<TComponent>(*this);
}
}
