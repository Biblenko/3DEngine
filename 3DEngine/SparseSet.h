#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <span>
#include <functional>
#include <stdexcept>
#include <limits>
#include <cassert>

namespace Engine
{
    using EntityID = size_t;

    constexpr EntityID MAX_ENTITIES = 0xAAFFFF;
    constexpr size_t NULL_INDEX = (size_t)(-1);

    // =========================================================================
    //  ISparseSet — interface
    // =========================================================================
    class ISparseSet
    {
    public:
        virtual ~ISparseSet() = default;

        virtual bool   Remove(EntityID entity) = 0;
        virtual bool   Has(EntityID entity) const = 0;
        virtual void   Clear() = 0;
        virtual size_t Size() const noexcept = 0;
        virtual bool   Empty() const noexcept = 0;
    };


    template <typename T>
    class SparseSet final : public ISparseSet
    {
        // ──────────────────────────────────────────────────────────────────
        //  Internals
        // ──────────────────────────────────────────────────────────────────
        std::vector<T>                       m_denseData;       // Component data
        std::vector<EntityID>                m_denseToEntity;   // Index  → EntityID
        std::unordered_map<EntityID, size_t> m_sparse;          // EntityID → Index

    public:
        // ──────────────────────────────────────────────────────────────────
        //  Iterator — yields { EntityID, T& } entries
        // ──────────────────────────────────────────────────────────────────
        template<typename SetT, typename ValueT>
        class IteratorBase
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;

            struct Entry { EntityID entity; ValueT& component; };

            IteratorBase(SetT& set, size_t index) : m_set(set), m_index(index) {}

            Entry operator*() const
            {
                return { m_set.m_denseToEntity[m_index], m_set.m_denseData[m_index] };
            }

            IteratorBase& operator++() { ++m_index; return *this; }
            IteratorBase  operator++(int) { auto tmp = *this; ++m_index; return tmp; }
            IteratorBase& operator--() { --m_index; return *this; }
            IteratorBase  operator--(int) { auto tmp = *this; --m_index; return tmp; }

            IteratorBase  operator+(difference_type n) const { return { m_set, m_index + n }; }
            IteratorBase  operator-(difference_type n) const { return { m_set, m_index - n }; }
            difference_type operator-(const IteratorBase& o) const
            {
                return static_cast<difference_type>(m_index) - static_cast<difference_type>(o.m_index);
            }

            bool operator==(const IteratorBase& o) const { return m_index == o.m_index; }
            bool operator!=(const IteratorBase& o) const { return m_index != o.m_index; }
            bool operator< (const IteratorBase& o) const { return m_index < o.m_index; }

        private:
            SetT& m_set;
            size_t m_index;
        };

        using Iterator = IteratorBase<SparseSet, T>;
        using ConstIterator = IteratorBase<const SparseSet, const T>;

        // ──────────────────────────────────────────────────────────────────
        //  Construction / destruction
        // ──────────────────────────────────────────────────────────────────
        SparseSet() = default;
        ~SparseSet() override = default;

        SparseSet(const SparseSet&) = delete;
        SparseSet& operator=(const SparseSet&) = delete;

        SparseSet(SparseSet&&) noexcept = default;
        SparseSet& operator=(SparseSet&&) noexcept = default;

        // ──────────────────────────────────────────────────────────────────
        //  Capacity
        // ──────────────────────────────────────────────────────────────────
        void Reserve(size_t capacity)
        {
            m_denseData.reserve(capacity);
            m_denseToEntity.reserve(capacity);
            m_sparse.reserve(capacity);
        }

        [[nodiscard]] size_t Size()  const noexcept override { return m_denseData.size(); }
        [[nodiscard]] bool   Empty() const noexcept override { return m_denseData.empty(); }

        // ──────────────────────────────────────────────────────────────────
        //  Core mutating operations
        // ──────────────────────────────────────────────────────────────────

        /// Insert or replace component for entity.
        /// Returns a reference to the stored component.
        template<typename... Args>
        T& Emplace(EntityID entity, Args&&... args)
        {
            auto it = m_sparse.find(entity);
            if (it != m_sparse.end())
            {
                // Replace existing
                T& existing = m_denseData[it->second];
                existing = T(std::forward<Args>(args)...);
                return existing;
            }

            // Insert new
            const size_t newIndex = m_denseData.size();
            m_denseData.emplace_back(std::forward<Args>(args)...);
            m_denseToEntity.push_back(entity);
            m_sparse.emplace(entity, newIndex);

            return m_denseData.back();
        }

