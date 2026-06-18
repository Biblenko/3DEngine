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
    using EntityID = size_t; // Унікальний ідентифікатор сутності

    constexpr EntityID MAX_ENTITIES = 0xAAFFFF; // Максимально дозволена кількість сутностей у системі
    constexpr size_t NULL_INDEX = (size_t)(-1); // Маркер недійсного або порожнього індексу

    // =========================================================================
    //  ISparseSet — інтерфейс
    //  Поліморфний інтерфейс для SparseSet. Дозволяє керувати пулами компонентів
    //  на рівні реєстру без знання конкретного типу даних T (Type Erasure).
    // =========================================================================
    class ISparseSet
    {
    public:
        virtual ~ISparseSet() = default;

        virtual bool   Remove(EntityID entity) = 0;       // Видалити сутність з пулу
        virtual bool   Has(EntityID entity) const = 0;    // Перевірити наявність сутності в пулі
        virtual void   Clear() = 0;                       // Повністю очистити пул
        virtual size_t Size() const noexcept = 0;         // Отримати кількість елементів
        virtual bool   Empty() const noexcept = 0;        // Перевірити пул на порожнечу
    };


    template <typename T>
    class SparseSet final : public ISparseSet
    {
        // ──────────────────────────────────────────────────────────────────
        //  Внутрішні структури даних
        // ──────────────────────────────────────────────────────────────────
        std::vector<T>                       m_denseData;     // Щільно упакований масив компонентів (гарантує локальність кешу при ітерації)
        std::vector<EntityID>                m_denseToEntity; // Пряме відображення: Індекс щільного масиву → EntityID сутності
        std::unordered_map<EntityID, size_t> m_sparse;        // Зворотне відображення: EntityID сутності → Індекс у щільному масиві

    public:
        // ──────────────────────────────────────────────────────────────────
        //  Ітератор — повертає пари { EntityID, T& }
        //  Реалізує Random Access Iterator для повної сумісності з алгоритмами STL.
        // ──────────────────────────────────────────────────────────────────
        template<typename SetT, typename ValueT>
        class IteratorBase
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;

            struct Entry { EntityID entity; ValueT& component; }; // Обгортка результату розіменування

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
        //  Конструктори / деструктор
        // ──────────────────────────────────────────────────────────────────
        SparseSet() = default;
        ~SparseSet() override = default;

        // Заборона копіювання пулу для уникнення випадкових важких алокацій
        SparseSet(const SparseSet&) = delete;
        SparseSet& operator=(const SparseSet&) = delete;

        // Дозвіл переміщення (move-семантика)
        SparseSet(SparseSet&&) noexcept = default;
        SparseSet& operator=(SparseSet&&) noexcept = default;

        // ──────────────────────────────────────────────────────────────────
        //  Місткість (Capacity)
        // ──────────────────────────────────────────────────────────────────

        /// Попередньо виділяє пам'ять у внутрішніх контейнерах для запобігання реаллокаціям під час динамічного додавання.
        void Reserve(size_t capacity)
        {
            m_denseData.reserve(capacity);
            m_denseToEntity.reserve(capacity);
            m_sparse.reserve(capacity);
        }

        [[nodiscard]] size_t Size()  const noexcept override { return m_denseData.size(); }
        [[nodiscard]] bool   Empty() const noexcept override { return m_denseData.empty(); }

        // ──────────────────────────────────────────────────────────────────
        //  Основні мутуючі операції
        // ──────────────────────────────────────────────────────────────────

        /// Додає новий компонент або замінює існуючий для сутності.
        /// Конструює об'єкт на місці (inplace) через std::forward. Повертає посилання на нього.
        template<typename... Args>
        T& Emplace(EntityID entity, Args&&... args)
        {
            auto it = m_sparse.find(entity);
            if (it != m_sparse.end())
            {
                // Компонент вже існує: перезаписуємо його оператором присвоювання
                T& existing = m_denseData[it->second];
                existing = T(std::forward<Args>(args)...);
                return existing;
            }

            // Компонента немає: додаємо в кінець щільних масивів та реєструємо в мапі
            const size_t newIndex = m_denseData.size();
            m_denseData.emplace_back(std::forward<Args>(args)...);
            m_denseToEntity.push_back(entity);
            m_sparse.emplace(entity, newIndex);

            return m_denseData.back();
        }

        /// Видаляє компонент сутності. Реалізує механізм Swap-and-Pop:
        /// затирає видалений елемент останнім елементом масиву, зберігаючи $O(1)$ складність та щільність пакування даних.
        bool Remove(EntityID entity) override
        {
            auto it = m_sparse.find(entity);
            if (it == m_sparse.end()) return false;

            const size_t indexRemoved = it->second;
            const size_t indexLast = m_denseData.size() - 1;

            // Якщо видаляється не останній елемент — переміщуємо останній на місце видаленого
            if (indexRemoved != indexLast)
            {
                const EntityID entityLast = m_denseToEntity[indexLast];

                m_denseData[indexRemoved] = std::move(m_denseData[indexLast]);
                m_denseToEntity[indexRemoved] = entityLast;
                m_sparse[entityLast] = indexRemoved; // Оновлюємо індекс переміщеної сутності в мапі
            }

            // Видаляємо дубльований останній елемент
            m_denseData.pop_back();
            m_denseToEntity.pop_back();
            m_sparse.erase(it);

            return true;
        }

        /// Повне очищення пулу без зміни виділеної місткості (capacity) контейнерів.
        void Clear() noexcept override
        {
            m_denseData.clear();
            m_denseToEntity.clear();
            m_sparse.clear();
        }

        // ──────────────────────────────────────────────────────────────────
        //  Пошук (Lookup)
        // ──────────────────────────────────────────────────────────────────

        [[nodiscard]] bool Has(EntityID entity) const override
        {
            return m_sparse.count(entity) != 0;
        }

        /// Повертає вказівник на компонент. Якщо компонент відсутній, повертає nullptr. Не кидає винятків.
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

        /// Повертає посилання на компонент. Кидає std::out_of_range, якщо сутність не знайдена.
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
        //  Отримання стандартних ітераторів
        // ──────────────────────────────────────────────────────────────────
        Iterator      begin()        noexcept { return { *this, 0 }; }
        Iterator      end()          noexcept { return { *this, m_denseData.size() }; }
        ConstIterator begin()  const noexcept { return { *this, 0 }; }
        ConstIterator end()    const noexcept { return { *this, m_denseData.size() }; }
        ConstIterator cbegin() const noexcept { return begin(); }
        ConstIterator cend()   const noexcept { return end(); }

        /// Прямий доступ до сирого неперервного масиву компонентів через std::span. Максимальна ефективність для CPU-кешу.
        [[nodiscard]] std::span<T>           Components()       noexcept { return m_denseData; }
        [[nodiscard]] std::span<const T>   Components() const noexcept { return m_denseData; }

        /// Доступ до масиву сутностей, чиї індекси строго паралельні масиву Components().
        [[nodiscard]] const std::vector<EntityID>& Entities() const noexcept
        {
            return m_denseToEntity;
        }

        // ──────────────────────────────────────────────────────────────────
        //  Допоміжні методи обходу (ForEach)
        // ──────────────────────────────────────────────────────────────────

        /// Перебір всіх елементів пулу через лямбда-функцію signature: fn(EntityID, T&).
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
        //  Сортування (Sorting)
        // ──────────────────────────────────────────────────────────────────

        /// Сортує компоненти за допомогою непрямої адресації (генерація індексної перестановки perm).
        /// Гарантує, що після зміни позицій компонентів у m_denseData всі індекси в m_sparse будуть повністю перебудовані.
        template<typename Compare>
        void Sort(Compare comp)
        {
            const size_t count = m_denseData.size();
            if (count < 2) return;

            // Створюємо масив індексів [0, 1, 2, ... count-1]
            std::vector<size_t> perm(count);
            std::iota(perm.begin(), perm.end(), 0);

            // Сортуємо індекси перестановки на основі значень реальних даних компонентів
            std::sort(perm.begin(), perm.end(), [&](size_t i, size_t j)
                {
                    return comp(m_denseData[i], m_denseData[j]);
                });

            // Тимчасові буфери для алокації відсортованих даних
            std::vector<T>       sortedData;
            std::vector<EntityID> sortedEntities;
            sortedData.reserve(count);
            sortedEntities.reserve(count);

            // Переміщуємо дані у відсортованому порядку
            for (size_t i = 0; i < count; ++i)
            {
                sortedData.push_back(std::move(m_denseData[perm[i]]));
                sortedEntities.push_back(m_denseToEntity[perm[i]]);
            }

            m_denseData = std::move(sortedData);
            m_denseToEntity = std::move(sortedEntities);

            // Повна перебудова розрідженої таблиці (хеш-мапи) під новий порядок індексів
            for (size_t i = 0; i < count; ++i)
                m_sparse[m_denseToEntity[i]] = i;
        }
    };
}