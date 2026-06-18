#include "pch.h"
#include "DataBaseCommunicateSystem.h"

#include "ECS.h"

/// Ініціалізація системи. Запускає окремий робочий потік для взаємодії з базою даних,
/// щоб уникнути блокування головного потоку рендерингу/оновлення движка.
void Engine::DataBaseCommunicateSystem::Init(ECS& e)
{
    m_dbThread = std::thread(&DataBaseCommunicateSystem::InitDataBase, this, std::ref(e));
}

/// Виконується в головному потоці движка кожний кадр.
/// Переносить отримані з БД зміни кольору в компоненти відповідних сутностей.
void Engine::DataBaseCommunicateSystem::Update(ECS& e, float dt)
{
    // Блокуємо доступ до черги змін, оскільки фоновий потік може одночасно в неї писати
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& entityData : m_entitysToChange)
    {
        auto material = e.m_registry.GetComponent<MaterialComponent>(entityData.id);

        if (material)
            material->m_ambient = glm::vec3(entityData.r, entityData.g, entityData.b);
    }

    // Очищаємо чергу після застосування всіх актуальних оновлень
    m_entitysToChange.clear();
}

/// Робочий метод для фонового потоку. Відповідає за підключення до MySQL,
/// початковий експорт стану ECS та регулярне опитування таблиці на наявність змін.
void Engine::DataBaseCommunicateSystem::InitDataBase(ECS& e)
{
    try {
        // Встановлення з'єднання з MySQL сервером через X DevAPI
        mysqlx::Session sess("127.0.1.31", 33060, "root", "");
        sess.sql("USE test").execute();

        // Очищаємо таблицю перед початком сесії для синхронізації «з нуля»
        sess.sql("TRUNCATE TABLE color").execute();

        // Початковий експорт: знаходимо всі сутності з необхідним набором компонентів
        // та записуємо їхні поточні дані в базу даних
        e.m_registry.Each<DataBaseConnectComponent, NameComponent, MaterialComponent>([&sess](EntityID entityId, DataBaseConnectComponent& db, NameComponent& name, MaterialComponent& material)
            {
                sess.sql("INSERT INTO color (entity, name, ambient_r, ambient_g, ambient_b) VALUES (?, ?, ?, ?, ?)")
                    .bind(entityId, name.m_name, material.m_ambient.r, material.m_ambient.g, material.m_ambient.b)
                    .execute();
            });

        // Головний цикл опитування бази даних (Polling loop)
        while (m_isRunning) {
            // Опитування бази даних кожні 3 секунди, щоб зменшити навантаження на мережу та СУБД
            std::this_thread::sleep_for(std::chrono::seconds(3));

            // Запитуємо актуальні значення кольорів
            mysqlx::SqlResult res = sess.sql("SELECT entity, ambient_r, ambient_g, ambient_b FROM color").execute();

            if (res.count() > 0) {
                {
                    // Захищаємо чергу м'ютексом на час запису нових даних з БД
                    std::lock_guard<std::mutex> lock(m_mutex);

                    for (mysqlx::Row row : res) {
                        m_entitysToChange.push_back(
                            entity_data{ row[0].get<int>(),
                                            row[1].get<float>(), row[2].get<float>(), row[3].get<float>() });
                    }
                }
            }
        }
    }
    catch (const mysqlx::Error& err) {
        // Обробка специфічних помилок MySQL із виведенням вікна повідомлення MFC (UTF-8 -> Конвертація в WideChar)
        CString msg = _T("MySQL Error: \n");
        msg += CString(CA2W(err.what(), CP_UTF8));

        AfxMessageBox(msg);
    }
    catch (const std::exception& ex) {
        // Обробка інших стандартних винятків C++
        CString msg = _T("Standard Exception: \n");
        msg += CString(CA2W(ex.what(), CP_UTF8));

        AfxMessageBox(msg);
    }
}

/// Деструктор системи. Забезпечує коректну зупинку фонового потоку без крашів додатка.
Engine::DataBaseCommunicateSystem::~DataBaseCommunicateSystem()
{
    if (m_dbThread.joinable()) {
        m_isRunning = false; // Виставляємо прапорець для виходу з циклу while у фоновому потоці

        m_dbThread.join(); // Очікуємо повного завершення роботи фонового потоку
    }
}