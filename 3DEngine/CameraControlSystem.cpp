#include "pch.h"

#include "CameraControlSystem.h"

#include "ECS.h"
#include "Components.h"

/// Ініціалізація системи. Якщо у реєстрі є камери, але жодна не активна, 
/// призначає першу знайдену камеру як активну.
void Engine::CameraControlSystem::Init(ECS& e)
{
	auto& cameras = e.m_registry.Entities<CameraComponent>();
	if (cameras.empty()) return;

	auto& activeCameras = e.m_registry.Entities<ActiveCamera>();
	if (!activeCameras.empty()) return; // Активна камера вже існує, ініціалізація не потрібна

	// Робимо першу камеру активною
	e.m_registry.EmplaceComponent<ActiveCamera>(cameras.front());
}

/// Основний цикл оновлення системи. Обробляє введення користувача,
/// розраховує вектори напрямку, обертання, згладжування переміщення та зміну камер.
void Engine::CameraControlSystem::Update(ECS& e, float dt)
{
	auto& activeCameras = e.m_registry.Entities<ActiveCamera>();
	if (activeCameras.empty()) return;

	// Отримуємо компоненти трансформації та руху для поточної активної камери
	TransformComponent* transform = e.m_registry.GetComponent<TransformComponent>(activeCameras.back());
	MovementComponent* movement = e.m_registry.GetComponent<MovementComponent>(activeCameras.back());

	auto& cameras = e.m_registry.Entities<CameraComponent>();

	if (!transform || !movement) return;

	// ──────────────────────────────────────────────────────────────────
	// Обертання камери (Mouse Look) за допомогою затиснутої правої кнопки миші
	// ──────────────────────────────────────────────────────────────────
	if (e.m_inputManager.IsKeyHeld(VK_RBUTTON))
	{
		if (!m_isRotating) {
			m_isRotating = true;
		}

		int dx, dy;
		e.m_inputManager.GetMouseDelta(dx, dy);

		if (dx != 0 || dy != 0) {
			// Оновлення кутів Ейлера (y — yaw / рискання, x — pitch / тангаж)
			transform->m_rotation.y += dx * movement->m_mouseSensitivity;
			transform->m_rotation.x -= dy * movement->m_mouseSensitivity;

			// Обмеження кута тангажу для запобігання перевертання камери (Gimbal Lock)
			if (transform->m_rotation.x > 89.0f) transform->m_rotation.x = 89.0f;
			if (transform->m_rotation.x < -89.0f) transform->m_rotation.x = -89.0f;

			transform->m_isDirty = true; // Сигналізуємо про необхідність перерахунку матриці трансформації
		}
	}
	else
	{
		if (m_isRotating) {
			m_isRotating = false;
		}
	}

	// ──────────────────────────────────────────────────────────────────
	// Розрахунок базисних векторів напрямку руху камери
	// ──────────────────────────────────────────────────────────────────
	glm::vec3 targetDirection(0.0f);
	glm::vec3 forward = transform->GetForward();

	// Проєктуємо вектор вперед на площину XZ (y = 0) для переміщення камери строго паралельно землі
	forward.y = 0;
	if (glm::length(forward) > 0.0001f) {
		forward = glm::normalize(forward);
	}

	// Розраховуємо вектор «вправо» за допомогою векторного добутку з глобальним вектором «вгору» (0, 1, 0)
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

	// ──────────────────────────────────────────────────────────────────
	// Опитування клавіатури для визначення напрямку руху
	// ──────────────────────────────────────────────────────────────────
	if (e.m_inputManager.IsKeyHeld('W'))		targetDirection += forward;
	if (e.m_inputManager.IsKeyHeld('S'))		targetDirection -= forward;
	if (e.m_inputManager.IsKeyHeld('A'))		targetDirection -= right;
	if (e.m_inputManager.IsKeyHeld('D'))		targetDirection += right;

	// Переміщення по вертикальній осі світу Y (незалежно від нахилу камери)
	if (e.m_inputManager.IsKeyHeld(VK_SPACE))	targetDirection += glm::vec3(0, 1, 0);
	if (e.m_inputManager.IsKeyHeld(VK_SHIFT))	targetDirection -= glm::vec3(0, 1, 0);

	// Нормалізуємо результуючий вектор, щоб швидкість по діагоналі не була вищою
	if (glm::length(targetDirection) > 0.0f) {
		targetDirection = glm::normalize(targetDirection);
	}

	// Перемикання між доступними камерами в сцені
	if (e.m_inputManager.IsKeyPressed('N')) NextCamera(e);
	if (e.m_inputManager.IsKeyPressed('B')) PreviousCamera(e);

	// ──────────────────────────────────────────────────────────────────
	// Застосування руху та інтерполяція швидкості (Smoothing / Lerp)
	// ──────────────────────────────────────────────────────────────────
	glm::vec3 targetVelocity = targetDirection * movement->m_moveSpeed;

	float lerpFactor = movement->m_smoothness * dt;
	if (lerpFactor > 1.0f) lerpFactor = 1.0f; // Запобігання овершуту при низькому FPS

	// Лінійна інтерполяція поточної швидкості до цільової для ефекту плавної зупинки/розгону
	movement->m_velocity = glm::mix(movement->m_velocity, targetVelocity, lerpFactor);

	// Епсилон-перевірка порогу швидкості для повної зупинки та уникнення мікрорухів (floating-point jitter)
	if (glm::length(movement->m_velocity) < 0.001f) {
		movement->m_velocity = glm::vec3(0.0f);
	}
	else {
		transform->m_position += movement->m_velocity * dt;
		transform->m_isDirty = true;
	}
}

