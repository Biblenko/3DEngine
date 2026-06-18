#include "pch.h"
#include "TextRenderSystem.h"

#include "ECS.h"
#include "Components.h"
#include "ShaderProgram.h"

namespace Engine
{
	/// Конструктор системи рендерингу тексту. Завантажує шейдер, 
	/// призначений для спрайтів та тексту, та обнуляє матриці.
	TextRenderSystem::TextRenderSystem()
	{
		m_shader = ResourceManager::Instance->loadShader("spriteShader", "Shaders/spriteTest.vert", "Shaders/spriteTest.frag");

		m_view = glm::mat4(1.0f);
		m_projection = glm::mat4(1.0f);
	}

	/// Метод ініціалізації системи. Наразі залишається порожнім, 
	/// оскільки локації уніформ не кешуються і викликаються за рядковими іменами безпосередньо в Render().
	void Engine::TextRenderSystem::Init(ECS& e)
	{

	}

	/// Головна функція рендерингу текстових компонентів. Налаштовує матриці камери 
	/// та послідовно відмальовує кожен текстовий меш із використанням атласу шрифту.
	void Engine::TextRenderSystem::Render(ECS& e)
	{
		if (!m_shader) return;

		m_shader->use();

		// Пошук активної камери в реєстрі ECS
		auto& activeCameras = e.m_registry.Entities<ActiveCamera>();

		CameraComponent* camera = nullptr;

		if (!activeCameras.empty())
		{
			camera = e.m_registry.GetComponent<CameraComponent>(activeCameras.back());
		}

		// Розрахунок та підготовка матриць камери
		if (camera)
		{
			m_projection = camera->m_projectionMatrix;

			TransformComponent* transform = e.m_registry.GetComponent<TransformComponent>(activeCameras.back());

			if (transform) {
				glm::vec3 camera_position = transform->m_position;
				glm::vec3 forward = transform->GetForward();

				// Розрахунок матриці вигляду (View Matrix)
				m_view = glm::lookAt(camera_position, camera_position + forward, glm::vec3(0, 1, 0));

				// Передача позиції камери в шейдер (може знадобитися для специфічних ефектів або освітлення тексту)
				m_shader->setVec3("viewPos", camera_position);
			}
		}

		// Передача глобальних матриць сцени в уніформи шейдера
		m_shader->setMatrix4("view", m_view);
		m_shader->setMatrix4("projection", m_projection);

		// Ітерація по всіх сутностях, які мають текст (TextComponent) та позицію у просторі (TransformComponent)
		e.m_registry.Each<TextComponent, TransformComponent>([this](EntityID entityId, TextComponent& textComponent, TransformComponent& transformComponent)
			{
				// Передача модельної матриці (позиція, масштаб, поворот текстового блоку)
				m_shader->setMatrix4("model", transformComponent.GetModelMatrix());

				// Активація текстурного слоту GL_TEXTURE0 для атласу шрифту
				glActiveTexture(GL_TEXTURE0);

				// Встановлення кольору тексту та прив'язка текстури з символами (Font Atlas)
				m_shader->setVec3("textColor", textComponent.color);
				textComponent.fontAtlas->bind();

				// Відрисовка згенерованого мешу тексту (DrawCall)
				textComponent.textMesh->render();
			});
	}
}