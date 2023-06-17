#pragma once

#include <algorithm>
#include <any>
#include <bitset>
#include <compare>
#include <cstdint>
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace ts
{
inline constexpr auto maxComponents{ 32 };

using TId = uint64_t;
using TSignature = std::bitset<maxComponents>;
using TPool = std::vector<std::pair<TId, std::any>>;

class Registry;

struct Entity
{
    Entity(TId id_, Registry* pRegistry_);

    TId getId();

    void setTag(const std::string& tag);
    bool hasTag(const std::string& tag) const;

    template <typename TComponent, typename... TArgs>
    void addComponent(TArgs&&... args);

    template <typename TComponent>
    TComponent& getComponent() const;

    auto operator<=>(const Entity& other) const;
    bool operator==(const Entity& other) const;

private:
    TId id;
    // To make the code easier readable
    Registry* pRegistry;
};

struct IComponent
{
protected:
    inline static TId nextId;
};

template <typename T>
class Component : public IComponent
{
public:
    static TId getId();
};

class System
{
public:
    System() = default;

    void addEntityToSystem(Entity entity);
    void removeEntityFromSystem(Entity entity);

    std::vector<Entity> getSystemEntities() const;
    const TSignature& getSignature() const;

    template <typename TComponent>
    void requireComponent();

private:
    TSignature mSignature;
    std::vector<Entity> mEntities;
};

class Registry
{
public:
    Registry() = default;

    Entity createEntity();

    void destroyEntity(Entity entity);

    template <typename TSystem, typename... TArgs>
    void addSystem(TArgs&&... args);

    template <typename TSystem>
    TSystem& getSystem() const;

    template <typename TComponent, typename... TArgs>
    void addComponent(Entity entity, TArgs&&... args);
    template <typename TComponent>
    TComponent& getComponent(Entity entity) const;

    void setEntityTag(Entity entity, const std::string& tag);
    bool hasEntityTag(Entity entity, const std::string& tag) const;

    void update();

    void addEntityToSystems(Entity entity);
    void removeEntityFromSystems(Entity entity);

private:
    uint64_t mNumEntities{};

    std::set<Entity> mEntitiesToBeAdded;
    std::set<Entity> mEntitiesToBeDestroyed;

    std::unordered_map<std::type_index, std::shared_ptr<System>> mSystems;
    std::unordered_map<std::string, Entity> mEntities;
    std::vector<std::unique_ptr<TPool>> mComponentPools;
    std::vector<TSignature> mEntitySignatures;
};

// Entity
inline Entity::Entity(TId id_, Registry* pRegistry_) : id{ id_ }, pRegistry{ pRegistry_ }
{
}

inline TId Entity::getId()
{
    return id;
}

inline void Entity::setTag(const std::string& tag)
{
    pRegistry->setEntityTag(*this, tag);
}

inline bool Entity::hasTag(const std::string& tag) const
{
    return pRegistry->hasEntityTag(*this, tag);
}

template <typename TComponent, typename... TArgs>
inline void Entity::addComponent(TArgs&&... args)
{
    pRegistry->addComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
TComponent& Entity::getComponent() const
{
    return pRegistry->getComponent<TComponent>(*this);
}

inline auto Entity::operator<=>(const Entity& other) const
{
    return id <=> other.id;
}

inline bool Entity::operator==(const Entity& other) const
{
    return id == other.id;
};

// Component
template <typename T>
inline TId Component<T>::getId()
{
    static auto id{ nextId++ };
    return id;
}

// System
inline void System::addEntityToSystem(Entity entity)
{
    mEntities.push_back(entity);
}

inline void System::removeEntityFromSystem(Entity entity)
{
    std::erase_if(mEntities, [&entity](Entity other) { return entity == other; });
}

inline std::vector<Entity> System::getSystemEntities() const
{
    return mEntities;
}

inline const TSignature& System::getSignature() const
{
    return mSignature;
}

template <typename TComponent>
void System::requireComponent()
{
    const auto componentId = Component<TComponent>::getId();
    mSignature.set(componentId);
}

// Registry
inline void Registry::update()
{
    for (auto entity : mEntitiesToBeAdded)
    {
        addEntityToSystems(entity);
    }
    mEntitiesToBeAdded.clear();

    for (auto entity : mEntitiesToBeDestroyed)
    {
        removeEntityFromSystems(entity);
        mEntitySignatures.at(entity.getId()).reset();

        for (auto& pPool : mComponentPools)
        {
            if (pPool != nullptr)
            {
                std::erase_if(*pPool,
                              [&entity](const std::pair<TId, std::any>& p) { return p.first == entity.getId(); });
            }
        }
    }
    mEntitiesToBeDestroyed.clear();
}

inline Entity Registry::createEntity()
{
    auto entityId{ mNumEntities++ };
    if (entityId >= mEntitySignatures.size())
    {
        mEntitySignatures.resize(entityId + 1);
    }
    Entity entity{ entityId, this };
    mEntitiesToBeAdded.insert(entity);
    return entity;
}

inline void Registry::destroyEntity(Entity entity)
{
    mEntitiesToBeDestroyed.insert(entity);
}

template <typename TSystem, typename... TArgs>
void Registry::addSystem(TArgs&&... args)
{
    mSystems.emplace(std::type_index(typeid(TSystem)), std::make_shared<TSystem>(std::forward<TArgs>(args)...));
}

template <typename TSystem>
TSystem& Registry::getSystem() const
{
    auto system = mSystems.find(std::type_index(typeid(TSystem)));
    return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename... TArgs>
inline void Registry::addComponent(Entity entity, TArgs&&... args)
{
    const auto componentId{ Component<TComponent>::getId() };
    const auto entityId{ entity.getId() };

    if (componentId >= mComponentPools.size())
    {
        mComponentPools.emplace_back(std::make_unique<TPool>());
    }
    mComponentPools.at(componentId)->emplace_back(entityId, TComponent{ std::forward<TArgs>(args)... });
    mEntitySignatures.at(entityId).set(componentId);
}

inline void Registry::addEntityToSystems(Entity entity)
{
    const auto& entitySignature = mEntitySignatures[entity.getId()];

    for (auto& system : mSystems)
    {
        const auto& systemSignature = system.second->getSignature();

        if ((entitySignature & systemSignature) == systemSignature)
        {
            system.second->addEntityToSystem(entity);
        }
    }
}

inline void Registry::removeEntityFromSystems(Entity entity)
{
    for (auto system : mSystems)
    {
        system.second->removeEntityFromSystem(entity);
    }
}

template <typename TComponent>
TComponent& Registry::getComponent(Entity entity) const
{
    const auto componentId = Component<TComponent>::getId();
    const auto entityId = entity.getId();
    auto& componentPool = mComponentPools.at(componentId);

    auto it{ std::ranges::find_if(*componentPool, [&entityId](const auto& pair) { return pair.first == entityId; }) };

    return std::any_cast<TComponent&>(it->second);
}

inline void Registry::setEntityTag(Entity entity, const std::string& tag)
{
    mEntities.emplace(tag, entity);
}

inline bool Registry::hasEntityTag(Entity entity, const std::string& tag) const
{
    (void)entity;

    if (mEntities.find(tag) != mEntities.end())
    {
        return true;
    }

    return false;
}
} // namespace ts