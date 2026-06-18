#include "pch.h"
#include "SpinAroundSystem.h"
#include "ECS.h"
#include "Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> 

/// Оновлює позиції сутностей, що мають обертатися навколо інших об'єктів (цілей).
void Engine::SpinAroundSystem::Update(ECS& e, float deltaTime)
{
    if (deltaTime <= 0.0f) return; // Захист від нульового або від'ємного кроку часу

    // Ітерація по всіх сутностях, які мають компоненти SpinAroundComponent та TransformComponent
    e.m_registry.Each<SpinAroundComponent, TransformComponent>([&](EntityID entityId, SpinAroundComponent& spin, TransformComponent& transform)
        {
            // Отримуємо компонент трансформації цілі, навколо якої здійснюється обертання
            auto* targetTransform = e.m_registry.GetComponent<TransformComponent>(spin.m_targetEntity);
            if (!targetTransform) return; // Якщо ціль не має трансформації, пропускаємо обробку

            // Перевірка на нульовий офсет (використовуємо скалярний добуток як квадрат довжини для уникнення важкого sqrt)
            if (glm::dot(spin.m_currentOffset, spin.m_currentOffset) < 0.0001f)
            {
                // Спроба ініціалізувати офсет через поточну різницю позицій об'єкта та цілі
                spin.m_currentOffset = transform.m_position - targetTransform->m_position;

                // Якщо об'єкт і ціль знаходяться в одній точці, генеруємо штучний початковий вектор офсету
                if (glm::dot(spin.m_currentOffset, spin.m_currentOffset) < 0.0001f)
                {
                    // Вибір допоміжного вектора залежно від напрямку осі обертання (щоб уникнути колінеарності)
                    glm::vec3 helper = (std::abs(spin.m_axis.y) < 0.999f) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);

                    // Створюємо перпендикулярний вектор до осі обертання за допомогою векторного добутку
                    spin.m_currentOffset = glm::normalize(glm::cross(spin.m_axis, helper)) * spin.m_distance;
                }
            }

            // Розрахунок кута повороту в радіанах за поточний кадр
            float angleRad = glm::radians(spin.m_speed * deltaTime);

            // Створення кватерніона обертання навколо заданої осі на розрахований кут
            glm::quat rotationQuat = glm::angleAxis(angleRad, spin.m_axis);

            // Обертаємо вектор зміщення (офсету) за допомогою кватерніона
            spin.m_currentOffset = rotationQuat * spin.m_currentOffset;

            // Обов'язкова нормалізація та масштабування для запобігання накопиченню похибок округлення (floating-point drift)
            spin.m_currentOffset = glm::normalize(spin.m_currentOffset) * spin.m_distance;

            // Обчислюємо фінальну позицію об'єкта та оновлюємо її в компоненті трансформації
            transform.SetPosition(targetTransform->m_position + spin.m_currentOffset);
        });
}