/// Змінює активну камеру. Видаляє маркер ActiveCamera з усіх сутностей і додає його новій.
void Engine::CameraControlSystem::ChangeActiveCamera(ECS& e, EntityID newActiveCamera)
{
	auto& curentActiveCameras = e.m_registry.Entities<ActiveCamera>();

	// Видаляємо компонент активності з попередніх камер (зазвичай вона одна)
	for (const auto& activeCamera : curentActiveCameras)
	{
		e.m_registry.RemoveComponent<ActiveCamera>(activeCamera);
	}

	// Призначаємо нову активну камеру, якщо у сутності є CameraComponent
	if (e.m_registry.HasComponent<CameraComponent>(newActiveCamera))
	{
		e.m_registry.EmplaceComponent<ActiveCamera>(newActiveCamera);
	}
}

/// Перемикає активність на наступну камеру в списку реєстратора зациклено (за годинниковою стрілкою).
void Engine::CameraControlSystem::NextCamera(ECS& e)
{
	auto& allCameras = e.m_registry.Entities<CameraComponent>();
	size_t camerasCount = allCameras.size();

	if (camerasCount == 0) return;

	for (size_t i = 0; i < camerasCount; i++)
	{
		if (e.m_registry.HasComponent<ActiveCamera>(allCameras[i]))
		{
			// Формула кільцевого буфера для інкременту з урахуванням переповнення масиву
			size_t nextIndex = (i + camerasCount + 1) % camerasCount;
			ChangeActiveCamera(e, allCameras[nextIndex]);
			return;
		}
	}

	ChangeActiveCamera(e, allCameras[0]);
}

/// Перемикає активність на попередню камеру в списку реєстратора зациклено (проти годинникової стрілки).
void Engine::CameraControlSystem::PreviousCamera(ECS& e)
{
	auto& allCameras = e.m_registry.Entities<CameraComponent>();
	size_t camerasCount = allCameras.size();

	if (camerasCount == 0) return;

	for (size_t i = 0; i < camerasCount; i++)
	{
		if (e.m_registry.HasComponent<ActiveCamera>(allCameras[i]))
		{
			// Формула кільцевого буфера для декременту (додавання camerasCount запобігає від'ємним значенням i - 1)
			size_t prevIndex = (i + camerasCount - 1) % camerasCount;
			ChangeActiveCamera(e, allCameras[prevIndex]);
			return;
		}
	}

	ChangeActiveCamera(e, allCameras[0]);
}