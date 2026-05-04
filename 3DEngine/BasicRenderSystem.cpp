#include "pch.h"

#include "BasicRenderSystem.h"

#include "ECS.h"
#include "Components.h"
#include "ShaderProgram.h"

namespace Engine
{
	BasicRenderSystem::BasicRenderSystem()
	{
		m_shader = ResourceManager::Instance->loadShader("basicShader", "Shaders/basic.vert", "Shaders/basic.frag");

		m_view = glm::mat4(1.0f);
		m_projection = glm::mat4(1.0f);
	}

	void BasicRenderSystem::Init(ECS& e)
	{
		auto cameras = e.m_registry.Entities<CameraComponent>();

		if (!cameras.empty()) {
			e.active_camera = cameras[0];
			return;
		}

		EntityID camera = e.m_registry.CreateEntity();

		e.m_registry.EmplaceComponent<CameraComponent>		(camera);
		e.m_registry.EmplaceComponent<TransformComponent>	(camera, 0.0f, 0.0f, 0.0f);
		e.m_registry.EmplaceComponent<MovementComponent>	(camera);

		e.active_camera = camera;
	}

	void BasicRenderSystem::Render(ECS& e)
	{
		if (!m_shader) return;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		auto lights = e.m_registry.Entities<PointLightComponent>();
		if (!lights.empty())
		{
			EntityID light = lights[0];

			PointLightComponent* point_light = e.m_registry.GetComponent<PointLightComponent>(light);
			TransformComponent* transform = e.m_registry.GetComponent<TransformComponent>(light);

			m_shader->setVec3("lightColor", point_light->m_intensity * point_light->m_color);

			if (transform)
			{
				m_shader->setVec3("lightPos", transform->m_position);
			}
			else
			{
				m_shader->setVec3("lightPos", glm::vec3(0.0f, 128.0f, 0.0f));
			}
		}

		m_shader->setInt("material.diffuseMap", 0);

		e.m_registry.Each<MeshComponent, TransformComponent, MaterialComponent>([this](EntityID entityId, MeshComponent& meshComponent, TransformComponent& transformComponent, MaterialComponent& materialComponent)
			{
				glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transformComponent.GetModelMatrix())));
				m_shader->setMatrix3("normalMatrix", normalMatrix);

				m_shader->setMatrix4("model", transformComponent.GetModelMatrix());

				m_shader->setVec3("material.ambient", materialComponent.m_ambient);
				m_shader->setVec3("material.diffuse", materialComponent.m_diffuse);
				m_shader->setVec3("material.specular", materialComponent.m_specular);
				m_shader->setFloat("material.shininess", materialComponent.m_shininess);

				glActiveTexture(GL_TEXTURE0);
				if (materialComponent.m_diffuseMap) {
					materialComponent.m_diffuseMap->bind();
				}
				else {
					ResourceManager::Instance->getTexture("NoTexture")->bind();
				}


				meshComponent.Render();
			});
	}
}