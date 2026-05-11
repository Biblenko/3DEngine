#include "pch.h"
#include "TextRenderSystem.h"

#include "ECS.h"
#include "Components.h"
#include "ShaderProgram.h"

Engine::TextRenderSystem::TextRenderSystem()
{
	m_shader = ResourceManager::Instance->loadShader("spriteShader", "Shaders/spriteTest.vert", "Shaders/spriteTest.frag");

	m_view = glm::mat4(1.0f);
	m_projection = glm::mat4(1.0f);
}

void Engine::TextRenderSystem::Init(ECS& e)
{

}

void Engine::TextRenderSystem::Render(ECS& e)
{
	if (!m_shader) return;

	m_shader->use();

	//Search for active camera
	auto& activeCameras = e.m_registry.Entities<ActiveCamera>();

	CameraComponent* camera = nullptr;

	if (!activeCameras.empty())
	{
		camera = e.m_registry.GetComponent<CameraComponent>(activeCameras.back());
	}

	if (camera)
	{
		m_projection = camera->m_projectionMatrix;

		TransformComponent* transform = e.m_registry.GetComponent<TransformComponent>(activeCameras.back());

		if (transform) {
			glm::vec3 camera_position = transform->m_position;
			glm::vec3 forward = transform->GetForward();

			m_view = glm::lookAt(camera_position, camera_position + forward, glm::vec3(0, 1, 0));

			m_shader->setVec3("viewPos", camera_position);
		}
	}

	m_shader->setMatrix4("view", m_view);
	m_shader->setMatrix4("projection", m_projection);

	e.m_registry.Each<TextComponent, TransformComponent>([this](EntityID entityId, TextComponent& textComponent, TransformComponent& transformComponent)
		{
			m_shader->setMatrix4("model", transformComponent.GetModelMatrix());

			glActiveTexture(GL_TEXTURE0);
			m_shader->setVec3("textColor", textComponent.color);
			textComponent.fontAtlas->bind();
			textComponent.textMesh->render();
		});
}
