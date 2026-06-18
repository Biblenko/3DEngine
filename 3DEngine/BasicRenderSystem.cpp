#include "pch.h"

#include "BasicRenderSystem.h"

#include "ECS.h"
#include "Components.h"
#include "ShaderProgram.h"

namespace Engine
{
	/// Конструктор системи рендерингу. Завантажує базову шейдерну програму
	/// та ініціалізує початкові матриці вигляду і проєкції.
	BasicRenderSystem::BasicRenderSystem()
	{
		m_shader = ResourceManager::Instance->loadShader("basicShader", "Shaders/basic.vert", "Shaders/basic.frag");

		m_view = glm::mat4(1.0f);
		m_projection = glm::mat4(1.0f);
	}

	/// Ініціалізація системи. Виконує одноразове кешування локацій (ID) уніформ-змінних шейдера,
	/// щоб уникнути дорогих викликів glGetUniformLocation у кожному кадрі.
	void BasicRenderSystem::Init(ECS& e)
	{
		m_shaderUniform.model = m_shader->getUniformLocation("model");
		m_shaderUniform.normalMatrix = m_shader->getUniformLocation("normalMatrix");

		m_shaderUniform.view = m_shader->getUniformLocation("view");
		m_shaderUniform.projection = m_shader->getUniformLocation("projection");

		m_shaderUniform.viewPos = m_shader->getUniformLocation("viewPos");

		m_shaderUniform.lightColor = m_shader->getUniformLocation("lightColor");
		m_shaderUniform.lightPos = m_shader->getUniformLocation("lightPos");

		m_shaderUniform.material.ambient = m_shader->getUniformLocation("material.ambient");
		m_shaderUniform.material.diffuse = m_shader->getUniformLocation("material.diffuse");
		m_shaderUniform.material.specular = m_shader->getUniformLocation("material.specular");
		m_shaderUniform.material.shininess = m_shader->getUniformLocation("material.shininess");

		m_shaderUniform.material.diffuseMap = m_shader->getUniformLocation("material.diffuseMap");
	}

	/// Головна функція рендерингу кадру. Очищає буфери, налаштовує параметри
	/// камери та світла, після чого малює всі сутності, що мають меш і матеріал.
	void BasicRenderSystem::Render(ECS& e)
	{
		if (!m_shader) return;

		// Очищення буферів кольору та глибини перед малюванням нового кадру
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_shader->use();

		// Пошук активної камери в реєстрі ECS
		auto& activeCameras = e.m_registry.Entities<ActiveCamera>();

		CameraComponent* camera = nullptr;

		if (!activeCameras.empty())
		{
			camera = e.m_registry.GetComponent<CameraComponent>(activeCameras.back());
		}

		// Розрахунок та передача даних камери в уніформи шейдера
		if (camera)
		{
			m_projection = camera->m_projectionMatrix;

			TransformComponent* transform = e.m_registry.GetComponent<TransformComponent>(activeCameras.back());

			if (transform) {
				glm::vec3 camera_position = transform->m_position;
				glm::vec3 forward = transform->GetForward();

				// Розрахунок матриці вигляду (View Matrix) за позицією камери та вектором напрямку
				m_view = glm::lookAt(camera_position, camera_position + forward, glm::vec3(0, 1, 0));

				// Передача позиції камери для розрахунку дзеркального освітлення (Specular/Blinn-Phong)
				m_shader->setVec3(m_shaderUniform.viewPos, camera_position);
			}
		}

		m_shader->setMatrix4(m_shaderUniform.view, m_view);
		m_shader->setMatrix4(m_shaderUniform.projection, m_projection);

		// Обробка першого знайденого точкового джерела світла
		auto& lights = e.m_registry.Entities<PointLightComponent>();
		if (!lights.empty())
		{
			EntityID light = lights[0];

			PointLightComponent* point_light = e.m_registry.GetComponent<PointLightComponent>(light);
			TransformComponent* transform = e.m_registry.GetComponent<TransformComponent>(light);

			// Передача фінального кольору світла з урахуванням його інтенсивності
			m_shader->setVec3(m_shaderUniform.lightColor, point_light->m_intensity * point_light->m_color);

			if (transform)
			{
				m_shader->setVec3(m_shaderUniform.lightPos, transform->m_position);
			}
			else
			{
				// Дефолтна позиція світла високо вгорі, якщо у сутності світла немає компонента Transform
				m_shader->setVec3(m_shaderUniform.lightPos, glm::vec3(0.0f, 128.0f, 0.0f));
			}
		}

		// Вказуємо шейдеру, що дифузна карта прив'язана до текстурного юніту GL_TEXTURE0
		m_shader->setInt(m_shaderUniform.material.diffuseMap, 0);

		// Ітераційний цикл по всіх об'єктах, які мають геометрію, трансформацію та матеріал
		e.m_registry.Each<MeshComponent, TransformComponent, MaterialComponent>([this](EntityID entityId, MeshComponent& meshComponent, TransformComponent& transformComponent, MaterialComponent& materialComponent)
			{
				// Передача нормальної матриці (транспонована інверсія модельної матриці 3x3) для коректного розрахунку нормалей при масштабуванні об'єкта
				m_shader->setMatrix3(m_shaderUniform.normalMatrix, transformComponent.GetNormalMatrix());
				m_shader->setMatrix4(m_shaderUniform.model, transformComponent.GetModelMatrix());

				// Передача властивостей матеріалу об'єкта
				m_shader->setVec3(m_shaderUniform.material.ambient, materialComponent.m_ambient);
				m_shader->setVec3(m_shaderUniform.material.diffuse, materialComponent.m_diffuse);
				m_shader->setVec3(m_shaderUniform.material.specular, materialComponent.m_specular);
				m_shader->setFloat(m_shaderUniform.material.shininess, materialComponent.m_shininess);

				// Активація текстурного слоту та прив'язка дифузної карти (з fallback-текстурою «NoTexture», якщо карта відсутня)
				glActiveTexture(GL_TEXTURE0);
				if (materialComponent.m_diffuseMap) {
					materialComponent.m_diffuseMap->bind();
				}
				else {
					ResourceManager::Instance->getTexture("NoTexture")->bind();
				}

				// Виклик OpenGL команди отрисовки геометрії (DrawCall)
				meshComponent.Render();
			});
	}
}