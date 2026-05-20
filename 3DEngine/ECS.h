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



namespace Engine {
	class ECS
	{
	public:
		ResourceManager *m_ResourceManager;
		InputManager m_inputManager;
		Registry m_registry;

		std::vector<std::unique_ptr<ISystem>> m_systems;

		EventDispatcher m_events;

		void LoadAllSystems()
		{
			AddSystem<BasicRenderSystem>();
			AddSystem<TextRenderSystem>();

			AddSystem<CameraControlSystem>();
			AddSystem<DataBaseCommunicateSystem>();
		}

		void CreateScene()
		{
			auto pointLight = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(pointLight, "PointLight");
			m_registry.EmplaceComponent<TransformComponent>(pointLight, 32.0f, 32.0f, 32.0f);
			m_registry.EmplaceComponent<PointLightComponent>(pointLight);

			auto camera = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(camera, "Camera");
			m_registry.EmplaceComponent<CameraComponent>(camera);
			m_registry.EmplaceComponent<TransformComponent>(camera, 1.f, 1.f, 1.f);
			m_registry.EmplaceComponent<MovementComponent>(camera);

			/*auto floor = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(floor, "Floor");
			m_registry.EmplaceComponent<TransformComponent>(floor, 0.0f, 0.0f, 0.0f);
			m_registry.EmplaceComponent<MeshComponent>(floor, m_ResourceManager->getMesh("Cube"));
			m_registry.EmplaceComponent<MaterialComponent>(floor, glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(1.f), glm::vec3(1.f), 1.f, "Concrete");*/



			auto floor = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(floor, "Floor");
			m_registry.EmplaceComponent<TransformComponent>(floor, 0.0f, 0.0f, 0.0f).SetScale(glm::vec3(6.f, 1.f, 8.f));
			m_registry.EmplaceComponent<MeshComponent>(floor, m_ResourceManager->getMesh("Cube"));
			m_registry.EmplaceComponent<MaterialComponent>(floor, glm::vec3(0.3f, 0.3f, 0.3f), glm::vec3(1.f), glm::vec3(1.f), 1.f, "Concrete");

			auto wall = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(wall, "Wall");
			m_registry.EmplaceComponent<TransformComponent>(wall, 0.0f, 3.5f, -3.5f).SetScale(glm::vec3(6.f, 6.f, 1.f));
			m_registry.EmplaceComponent<MeshComponent>(wall, m_ResourceManager->getMesh("Cube"));
			m_registry.EmplaceComponent<MaterialComponent>(wall, glm::vec3(1.0f, 0.4f, 0.3f), glm::vec3(1.0f, 0.4f, 0.5f), glm::vec3(1.0f, 0.4f, 0.5f), 1.f, "Brick");
			m_registry.EmplaceComponent<DataBaseConnectComponent>(wall);

			auto obj1 = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(obj1, "Obj1");
			m_registry.EmplaceComponent<TransformComponent>(obj1, 1.5f, 1.f, 2.5f);
			m_registry.EmplaceComponent<MeshComponent>(obj1, m_ResourceManager->getMesh("Cube"));
			m_registry.EmplaceComponent<MaterialComponent>(obj1, glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), 1.f, "DiamondPlate");

			auto obj2 = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(obj2, "Obj2");
			m_registry.EmplaceComponent<TransformComponent>(obj2, 1.5f, 1.f, 1.0f);
			m_registry.EmplaceComponent<MeshComponent>(obj2, m_ResourceManager->getMesh("Cube"));
			m_registry.EmplaceComponent<MaterialComponent>(obj2, glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), 1.f, "DiamondPlate");

			auto obj3 = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(obj3, "Obj3");
			m_registry.EmplaceComponent<TransformComponent>(obj3, 1.5f, 1.f, -.5f);
			m_registry.EmplaceComponent<MeshComponent>(obj3, m_ResourceManager->getMesh("Cube"));
			m_registry.EmplaceComponent<MaterialComponent>(obj3, glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), 1.f, "DiamondPlate");

			auto obj4 = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(obj4, "Obj4");
			m_registry.EmplaceComponent<TransformComponent>(obj4, -1.5f, 1.f, 2.5f);
			m_registry.EmplaceComponent<MeshComponent>(obj4, m_ResourceManager->getMesh("Cube"));
			m_registry.EmplaceComponent<MaterialComponent>(obj4, glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), 1.f, "DiamondPlate");

			auto obj5 = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(obj5, "Obj5");
			m_registry.EmplaceComponent<TransformComponent>(obj5, -1.5f, 1.f, 1.0f);
			m_registry.EmplaceComponent<MeshComponent>(obj5, m_ResourceManager->getMesh("Cube"));
			m_registry.EmplaceComponent<MaterialComponent>(obj5, glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), 1.f, "DiamondPlate");
			m_registry.EmplaceComponent<DataBaseConnectComponent>(obj5);

			auto obj6 = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(obj6, "Obj6");
			m_registry.EmplaceComponent<TransformComponent>(obj6, -1.5f, 1.f, -.5f);
			m_registry.EmplaceComponent<MeshComponent>(obj6, m_ResourceManager->getMesh("Cube"));
			m_registry.EmplaceComponent<MaterialComponent>(obj6, glm::vec3(1.f), glm::vec3(1.f), glm::vec3(1.f), 1.f, "DiamondPlate");

			
			auto text = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(text, "Text");
			m_registry.EmplaceComponent<TransformComponent>(text, -1.5f, 3.f, -2.9f).SetScale(glm::vec3(2.f));
			m_registry.EmplaceComponent<TextComponent>(text, "Factory", m_ResourceManager->getTexture("Text"));
			
		}

		void Init() 
		{
			m_ResourceManager = ResourceManager::Instance;

			//Mashes
			m_ResourceManager->loadMesh("Cube", MeshGenerator::Cube(1.0f));
			

			m_ResourceManager->loadTexture("NoTexture", "Textures/notexture.png");

			//Textures
			m_ResourceManager->loadTexture("Cobble", "Textures/cobble.png");
			m_ResourceManager->loadTexture("Concrete", "Textures/concrete.png");
			m_ResourceManager->loadTexture("Brick", "Textures/brick.png");
			m_ResourceManager->loadTexture("Glass", "Textures/glass.png");
			m_ResourceManager->loadTexture("CorrodedMetal", "Textures/corrodedmetal.png");
			m_ResourceManager->loadTexture("DiamondPlate", "Textures/diamondplate.png");

			//Text
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
				system->FixedUpdate(*this, fixed_dt);
			}
		}

		void Update(float dt) 
		{
			for (auto& system : m_systems)
			{
				system->Update(*this, dt);
			}

			m_inputManager.Update();
		}

		void Render() 
		{
			for (auto& system : m_systems)
			{
				system->Render(*this);
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
