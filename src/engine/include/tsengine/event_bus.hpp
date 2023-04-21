#pragma once

#include <functional>
#include <list>
#include <map>
#include <memory>

namespace ts
{
class IEventCallback;

using THandlerList = std::list<std::unique_ptr<IEventCallback>>;

struct Event
{
};

class IEventCallback
{
public:
    virtual ~IEventCallback() = default;

    void execute(Event& e)
    {
        call(e);
    }

private:
    virtual void call(Event& e) = 0;
};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback
{
    using TCallback = void (TOwner::*)(TEvent&);

public:
    EventCallback(TOwner* pOwner, TCallback callback) : mpOwner{ pOwner }, mCallback{ callback }
    {
    }

    virtual ~EventCallback() override = default;

private:
    TOwner* mpOwner;
    TCallback mCallback;

    virtual void call(Event& e) override
    {
        std::invoke(mCallback, mpOwner, static_cast<TEvent&>(e));
    }
};

class EventBus
{
public:
    EventBus() : mSubscribers{}
    {
    }

    template <typename TEvent, typename TOwner>
    void subscribeToEvent(TOwner* pOwner, void (TOwner::*callback)(TEvent&))
    {
        if(mSubscribers[typeid(TEvent)] == nullptr)
        {
            mSubscribers[typeid(TEvent)] = std::make_unique<THandlerList>();
        }
        mSubscribers.at(typeid(TEvent))
          ->emplace_back(std::make_unique<EventCallback<TOwner, TEvent>>(pOwner, callback));
    }

    template <typename TEvent, typename... TArgs>
    void emitEvent(TArgs&&... args)
    {
        auto handlers{ mSubscribers.at(typeid(TEvent)).get() };
        if(handlers)
        {
            for(auto it{ handlers->begin() }; it != handlers->end(); ++it)
            {
                auto handler{ it->get() };
                TEvent event(std::forward<TArgs>(args)...);
                handler->execute(event);
            }
        }
    }

private:
    std::map<std::type_index, std::unique_ptr<THandlerList>> mSubscribers;
};
} // namespace ts
