#pragma once

#include "ResourceManager.h"

#include "MeshGenerator.h"

#include "EventDispatcher.h"
#include "InputManager.h"

#include "ISystem.h"
#include "Registry.h"
#include "Components.h"

#include "BasicRenderSystem.h"
#include "TextRenderSystem.h"

#include "CameraControlSystem.h"
#include "DataBaseCommunicateSystem.h"
#include "SpinAroundSystem.h"

#include <random>



namespace Engine {
	class ECS
	{
	public:
        // Головні менеджери управління
		ResourceManager *m_ResourceManager;
		InputManager m_inputManager;
		Registry m_registry;

        // Усі системи
		std::vector<std::unique_ptr<ISystem>> m_systems;

        // Диспетчер подій
		EventDispatcher m_events;

		void LoadAllSystems()
		{
			AddSystem<BasicRenderSystem>();
			AddSystem<TextRenderSystem>();

			AddSystem<CameraControlSystem>();
			AddSystem<DataBaseCommunicateSystem>();
			AddSystem<SpinAroundSystem>();
		}

        void CreateScene()
        {
            // 1
            std::vector<std::string> shapes = { "Cube", "Tetrahedron", "Octahedron", "Icosahedron", "Dodecahedron", "Sphere" };

            {
                // =========================================================================
                // СЦЕНА 1: УСІ ФІГУРИ ТА ТРАНСФОРМАЦІЇ
                // =========================================================================

                // Світло
                auto light1 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(light1, "Light_Scene1");
                m_registry.EmplaceComponent<TransformComponent>(light1, 0.0f, 100.0f, 100.0f);
                m_registry.EmplaceComponent<PointLightComponent>(light1, glm::vec3(1.0f), 0.5f);

                // Камера для Сцени 1
                auto camera1 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(camera1, "Camera_Scene1");
                m_registry.EmplaceComponent<CameraComponent>(camera1);
                m_registry.EmplaceComponent<TransformComponent>(camera1, 0.0f, 4.0f, 12.0f);
                m_registry.EmplaceComponent<MovementComponent>(camera1);
                
                float startX = -6.0f;
                float spacing = 2.5f;

                for (size_t i = 0; i < shapes.size(); ++i)
                {
                    auto obj = m_registry.CreateEntity();
                    m_registry.EmplaceComponent<NameComponent>(obj, "Transformed_" + shapes[i]);

                    auto& t = m_registry.EmplaceComponent<TransformComponent>(obj, startX + (i * spacing), 1.5f, 0.0f);
                    // Різні комбінації трансформацій для демонстрації
                    if (i == 0) { t.SetScale(glm::vec3(1.5f, 0.5f, 1.5f)); t.SetRotation(glm::vec3(45.0f, 0.0f, 0.0f)); }
                    else if (i == 1) { t.SetScale(glm::vec3(1.2f)); t.SetRotation(glm::vec3(0.0f, 45.0f, 0.0f)); }
                    else if (i == 2) { t.SetScale(glm::vec3(0.5f, 2.0f, 0.5f)); t.SetRotation(glm::vec3(0.0f, 0.0f, 45.0f)); }
                    else if (i == 3) { t.SetScale(glm::vec3(1.0f)); t.SetRotation(glm::vec3(30.0f, 30.0f, 0.0f)); }
                    else if (i == 4) { t.SetScale(glm::vec3(0.7f)); t.SetRotation(glm::vec3(60.0f, 0.0f, 60.0f)); }
                    else if (i == 5) { t.SetScale(glm::vec3(1.6f, 1.6f, 0.4f)); t.SetRotation(glm::vec3(15.0f, 15.0f, 15.0f)); }

                    m_registry.EmplaceComponent<MeshComponent>(obj, m_ResourceManager->getMesh(shapes[i]));
                    m_registry.EmplaceComponent<MaterialComponent>(obj, glm::vec3(0.2f), glm::vec3(0.8f), glm::vec3(1.0f), 32.0f, "NoTexture");
                }
            }
            // 2
            {
                // =========================================================================
                // СЦЕНА 2: КОЛЬОРИ ТА ТЕКСТУРИ
                // =========================================================================

                // Камера для Сцени 2
                auto camera2 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(camera2, "Camera_Scene2");
                m_registry.EmplaceComponent<CameraComponent>(camera2);
                m_registry.EmplaceComponent<TransformComponent>(camera2, 40.0f, 4.0f, 12.0f);
                m_registry.EmplaceComponent<MovementComponent>(camera2);

                std::vector<std::string> textures = { "NoTexture", "Cobble", "Concrete", "Brick", "Glass", "CorrodedMetal", "DiamondPlate" };
                std::vector<glm::vec3> colors = {
                    glm::vec3(1.0f, 0.0f, 0.0f), // Червоний
                    glm::vec3(0.0f, 1.0f, 0.0f), // Зелений
                    glm::vec3(0.0f, 0.0f, 1.0f), // Синій
                    glm::vec3(1.0f, 1.0f, 0.0f), // Жовтий
                    glm::vec3(1.0f, 0.0f, 1.0f), // Пурпурний
                    glm::vec3(0.0f, 1.0f, 1.0f), // Блакитний
                    glm::vec3(1.0f, 1.0f, 1.0f)  // Білий
                };

                float startX2 = 40.0f - 7.5f;
                for (size_t i = 0; i < textures.size(); ++i)
                {
                    auto obj = m_registry.CreateEntity();
                    m_registry.EmplaceComponent<NameComponent>(obj, "Material_Demo_" + textures[i]);
                    m_registry.EmplaceComponent<TransformComponent>(obj, startX2 + (i * 2.5f), 1.5f, 0.0f);

                    // Циклічно беремо фігуру зі списку, щоб показати різні меші з різними текстурами
                    m_registry.EmplaceComponent<MeshComponent>(obj, m_ResourceManager->getMesh(shapes[i % shapes.size()]));
                    m_registry.EmplaceComponent<MaterialComponent>(obj, colors[i] * 0.2f, colors[i], glm::vec3(0.8f), 45.0f, textures[i]);
                }
            }
            // 3
            {
                // =========================================================================
                // СЦЕНА 3: МОЖЛИВОСТІ ТЕКСТУ
                // =========================================================================

                // Камера для Сцени 3
                auto camera3 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(camera3, "Camera_Scene3");
                m_registry.EmplaceComponent<CameraComponent>(camera3);
                m_registry.EmplaceComponent<TransformComponent>(camera3, 80.0f, 4.0f, 12.0f);
                m_registry.EmplaceComponent<MovementComponent>(camera3);

                // 1. Шрифт "Text", Маленький, Червоний
                auto text1 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(text1, "Text_Small_Red");
                m_registry.EmplaceComponent<TransformComponent>(text1, 75.0f, 5.0f, 0.0f).SetScale(glm::vec3(1.0f));
                auto& txtComp1 = m_registry.EmplaceComponent<TextComponent>(text1, "Standard Font Small Red", m_ResourceManager->getTexture("Text"));
                txtComp1.color = glm::vec3(1.0f, 0.0f, 0.0f);

                // 2. Шрифт "Text", Середній, Зелений
                auto text2 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(text2, "Text_Medium_Green");
                m_registry.EmplaceComponent<TransformComponent>(text2, 75.0f, 3.5f, 0.0f).SetScale(glm::vec3(1.8f));
                auto& txtComp2 = m_registry.EmplaceComponent<TextComponent>(text2, "Standard Font Medium Green", m_ResourceManager->getTexture("Text"));
                txtComp2.color = glm::vec3(0.0f, 1.0f, 0.0f);

                // 3. Шрифт "SuperText", Звичайний, Синій
                auto text3 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(text3, "Text_Super_Blue");
                m_registry.EmplaceComponent<TransformComponent>(text3, 75.0f, 2.0f, 0.0f).SetScale(glm::vec3(1.5f));
                auto& txtComp3 = m_registry.EmplaceComponent<TextComponent>(text3, "Super Font Blue", m_ResourceManager->getTexture("SuperText"));
                txtComp3.color = glm::vec3(0.0f, 0.4f, 1.0f);

                // 4. Шрифт "SuperText", Великий, Жовтий
                auto text4 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(text4, "Text_Super_Large_Yellow");
                m_registry.EmplaceComponent<TransformComponent>(text4, 75.0f, -0.5f, 0.0f).SetScale(glm::vec3(3.0f));
                auto& txtComp4 = m_registry.EmplaceComponent<TextComponent>(text4, "HUGE YELLOW", m_ResourceManager->getTexture("SuperText"));
                txtComp4.color = glm::vec3(1.0f, 1.0f, 0.0f);
            }
            // 4
            {
                // =========================================================================
                // СЦЕНА 4: ФАБРИКА
                // =========================================================================

                float factoryOffsetX = 120.0f;

                // Камера
                auto camera4_1 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(camera4_1, "Camera_Factory_1");
                m_registry.EmplaceComponent<CameraComponent>(camera4_1);
                m_registry.EmplaceComponent<TransformComponent>(camera4_1, factoryOffsetX + 1.0f, 5.0f, 5.0f);
                m_registry.EmplaceComponent<MovementComponent>(camera4_1);

                // Камера 2
                auto camera4_2 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(camera4_2, "Camera_Factory_2");
                m_registry.EmplaceComponent<CameraComponent>(camera4_2);
                m_registry.EmplaceComponent<TransformComponent>(camera4_2, factoryOffsetX + 12.0f, 12.0f, 12.0f);
                m_registry.EmplaceComponent<MovementComponent>(camera4_2);
                m_registry.EmplaceComponent<MeshComponent>(camera4_2, m_ResourceManager->getMesh("Cube"));
                m_registry.EmplaceComponent<MaterialComponent>(camera4_2, glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f), 1.0f, "Concrete");

                // Підлога
                auto floor = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(floor, "Floor");
                m_registry.EmplaceComponent<TransformComponent>(floor, factoryOffsetX + 0.0f, 0.0f, 0.0f).SetScale(glm::vec3(6.f, 1.f, 8.f));
                m_registry.EmplaceComponent<MeshComponent>(floor, m_ResourceManager->getMesh("Cube"));
                m_registry.EmplaceComponent<MaterialComponent>(floor, glm::vec3(0.3f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, "Concrete");
                m_registry.EmplaceComponent<DataBaseConnectComponent>(floor);

                // Стіна
                auto wall = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(wall, "Wall");
                m_registry.EmplaceComponent<TransformComponent>(wall, factoryOffsetX + 0.0f, 3.5f, -3.5f).SetScale(glm::vec3(6.f, 6.f, 1.f));
                m_registry.EmplaceComponent<MeshComponent>(wall, m_ResourceManager->getMesh("Cube"));
                m_registry.EmplaceComponent<MaterialComponent>(wall, glm::vec3(1.0f, 0.4f, 0.3f), glm::vec3(1.0f, 0.4f, 0.5f), glm::vec3(1.0f, 0.4f, 0.5f), 1.0f, "Brick");
                m_registry.EmplaceComponent<DataBaseConnectComponent>(wall);

                // Об'єкти сцени
                auto obj1 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(obj1, "Obj1");
                m_registry.EmplaceComponent<TransformComponent>(obj1, factoryOffsetX + 1.5f, 1.0f, 2.5f);
                m_registry.EmplaceComponent<MeshComponent>(obj1, m_ResourceManager->getMesh("Octahedron"));
                m_registry.EmplaceComponent<MaterialComponent>(obj1, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, "DiamondPlate");
                m_registry.EmplaceComponent<DataBaseConnectComponent>(obj1);

                auto obj2 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(obj2, "Obj2");
                m_registry.EmplaceComponent<TransformComponent>(obj2, factoryOffsetX + 1.5f, 1.0f, 1.0f);
                m_registry.EmplaceComponent<MeshComponent>(obj2, m_ResourceManager->getMesh("Sphere"));
                m_registry.EmplaceComponent<MaterialComponent>(obj2, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, "DiamondPlate");
                m_registry.EmplaceComponent<DataBaseConnectComponent>(obj2);

                auto obj3 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(obj3, "Obj3");
                m_registry.EmplaceComponent<TransformComponent>(obj3, factoryOffsetX + 1.5f, 1.0f, -0.5f);
                m_registry.EmplaceComponent<MeshComponent>(obj3, m_ResourceManager->getMesh("Cube"));
                m_registry.EmplaceComponent<MaterialComponent>(obj3, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, "DiamondPlate");
                m_registry.EmplaceComponent<DataBaseConnectComponent>(obj3);

                auto obj4 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(obj4, "Obj4");
                m_registry.EmplaceComponent<TransformComponent>(obj4, factoryOffsetX + -1.5f, 1.0f, 2.5f);
                m_registry.EmplaceComponent<MeshComponent>(obj4, m_ResourceManager->getMesh("Icosahedron"));
                m_registry.EmplaceComponent<MaterialComponent>(obj4, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, "DiamondPlate");
                m_registry.EmplaceComponent<DataBaseConnectComponent>(obj4);

                auto obj5 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(obj5, "Obj5");
                m_registry.EmplaceComponent<TransformComponent>(obj5, factoryOffsetX + -1.5f, 1.0f, 1.0f);
                m_registry.EmplaceComponent<MeshComponent>(obj5, m_ResourceManager->getMesh("Tetrahedron"));
                m_registry.EmplaceComponent<MaterialComponent>(obj5, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, "DiamondPlate");
                m_registry.EmplaceComponent<DataBaseConnectComponent>(obj5);

                auto obj6 = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(obj6, "Obj6");
                m_registry.EmplaceComponent<TransformComponent>(obj6, factoryOffsetX + -1.5f, 1.0f, -0.5f);
                m_registry.EmplaceComponent<MeshComponent>(obj6, m_ResourceManager->getMesh("Dodecahedron"));
                m_registry.EmplaceComponent<MaterialComponent>(obj6, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), 1.0f, "DiamondPlate");
                m_registry.EmplaceComponent<DataBaseConnectComponent>(obj6);

                // Текст фабрики
                auto textFactory = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(textFactory, "Text_Factory");
                m_registry.EmplaceComponent<TransformComponent>(textFactory, factoryOffsetX + -1.5f, 3.0f, -2.9f).SetScale(glm::vec3(2.0f));
                m_registry.EmplaceComponent<TextComponent>(textFactory, "Factory", m_ResourceManager->getTexture("SuperText"));
            }
            // 5
            {
                // =========================================================================
                // СЦЕНА 5: СОНЯЧНА СИСТЕМА
                // =========================================================================

                float SpinOffsetX = 180.0f;
                glm::vec3 systemCenter = glm::vec3(SpinOffsetX + 0.0f, 0.0f, 0.0f);
                glm::vec3 upAxis = glm::vec3(0.0f, 1.0f, 0.0f);

                // Камера
                auto sunCamera = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(sunCamera, "Space_Camera");
                m_registry.EmplaceComponent<CameraComponent>(sunCamera);
                m_registry.EmplaceComponent<TransformComponent>(sunCamera, SpinOffsetX + 0.0f, 25.0f, 35.0f).SetRotation(glm::vec3(-40.0f, 0.0f, 0.0f));
                m_registry.EmplaceComponent<MovementComponent>(sunCamera, 10.0f, 8.0f);

                // Сонце
                auto sun = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(sun, "Sun");
                m_registry.EmplaceComponent<TransformComponent>(sun, systemCenter.x, systemCenter.y, systemCenter.z).SetScale(glm::vec3(3.0f));
                m_registry.EmplaceComponent<MeshComponent>(sun, m_ResourceManager->getMesh("Sphere"));
                m_registry.EmplaceComponent<MaterialComponent>(sun, glm::vec3(1.0f, 0.6f, 0.0f), glm::vec3(1.0f, 0.7f, 0.0f), glm::vec3(0.2f), 8.0f);
                m_registry.EmplaceComponent<PointLightComponent>(sun, glm::vec3(1.0f, 0.9f, 0.8f), 2.0f); // Излучает свет

                // Меркурій
                auto mercury = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(mercury, "Mercury");
                m_registry.EmplaceComponent<TransformComponent>(mercury, systemCenter.x, systemCenter.y, systemCenter.z).SetScale(glm::vec3(0.3f));
                m_registry.EmplaceComponent<MeshComponent>(mercury, m_ResourceManager->getMesh("Sphere"));
                m_registry.EmplaceComponent<MaterialComponent>(mercury, glm::vec3(0.4f), glm::vec3(0.5f), glm::vec3(0.1f), 16.0f);
                m_registry.EmplaceComponent<SpinAroundComponent>(mercury, sun, 4.5f, 120.0f, upAxis);

                // Вінера
                auto venus = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(venus, "Venus");
                m_registry.EmplaceComponent<TransformComponent>(venus, systemCenter.x, systemCenter.y, systemCenter.z).SetScale(glm::vec3(0.6f));
                m_registry.EmplaceComponent<MeshComponent>(venus, m_ResourceManager->getMesh("Sphere"));
                m_registry.EmplaceComponent<MaterialComponent>(venus, glm::vec3(0.7f, 0.5f, 0.3f), glm::vec3(0.8f, 0.6f, 0.4f), glm::vec3(0.1f), 32.0f);
                m_registry.EmplaceComponent<SpinAroundComponent>(venus, sun, 6.5f, -80.0f, upAxis); // Минус в скорости крутит обратно

                // Земля
                auto earth = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(earth, "Earth");
                m_registry.EmplaceComponent<TransformComponent>(earth, systemCenter.x, systemCenter.y, systemCenter.z).SetScale(glm::vec3(0.7f));
                m_registry.EmplaceComponent<MeshComponent>(earth, m_ResourceManager->getMesh("Sphere"));
                m_registry.EmplaceComponent<MaterialComponent>(earth, glm::vec3(0.1f, 0.2f, 0.6f), glm::vec3(0.2f, 0.4f, 0.8f), glm::vec3(0.4f), 64.0f);
                m_registry.EmplaceComponent<SpinAroundComponent>(earth, sun, 9.5f, 50.0f, upAxis);

                // Луна
                auto moon = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(moon, "Moon");
                m_registry.EmplaceComponent<TransformComponent>(moon, systemCenter.x, systemCenter.y, systemCenter.z).SetScale(glm::vec3(0.18f));
                m_registry.EmplaceComponent<MeshComponent>(moon, m_ResourceManager->getMesh("Sphere"));
                m_registry.EmplaceComponent<MaterialComponent>(moon, glm::vec3(0.25f), glm::vec3(0.3f), glm::vec3(0.0f), 1.0f);
                m_registry.EmplaceComponent<SpinAroundComponent>(moon, earth, 1.4f, 250.0f, glm::vec3(0.1f, 1.0f, 0.0f));

                // Марс
                auto mars = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(mars, "Mars");
                m_registry.EmplaceComponent<TransformComponent>(mars, systemCenter.x, systemCenter.y, systemCenter.z).SetScale(glm::vec3(0.45f));
                m_registry.EmplaceComponent<MeshComponent>(mars, m_ResourceManager->getMesh("Sphere"));
                m_registry.EmplaceComponent<MaterialComponent>(mars, glm::vec3(0.6f, 0.1f, 0.0f), glm::vec3(0.8f, 0.2f, 0.1f), glm::vec3(0.2f), 16.0f);
                m_registry.EmplaceComponent<SpinAroundComponent>(mars, sun, 13.0f, 40.0f, upAxis);

                // Юпітер
                auto jupiter = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(jupiter, "Jupiter");
                m_registry.EmplaceComponent<TransformComponent>(jupiter, systemCenter.x, systemCenter.y, systemCenter.z).SetScale(glm::vec3(1.8f));
                m_registry.EmplaceComponent<MeshComponent>(jupiter, m_ResourceManager->getMesh("Sphere"));
                m_registry.EmplaceComponent<MaterialComponent>(jupiter, glm::vec3(0.5f, 0.4f, 0.3f), glm::vec3(0.6f, 0.5f, 0.4f), glm::vec3(0.3f), 32.0f);
                m_registry.EmplaceComponent<SpinAroundComponent>(jupiter, sun, 18.5f, 20.0f, upAxis);

                // Сатурн
                auto saturn = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(saturn, "Saturn");
                m_registry.EmplaceComponent<TransformComponent>(saturn, systemCenter.x, systemCenter.y, systemCenter.z).SetScale(glm::vec3(1.4f));
                m_registry.EmplaceComponent<MeshComponent>(saturn, m_ResourceManager->getMesh("Sphere"));
                m_registry.EmplaceComponent<MaterialComponent>(saturn, glm::vec3(0.65f, 0.55f, 0.35f), glm::vec3(0.75f, 0.65f, 0.45f), glm::vec3(0.2f), 32.0f);
                m_registry.EmplaceComponent<SpinAroundComponent>(saturn, sun, 24.0f, 15.0f, upAxis);

                // Кільце
                float ringRadius = 2.4f;
                float ringSpeed = 160.0f;
                glm::vec3 ringAxis = glm::normalize(glm::vec3(0.15f, 0.95f, 0.0f));

                glm::vec3 helper = (std::abs(ringAxis.y) < 0.999f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
                glm::vec3 dirU = glm::normalize(glm::cross(ringAxis, helper));
                glm::vec3 dirV = glm::normalize(glm::cross(ringAxis, dirU));

                for (int i = 0; i < 32; ++i)
                {
                    auto ringPart = m_registry.CreateEntity();
                    m_registry.EmplaceComponent<NameComponent>(ringPart, "Saturn_Ring_" + std::to_string(i));

                    float angle = glm::radians(i * (360.f / 32.f));

                    glm::vec3 localOffset = (dirU * cos(angle) + dirV * sin(angle)) * ringRadius;

                    glm::vec3 spawnPos = systemCenter + localOffset;

                    m_registry.EmplaceComponent<TransformComponent>(ringPart, spawnPos.x, spawnPos.y, spawnPos.z).SetScale(glm::vec3(0.12f));
                    m_registry.EmplaceComponent<MeshComponent>(ringPart, m_ResourceManager->getMesh("Sphere"));
                    m_registry.EmplaceComponent<MaterialComponent>(ringPart, glm::vec3(0.5f, 0.4f, 0.3f), glm::vec3(0.55f, 0.45f, 0.35f), glm::vec3(0.0f), 8.0f);

                    m_registry.EmplaceComponent<SpinAroundComponent>(ringPart, saturn, ringRadius, ringSpeed, ringAxis, localOffset);
                }

                // Уран
                auto uranus = m_registry.CreateEntity();
                m_registry.EmplaceComponent<NameComponent>(uranus, "Uranus");
                m_registry.EmplaceComponent<TransformComponent>(uranus, systemCenter.x, systemCenter.y, systemCenter.z).SetScale(glm::vec3(1.0f));
                m_registry.EmplaceComponent<MeshComponent>(uranus, m_ResourceManager->getMesh("Sphere"));
                m_registry.EmplaceComponent<MaterialComponent>(uranus, glm::vec3(0.4f, 0.6f, 0.7f), glm::vec3(0.5f, 0.65f, 0.75f), glm::vec3(0.3f), 32.0f);
                m_registry.EmplaceComponent<SpinAroundComponent>(uranus, sun, 30.0f, 8.0f, upAxis);
            }
        }

