#pragma once

#include "ResourceManager.h"

#include "MeshGenerator.h"

#include "EventDispatcher.h"
#include "InputManager.h"

#include "ISystem.h"
#include "Registry.h"
#include "Components.h"

#include "BasicRenderSystem.h"
#include "CameraControlSystem.h"



namespace Engine {
	class ECS
	{
	public:
		EntityID active_camera;

		ResourceManager *m_ResourceManager;
		InputManager m_inputManager;
		Registry m_registry;

		std::vector<std::unique_ptr<ISystem>> m_systems;

		EventDispatcher m_events;

		void LoadAllSystems()
		{
			AddSystem<BasicRenderSystem>();
			AddSystem<CameraControlSystem>();
		}

		void CreateScene()
		{
			auto pointLight = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(pointLight, "PointLight");
			m_registry.EmplaceComponent<TransformComponent>(pointLight, 10.0f, 10.0f, 10.0f);
			m_registry.EmplaceComponent<PointLightComponent>(pointLight);



			auto floor = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(floor, "Floor");
			m_registry.EmplaceComponent<TransformComponent>(floor, 0.0f, 1.0f, 0.0f);

			auto& floorTransform = *m_registry.GetComponent<TransformComponent>(floor);
			floorTransform.SetScale(glm::vec3(64.0f, 1.0f, 64.0f));

			m_registry.EmplaceComponent<MeshComponent>(floor, m_ResourceManager->getMesh("Cube"));
			m_registry.EmplaceComponent<MaterialComponent>(floor, MaterialComponent::RedPlastic());



			auto camera = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(camera, "Camera1");
			m_registry.EmplaceComponent<TransformComponent>(camera, 4.0f, 4.0f, 4.0f);
			m_registry.EmplaceComponent<CameraComponent>(camera);
			m_registry.EmplaceComponent<MovementComponent>(camera, 16.f, 32.f, 0.1f);
		}

		void Init() 
		{
			m_ResourceManager = ResourceManager::Instance;

			m_ResourceManager->loadMesh("Cube", MeshGenerator::Cube(1.0f));

			m_ResourceManager->loadTexture("NoTexture", "Textures/notexture.png");

			m_ResourceManager->loadTexture("Cobble", "Textures/cobble.png");
			m_ResourceManager->loadTexture("Brick", "Textures/brick.png");
			m_ResourceManager->loadTexture("Glass", "Textures/glass.png");

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
