#include "pch.h"
#include "DataBaseCommunicateSystem.h"

#include "ECS.h"


void Engine::DataBaseCommunicateSystem::Init(ECS& e)
{
    m_dbThread = std::thread(&DataBaseCommunicateSystem::InitDataBase, this, std::ref(e));

    //InitDataBase(e);
}

void Engine::DataBaseCommunicateSystem::Update(ECS& e, float dt)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& entityData : m_entitysToChange)
    {
        auto material = e.m_registry.GetComponent<MaterialComponent>(entityData.id);

        if (material)
            material->m_ambient = glm::vec3(entityData.r, entityData.g, entityData.b);
    }

    m_entitysToChange.clear();
}

void Engine::DataBaseCommunicateSystem::InitDataBase(ECS& e)
{
    try {
        mysqlx::Session sess("127.0.1.31", 33060, "root", "");
        sess.sql("USE test").execute();

        sess.sql("TRUNCATE TABLE color").execute();

        //auto linked_entities = e.m_registry.Entities<DataBaseConnectComponent>();

        e.m_registry.Each<DataBaseConnectComponent, NameComponent, MaterialComponent>([&sess](EntityID entityId, DataBaseConnectComponent& db, NameComponent& name, MaterialComponent& material)
            {
                sess.sql("INSERT INTO color (entity, name, ambient_r, ambient_g, ambient_b) VALUES (?, ?, ?, ?, ?)")
                    .bind(entityId, name.m_name, material.m_ambient.r, material.m_ambient.g, material.m_ambient.b)
                    .execute();
            });

        while (m_isRunning) {
            std::this_thread::sleep_for(std::chrono::seconds(3));

            mysqlx::SqlResult res = sess.sql("SELECT entity, ambient_r, ambient_g, ambient_b FROM color").execute();

            if (res.count() > 0) {
                {
                    std::lock_guard<std::mutex> lock(m_mutex);

                    for (mysqlx::Row row : res) {
                        m_entitysToChange.push_back(
                            entity_data{    row[0].get<int>(), 
                                            row[1].get<float>(), row[2].get<float>(), row[3].get<float>() });
                    }
                }

                //sess.sql("UPDATE alarm SET value = 0 WHERE value = 1").execute();
            }
        }
    }
    catch (const mysqlx::Error& err) {
        CString msg = _T("MySQL Error: \n");
        msg += CString(CA2W(err.what(), CP_UTF8));

        AfxMessageBox(msg);
    }
    catch (const std::exception& ex) {
        CString msg = _T("Standard Exception: \n");
        msg += CString(CA2W(ex.what(), CP_UTF8));

        AfxMessageBox(msg);
    }
}

Engine::DataBaseCommunicateSystem::~DataBaseCommunicateSystem()
{
    if (m_dbThread.joinable()) {
        m_isRunning = false;

        m_dbThread.join();
    }
}