        /// Remove entity. Returns true if the entity was present.
        bool Remove(EntityID entity) override
        {
            auto it = m_sparse.find(entity);
            if (it == m_sparse.end()) return false;

            const size_t indexRemoved = it->second;
            const size_t indexLast = m_denseData.size() - 1;

            // Swap with last element to maintain dense packing
            if (indexRemoved != indexLast)
            {
                const EntityID entityLast = m_denseToEntity[indexLast];

                m_denseData[indexRemoved] = std::move(m_denseData[indexLast]);
                m_denseToEntity[indexRemoved] = entityLast;
                m_sparse[entityLast] = indexRemoved;
            }

            m_denseData.pop_back();
            m_denseToEntity.pop_back();
            m_sparse.erase(it);

            return true;
        }

        void Clear() noexcept override
        {
            m_denseData.clear();
            m_denseToEntity.clear();
            m_sparse.clear();
        }

        // ──────────────────────────────────────────────────────────────────
        //  Lookup
        // ──────────────────────────────────────────────────────────────────

        [[nodiscard]] bool Has(EntityID entity) const override
        {
            return m_sparse.count(entity) != 0;
        }

        /// Returns nullptr if entity has no component.
        [[nodiscard]] T* Get(EntityID entity) noexcept
        {
            auto it = m_sparse.find(entity);
            return (it != m_sparse.end()) ? &m_denseData[it->second] : nullptr;
        }

        [[nodiscard]] const T* Get(EntityID entity) const noexcept
        {
            auto it = m_sparse.find(entity);
            return (it != m_sparse.end()) ? &m_denseData[it->second] : nullptr;
        }

        /// Throws std::out_of_range if entity has no component.
        [[nodiscard]] T& GetChecked(EntityID entity)
        {
            auto it = m_sparse.find(entity);
            if (it == m_sparse.end())
                throw std::out_of_range("SparseSet::GetChecked — entity not found");
            return m_denseData[it->second];
        }

        [[nodiscard]] const T& GetChecked(EntityID entity) const
        {
            auto it = m_sparse.find(entity);
            if (it == m_sparse.end())
                throw std::out_of_range("SparseSet::GetChecked — entity not found");
            return m_denseData[it->second];
        }

        // ──────────────────────────────────────────────────────────────────
        //  Iteration
        // ──────────────────────────────────────────────────────────────────
        Iterator      begin()        noexcept { return { *this, 0 }; }
        Iterator      end()          noexcept { return { *this, m_denseData.size() }; }
        ConstIterator begin()  const noexcept { return { *this, 0 }; }
        ConstIterator end()    const noexcept { return { *this, m_denseData.size() }; }
        ConstIterator cbegin() const noexcept { return begin(); }
        ConstIterator cend()   const noexcept { return end(); }

        /// Direct access to tightly-packed component array — best cache performance.
        [[nodiscard]] std::span<T>         Components()       noexcept { return m_denseData; }
        [[nodiscard]] std::span<const T>   Components() const noexcept { return m_denseData; }

        /// Read-only access to the entity list (parallel to Components()).
        [[nodiscard]] const std::vector<EntityID>& Entities() const noexcept
        {
            return m_denseToEntity;
        }

        // ──────────────────────────────────────────────────────────────────
        //  ForEach helpers
        // ──────────────────────────────────────────────────────────────────

        /// Iterate over all (EntityID, T&) pairs.
        template<typename Fn>
        void ForEach(Fn&& fn)
        {
            for (size_t i = 0; i < m_denseData.size(); ++i)
                fn(m_denseToEntity[i], m_denseData[i]);
        }

        template<typename Fn>
        void ForEach(Fn&& fn) const
        {
            for (size_t i = 0; i < m_denseData.size(); ++i)
                fn(m_denseToEntity[i], m_denseData[i]);
        }

        // ──────────────────────────────────────────────────────────────────
        //  Sorting
        // ──────────────────────────────────────────────────────────────────

        /// Sort components in-place. comp(T, T) → bool (like std::less).
        /// After sorting, all internal indices are updated automatically.
        template<typename Compare>
        void Sort(Compare comp)
        {
            const size_t count = m_denseData.size();
            if (count < 2) return;

            // Build permutation
            std::vector<size_t> perm(count);
            std::iota(perm.begin(), perm.end(), 0);
            std::sort(perm.begin(), perm.end(), [&](size_t i, size_t j)
                {
                    return comp(m_denseData[i], m_denseData[j]);
                });

            // Apply permutation
            std::vector<T>        sortedData;
            std::vector<EntityID> sortedEntities;
            sortedData.reserve(count);
            sortedEntities.reserve(count);

            for (size_t i = 0; i < count; ++i)
            {
                sortedData.push_back(std::move(m_denseData[perm[i]]));
                sortedEntities.push_back(m_denseToEntity[perm[i]]);
            }

            m_denseData = std::move(sortedData);
            m_denseToEntity = std::move(sortedEntities);

            // Rebuild sparse index
            for (size_t i = 0; i < count; ++i)
                m_sparse[m_denseToEntity[i]] = i;
        }
    };
}