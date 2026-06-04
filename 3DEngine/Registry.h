#pragma once
#include "SparseSet.h"

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <vector>

namespace Engine
{
    // =========================================================================
    //  Registry
    //  Owns all component pools and manages entity lifetimes.
    // =========================================================================
    class Registry
    {
        // unique_ptr: Registry is the sole owner — no shared ownership needed
        using PoolMap = std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>>;

        EntityID  m_nextEntityId = 0;
        std::vector<EntityID> m_freeEntities;
        PoolMap   m_componentSets;

    public:
        // ──────────────────────────────────────────────────────────────────
        //  Entity lifecycle
        // ──────────────────────────────────────────────────────────────────

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

        void DestroyEntity(EntityID entity)
        {
            for (auto& [type, pool] : m_componentSets)
                pool->Remove(entity);

            m_freeEntities.push_back(entity);
        }

        void Clear()
        {
            for (auto& [type, pool] : m_componentSets)
                pool->Clear();

            m_nextEntityId = 0;
            m_freeEntities.clear();
        }

        // ──────────────────────────────────────────────────────────────────
        //  Pool access (internal)
        //
        //  GetPool()     — creates pool if missing, use for writes
        //  TryGetPool()  — returns nullptr if missing, use for reads
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
        //  Component operations
        // ──────────────────────────────────────────────────────────────────

        /// Add or replace a component on entity.
        template<typename T, typename... Args>
        T& EmplaceComponent(EntityID entity, Args&&... args)
        {
            return GetPool<T>().Emplace(entity, std::forward<Args>(args)...);
        }

        /// Remove a component from entity. Returns true if it existed.
        template<typename T>
        bool RemoveComponent(EntityID entity)
        {
            auto* pool = TryGetPool<T>();
            return pool && pool->Remove(entity);
        }

        /// Returns nullptr if entity has no component of type T.
        template<typename T>
        [[nodiscard]] T* GetComponent(EntityID entity)
        {
            auto* pool = TryGetPool<T>();
            return pool ? pool->Get(entity) : nullptr;
        }

        template<typename T>
        [[nodiscard]] const T* GetComponent(EntityID entity) const
        {
            auto* pool = TryGetPool<T>();
            return pool ? pool->Get(entity) : nullptr;
        }

        template<typename T>
        [[nodiscard]] bool HasComponent(EntityID entity) const
        {
            auto* pool = TryGetPool<T>();
            return pool && pool->Has(entity);
        }

        // ──────────────────────────────────────────────────────────────────
        //  Single-type views — direct span access, zero overhead
        // ──────────────────────────────────────────────────────────────────

        /// Returns span over tightly-packed component array.
        template<typename T>
        [[nodiscard]] std::span<T> Components()
        {
            return GetPool<T>().Components();
        }

        template<typename T>
        [[nodiscard]] std::span<const T> Components() const
        {
            auto* pool = TryGetPool<T>();
            return pool ? pool->Components() : std::span<const T>{};
        }

        /// Returns the entity list parallel to Components<T>().
        template<typename T>
        [[nodiscard]] const std::vector<EntityID>& Entities() const
        {
            static const std::vector<EntityID> empty;
            auto* pool = TryGetPool<T>();
            return pool ? pool->Entities() : empty;
        }

        // ──────────────────────────────────────────────────────────────────
        //  Multi-component iteration  Each<T1, T2, ...>(func)
        // ──────────────────────────────────────────────────────────────────
        template<typename T1, typename... Tn, typename Func>
        void Each(Func&& func)
        {
            SparseSet<T1>& pool1 = GetPool<T1>();

            if constexpr (sizeof...(Tn) == 0)
            {
                // Fast path: single component, no Has() checks needed
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
                // Cache pointers to all secondary component pools once before the loop
                auto otherPools = std::make_tuple(TryGetPool<Tn>()...);

                // If any required pool is missing, there is nothing to iterate
                bool allExist = std::apply([](auto*... pools) { return ((pools != nullptr) && ...); }, otherPools);
                if (!allExist) return;

                // Main iteration loop over the primary pool
                for (auto [entity, comp1] : pool1)
                {
                    // Fetch pointers to secondary components using a single lookup per pool
                    auto componentPtrs = std::make_tuple(std::get<SparseSet<Tn>*>(otherPools)->Get(entity)...);

                    // Validate that the entity contains all required components
                    bool hasAll = std::apply([](auto*... ptrs) { return ((ptrs != nullptr) && ...); }, componentPtrs);
                    if (!hasAll) continue;

                    // Invoke user callback with or without EntityID based on its signature
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