		void Init() 
		{
            // Получаємо ресурсний менеджер
			m_ResourceManager = ResourceManager::Instance;

			// Завантаження Фігур
			m_ResourceManager->loadMesh("Cube", MeshGenerator::Cube(1.0f));
			m_ResourceManager->loadMesh("Tetrahedron", MeshGenerator::Tetrahedron(1.0f));
			m_ResourceManager->loadMesh("Octahedron", MeshGenerator::Octahedron(1.0f));
			m_ResourceManager->loadMesh("Icosahedron", MeshGenerator::Icosahedron(1.0f));
			m_ResourceManager->loadMesh("Dodecahedron", MeshGenerator::Dodecahedron(1.0f));
			m_ResourceManager->loadMesh("Sphere", MeshGenerator::Sphere(0.75f, 16, 16));
			
			m_ResourceManager->loadTexture("NoTexture", "Textures/notexture.png");

			// Завантаження Текстур
			m_ResourceManager->loadTexture("Cobble", "Textures/cobble.png");
			m_ResourceManager->loadTexture("Concrete", "Textures/concrete.png");
			m_ResourceManager->loadTexture("Brick", "Textures/brick.png");
			m_ResourceManager->loadTexture("Glass", "Textures/glass.png");
			m_ResourceManager->loadTexture("CorrodedMetal", "Textures/corrodedmetal.png");
			m_ResourceManager->loadTexture("DiamondPlate", "Textures/diamondplate.png");

			// Текстовий атлас
			std::vector<std::string> subTextures = {
				" ", "!", "\"", "#", "$", "%", "_", "'", "(", ")",
				"*", "+", ",", "-", ".", "/", "0", "1", "2", "3",
				"4", "5", "6", "7", "8", "9", ":", ";", "<", "=",
				">", "?", "@", "A", "B", "C", "D", "E", "F", "G",
				"H", "I", "J", "K", "L", "M", "N", "O", "P", "Q",
				"R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[",
				"\\", "]", "^", "__", "`", "a", "b", "c", "d", "e",
				"f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
				"p", "q", "r", "s", "t", "u", "v", "w", "x", "y",
				"z", "{", "|", "}", "~", "box1", "€", "box2", "coma", "ƒ"
			};

			m_ResourceManager->loadTextureAtlas("Text", "Textures/text_atlas.png", subTextures, 25, 25);
			m_ResourceManager->loadTextureAtlas("SuperText", "Textures/super_text.png", subTextures, 102, 102);


			LoadAllSystems();


			CreateScene();


			for (auto& system : m_systems)
			{
				system->Init(*this);
			}
		}

		void Shutdown() {}

		void FixedUpdate(float fixed_dt) 
		{
			for (auto& system : m_systems)
			{
                if (system->IsEnabled()) {
                    system->FixedUpdate(*this, fixed_dt);
                }
			}
		}

		void Update(float dt) 
		{
			for (auto& system : m_systems)
			{
                if (system->IsEnabled()) {
                    system->Update(*this, dt);
                }
			}

			m_inputManager.Update();
		}

		void Render() 
		{
			for (auto& system : m_systems)
			{
                if (system->IsEnabled()) {
                    system->Render(*this);
                }
			}
		}

		template <typename T, typename... Args>
		T* AddSystem(Args&&... args)
		{
			auto system = std::make_unique<T>(std::forward<Args>(args)...);
			T* systemPtr = system.get();
			m_systems.push_back(std::move(system));

			return systemPtr;
		}
	};
}
