#pragma once
#include <unordered_map>
#include <vector>
#include <functional>
#include <typeindex>
#include <utility>

#include "Events.h"

namespace Engine {

    // =========================================================================
    //  EventDispatcher (Диспетчер подій)
    //  Реалізує шину подій (Event Bus). Дозволяє передавати об'єкти подій до
    //  зареєстрованих слухачів без жорсткої зв'язності (decoupling).
    // =========================================================================
    class EventDispatcher
    {
    private:
        // Хеш-карта слухачів. Ключ — унікальний індекс типу події (std::type_index).
        // Значення — вектор функціональних обгорток, які стирають конкретний тип події до `const void*`.
        std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> m_listeners;

    public:
        /// Реєструє обробник події (Callable) для конкретного типу події EventType.
        /// Підтримує звичайні функції, лямбди та std::bind.
        template<typename EventType, typename Callable>
        void Subscribe(Callable&& callback)
        {
            std::type_index typeIdx = std::type_index(typeid(EventType));

            // Захоплюємо обробник через універсальне посилання (std::forward) для оптимізації rvalue/lvalue
            m_listeners[typeIdx].push_back(
                [cb = std::forward<Callable>(callback)](const void* rawEvent) {
                    // Відновлюємо оригінальний тип події із сирого вказівника та викликаємо користувацький обробник
                    cb(*static_cast<const EventType*>(rawEvent));
                }
            );
        }

        /// Миттєво сповіщає всіх слухачів, зареєстрованих на тип події EventType.
        template<typename EventType>
        void Trigger(const EventType& event)
        {
            std::type_index typeIdx = std::type_index(typeid(EventType));

            auto it = m_listeners.find(typeIdx);
            if (it != m_listeners.end())
            {
                // Ітерація за індексом замість ітераторів захищає від інвалідації пам'яті,
                // якщо під час виконання обробника буде викликано Subscribe для цього ж типу події.
                size_t count = it->second.size();
                for (size_t i = 0; i < count; ++i) {
                    // Передаємо адресу об'єкта події як сирий вказівник
                    it->second[i](&event);
                }
            }
        }

        /// Повністю очищає карту слухачів подій, анулюючи всі підписки.
        void Clear() {
            m_listeners.clear();
        }
    };
}