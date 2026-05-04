#pragma once
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>

namespace Engine {
    class EventDispatcher
    {
    private:
        // Мапа: Тип події -> Список функцій-обробників.
        // std::function приймає 'const void*', що дозволяє зберігати різнотипні колбеки в одному векторі.
        // Це техніка Type Erasure (стирання типу).
        std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> m_listeners;

    public:
        // Реєстрація слухача на подію типу EventType.
        // Використання: dispatcher.Subscribe<MyEvent>([](const MyEvent& e) { ... });
        template<typename EventType>
        void Subscribe(std::function<void(const EventType&)> callback)
        {
            std::type_index typeIdx = std::type_index(typeid(EventType));

            // Створюємо лямбду-обгортку (thunk), яка адаптує інтерфейс void* до конкретного EventType.
            // Ми захоплюємо оригінальний 'callback' за значенням.
            auto wrapper = [callback](const void* rawEvent) {
                // static_cast тут безпечний, тому що ми гарантуємо відповідність типів
                // завдяки ключу typeIdx у мапі. Це швидше за dynamic_cast.
                const EventType* event = static_cast<const EventType*>(rawEvent);
                callback(*event);
                };

            m_listeners[typeIdx].push_back(wrapper);
        }

        // Миттєвий запуск події. Усі підписники будуть викликані синхронно.
        template<typename EventType>
        void Trigger(const EventType& event)
        {
            std::type_index typeIdx = std::type_index(typeid(EventType));

            // Перевіряємо, чи є підписники саме для цього типу події
            auto it = m_listeners.find(typeIdx);
            if (it != m_listeners.end())
            {
                // Проходимо по всіх слухачах і передаємо адресу події як void*
                for (auto& listener : it->second) {
                    listener(&event);
                }
            }
        }

        // Очищення всіх підписників (наприклад, при перезавантаженні рівня)
        void Clear() {
            m_listeners.clear();
        }
    };
}