#pragma once
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include <utility>

#include "Events.h"

namespace Engine {
    class EventDispatcher
    {
    private:
        std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> m_listeners;

    public:
        template<typename EventType, typename Callable>
        void Subscribe(Callable&& callback)
        {
            std::type_index typeIdx = std::type_index(typeid(EventType));

            m_listeners[typeIdx].push_back(
                [cb = std::forward<Callable>(callback)](const void* rawEvent) {
                    cb(*static_cast<const EventType*>(rawEvent));
                }
            );
        }

        template<typename EventType>
        void Trigger(const EventType& event)
        {
            std::type_index typeIdx = std::type_index(typeid(EventType));

            auto it = m_listeners.find(typeIdx);
            if (it != m_listeners.end())
            {
                size_t count = it->second.size();
                for (size_t i = 0; i < count; ++i) {
                    it->second[i](&event);
                }
            }
        }

        void Clear() {
            m_listeners.clear();
        }
    };
}