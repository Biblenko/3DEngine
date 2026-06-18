#pragma once
#include "SparseSet.h"

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>

namespace Engine
{
    // =========================================================================
    //  Registry (Реєстр)
    //  Володіє всіма пулами компонентів та керує життєвим циклом сутностей.
    // =========================================================================
    class Registry
    {
        // unique_ptr: Реєстр є єдиним власником пулів — спільне володіння (shared) не потрібне
        using PoolMap = std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>>;

        EntityID              m_nextEntityId = 0;   // Ідентифікатор для генерації наступної нової сутності
        std::vector<EntityID> m_freeEntities;       // Список раніше видалених ID для їхнього повторного використання
        PoolMap               m_componentSets;      // Хеш-карта пулів компонентів, де ключ — тип компонента

    public:
        // ──────────────────────────────────────────────────────────────────
        //  Життєвий цикл сутностей
        // ──────────────────────────────────────────────────────────────────

        /// Створює нову сутність. Повертає раніше видалений ID, якщо такий є, інакше генерує новий.
        EntityID CreateEntity()
        {
            if (!m_freeEntities.empty())
            {
                EntityID id = m_freeEntities.back();
                m_freeEntities.pop_back();
                return id;
            }
            return m_nextEntityId++;
        }

        /// Повністю видаляє сутність з системи, очищаючи її компоненти в усіх пулах та резервуючи ID для повторного використання.
        void DestroyEntity(EntityID entity)
        {
            for (auto& [type, pool] : m_componentSets)
                pool->Remove(entity);

            m_freeEntities.push_back(entity);
        }

        /// Скидає стан реєстру: очищає всі пули компонентів, обнуляє лічильник ID та список вільних сутностей.
        void Clear()
        {
            for (auto& [type, pool] : m_componentSets)
                pool->Clear();

            m_nextEntityId = 0;
            m_freeEntities.clear();
        }

        // ──────────────────────────────────────────────────────────────────
        //  Доступ до пулів (внутрішні методи)
        //
        //  GetPool()    — створює пул, якщо він відсутній (для операцій запису)
        //  TryGetPool() — повертає nullptr, якщо пулу немає (для операцій читання)
        // ──────────────────────────────────────────────────────────────────
    private:
        template<typename T>
        SparseSet<T>& GetPool()
        {
            const std::type_index typeIdx(typeid(T));
            auto it = m_componentSets.find(typeIdx);

            if (it == m_componentSets.end())
            {
                auto [inserted, _] = m_componentSets.emplace(
                    typeIdx, std::make_unique<SparseSet<T>>());
                return static_cast<SparseSet<T>&>(*inserted->second);
            }

            return static_cast<SparseSet<T>&>(*it->second);
        }

        template<typename T>
        SparseSet<T>* TryGetPool() const
        {
            const std::type_index typeIdx(typeid(T));
            auto it = m_componentSets.find(typeIdx);
            if (it == m_componentSets.end()) return nullptr;
            return static_cast<SparseSet<T>*>(it->second.get());
        }

        // ──────────────────────────────────────────────────────────────────
    public:
        // ──────────────────────────────────────────────────────────────────
        //  Операції з компонентами
        // ──────────────────────────────────────────────────────────────────

        /// Додає або замінює компонент типу T для сутності, конструюючи його на місці (inplace).
        template<typename T, typename... Args>
        T& EmplaceComponent(EntityID entity, Args&&... args)
        {
            return GetPool<T>().Emplace(entity, std::forward<Args>(args)...);
        }

        /// Видаляє компонент типу T із сутності. Повертає true, якщо компонент існував і був видалений.
        template<typename T>
        bool RemoveComponent(EntityID entity)
        {
            auto* pool = TryGetPool<T>();
            return pool && pool->Remove(entity);
        }

        /// Повертає вказівник на компонент типу T для сутності, або nullptr, якщо компонент відсутній.
        template<typename T>
        [[nodiscard]] T* GetComponent(EntityID entity)
        {
            auto* pool = TryGetPool<T>();
            return pool ? pool->Get(entity) : nullptr;
        }

