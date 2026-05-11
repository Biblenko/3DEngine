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
		}

		void CreateScene()
		{
			auto pointLight = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(pointLight, "PointLight");
			m_registry.EmplaceComponent<TransformComponent>(pointLight, 10.0f, 10.0f, 10.0f);
			m_registry.EmplaceComponent<PointLightComponent>(pointLight);



			for (size_t i = 0; i < 64 * 64; i++)
			{
				auto floor = m_registry.CreateEntity();

				//m_registry.EmplaceComponent<NameComponent>(floor, "Floor");
				m_registry.EmplaceComponent<TransformComponent>(floor, float(i % 64), 0.0f, float(i / 64));
				m_registry.EmplaceComponent<MeshComponent>(floor, m_ResourceManager->getMesh("Cube"));
				m_registry.EmplaceComponent<MaterialComponent>(floor, MaterialComponent::RedPlastic());
			}

			

			auto camera = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(camera, "Camera");
			m_registry.EmplaceComponent<CameraComponent>(camera);
			m_registry.EmplaceComponent<TransformComponent>(camera, 1.f, 1.f, 1.f);
			m_registry.EmplaceComponent<MovementComponent>(camera);

			auto camera2 = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(camera2, "Camera2");
			m_registry.EmplaceComponent<CameraComponent>(camera2);
			m_registry.EmplaceComponent<TransformComponent>(camera2, 10.f, 1.f, 10.f);
			m_registry.EmplaceComponent<MovementComponent>(camera2);

			auto text = m_registry.CreateEntity();

			m_registry.EmplaceComponent<NameComponent>(text, "Text");
			m_registry.EmplaceComponent<TransformComponent>(text, 0.0f, 1.0f, 0.0f);

			auto& spriteTransform = *m_registry.GetComponent<TransformComponent>(text);
			spriteTransform.SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
			spriteTransform.SetScale(glm::vec3(.2f, .2f, .2f));

			/*m_registry.EmplaceComponent<MeshComponent>(text, m_ResourceManager->getMesh("Sprite"));
			m_registry.EmplaceComponent<MaterialComponent>(text, 
				glm::vec3(1.f, 1.f, 1.f),
				glm::vec3(0.75164f, 0.60648f, 0.22648f),  
				glm::vec3(0.62828f, 0.5558f, 0.366065f),
				120.0f, m_ResourceManager->getTexture("Text"));*/

			m_registry.EmplaceComponent<TextComponent>(text, "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.", m_ResourceManager->getTexture("Text"));
		}

		void Init() 
		{
			m_ResourceManager = ResourceManager::Instance;

			//Mashes
			m_ResourceManager->loadMesh("Cube", MeshGenerator::Cube(1.0f));
			

			m_ResourceManager->loadTexture("NoTexture", "Textures/notexture.png");

			//Textures
			m_ResourceManager->loadTexture("Cobble", "Textures/cobble.png");
			m_ResourceManager->loadTexture("Brick", "Textures/brick.png");
			m_ResourceManager->loadTexture("Glass", "Textures/glass.png");

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
