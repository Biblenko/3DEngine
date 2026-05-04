#include "pch.h"
#include "SpriteRenderSystem.h"

#include "ECS.h"
#include "Components.h"
#include "ShaderProgram.h"

Engine::SpriteRenderSystem::SpriteRenderSystem()
{
	m_shader = ResourceManager::Instance->loadShader("spriteShader", "Shaders/spriteTest.vert", "Shaders/spritTest.frag");

	m_view = glm::mat4(1.0f);
	m_projection = glm::mat4(1.0f);
}

void Engine::SpriteRenderSystem::Init(ECS& e)
{
	auto cameras = e.m_registry.Entities<CameraComponent>();

	if (!cameras.empty()) {
		e.active_camera = cameras[0];
		return;
	}

	EntityID camera = e.m_registry.CreateEntity();

	e.m_registry.EmplaceComponent<CameraComponent>(camera);
	e.m_registry.EmplaceComponent<TransformComponent>(camera, 0.0f, 0.0f, 0.0f);
	e.m_registry.EmplaceComponent<MovementComponent>(camera);

	e.active_camera = camera;
}

void Engine::SpriteRenderSystem::Render(ECS& e)
{
	if (!m_shader) return;

	m_shader->use();

	CameraComponent* camera = e.m_registry.GetComponent<CameraComponent>(e.active_camera);

	if (camera)
	{
		m_projection = camera->m_projectionMatrix;

		TransformComponent* transform = e.m_registry.GetComponent<TransformComponent>(e.active_camera);

		if (transform) {
			glm::vec3 camera_position = transform->m_position;
			glm::vec3 forward = transform->GetForward();

			m_view = glm::lookAt(camera_position, camera_position + forward, glm::vec3(0, 1, 0));

			m_shader->setVec3("viewPos", camera_position);
		}
	}

	m_shader->setMatrix4("view", m_view);
	m_shader->setMatrix4("projection", m_projection);

	e.m_registry.Each<SpriteComponent, TransformComponent, MaterialComponent>([this](EntityID entityId, SpriteComponent& spriteComponent, TransformComponent& transformComponent, MaterialComponent& materialComponent)
		{
			m_shader->setMatrix4("model", transformComponent.GetModelMatrix());

			glActiveTexture(GL_TEXTURE0);
			
			spriteComponent.m_sprite->render();
		});
}