        /// Константна версія GetComponent для безпечного читання компонентів без модифікації.
        template<typename T>
        [[nodiscard]] const T* GetComponent(EntityID entity) const
        {
            auto* pool = TryGetPool<T>();
            return pool ? pool->Get(entity) : nullptr;
        }

        /// Перевіряє, чи володіє сутність компонентом типу T.
        template<typename T>
        [[nodiscard]] bool HasComponent(EntityID entity) const
        {
            auto* pool = TryGetPool<T>();
            return pool && pool->Has(entity);
        }

        // ──────────────────────────────────────────────────────────────────
        //  Однотипні представлення (Views) — прямий доступ до масивів без накладних витрат
        // ──────────────────────────────────────────────────────────────────

        /// Повертає std::span (неперервний масив) для швидкої ітерації по всіх компонентах типу T.
        template<typename T>
        [[nodiscard]] std::span<T> Components()
        {
            return GetPool<T>().Components();
        }

        /// Константна версія для отримання неперервного масиву компонентів типу T.
        template<typename T>
        [[nodiscard]] std::span<const T> Components() const
        {
            auto* pool = TryGetPool<T>();
            return pool ? pool->Components() : std::span<const T>{};
        }

        /// Повертає список ID сутностей, який повністю паралельний масиву, отриманому з Components<T>().
        template<typename T>
        [[nodiscard]] const std::vector<EntityID>& Entities() const
        {
            static const std::vector<EntityID> empty;
            auto* pool = TryGetPool<T>();
            return pool ? pool->Entities() : empty;
        }

        // ──────────────────────────────────────────────────────────────────
        //  Мультикомпонентна ітерація: Each<T1, T2, ...>(func)
        // ──────────────────────────────────────────────────────────────────
        template<typename T1, typename... Tn, typename Func>
        void Each(Func&& func)
        {
            SparseSet<T1>& pool1 = GetPool<T1>();

            if constexpr (sizeof...(Tn) == 0)
            {
                // Швидкий шлях: лише один компонент, додаткові перевірки Has() не потрібні
                for (auto [entity, comp] : pool1)
                {
                    if constexpr (std::is_invocable_v<Func, EntityID, T1&>)
                        func(entity, comp);
                    else
                        func(comp);
                }
            }
            else
            {
                // Кешуємо вказівники на всі другорядні пули компонентів один раз перед початком циклу
                auto otherPools = std::make_tuple(TryGetPool<Tn>()...);

                // Якщо бодай один із необхідних пулів взагалі відсутній у реєстрі — ітерація не має сенсу
                bool allExist = std::apply([](auto*... pools) { return ((pools != nullptr) && ...); }, otherPools);
                if (!allExist) return;

                // Основний цикл ітерації по головному пулу (pool1)
                for (auto [entity, comp1] : pool1)
                {
                    // Отримуємо вказівники на другорядні компоненти за допомогою одного пошуку на пул
                    auto componentPtrs = std::make_tuple(std::get<SparseSet<Tn>*>(otherPools)->Get(entity)...);

                    // Перевіряємо, чи поточна сутність містить абсолютно всі запитувані компоненти
                    bool hasAll = std::apply([](auto*... ptrs) { return ((ptrs != nullptr) && ...); }, componentPtrs);
                    if (!hasAll) continue;

                    // Викликаємо користувацьку функцію (колбек) залежно від її сигнатури (з EntityID чи без)
                    if constexpr (std::is_invocable_v<Func, EntityID, T1&, Tn&...>)
                    {
                        std::apply([&](auto*... ptrs) {
                            func(entity, comp1, *ptrs...);
                            }, componentPtrs);
                    }
                    else
                    {
                        std::apply([&](auto*... ptrs) {
                            func(comp1, *ptrs...);
                            }, componentPtrs);
                    }
                }
            }
        }
    };
}