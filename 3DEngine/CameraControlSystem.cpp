#include "pch.h"

#include "CameraControlSystem.h"

#include "ECS.h"
#include "Components.h"

void Engine::CameraControlSystem::Update(ECS& e, float dt)
{
	if (!e.active_camera) return;

	TransformComponent* transform	= e.m_registry.GetComponent<TransformComponent>(e.active_camera);
	MovementComponent* movement		= e.m_registry.GetComponent<MovementComponent>(e.active_camera);

	if (!transform || !movement) return;

	if (e.m_inputManager.IsKeyHeld(VK_RBUTTON))
	{
		if (!m_isRotating) {
			m_isRotating = true;
		}

		int dx, dy;
		e.m_inputManager.GetMouseDelta(dx, dy);

		if (dx != 0 || dy != 0) {
			transform->m_rotation.y += dx * movement->m_mouseSensitivity;
			transform->m_rotation.x -= dy * movement->m_mouseSensitivity;

			if (transform->m_rotation.x > 89.0f) transform->m_rotation.x = 89.0f;
			if (transform->m_rotation.x < -89.0f) transform->m_rotation.x = -89.0f;

			transform->m_isDirty = true;
		}
	}
	else
	{
		if (m_isRotating) {
			m_isRotating = false;
		}
	}

	glm::vec3 targetDirection(0.0f);
	glm::vec3 forward = transform->GetForward();

	forward.y = 0;
	if (glm::length(forward) > 0.0001f) {
		forward = glm::normalize(forward);
	}

	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

	if (e.m_inputManager.IsKeyHeld('W'))		targetDirection += forward;
	if (e.m_inputManager.IsKeyHeld('S'))		targetDirection -= forward;
	if (e.m_inputManager.IsKeyHeld('A'))		targetDirection -= right;
	if (e.m_inputManager.IsKeyHeld('D'))		targetDirection += right;

	if (e.m_inputManager.IsKeyHeld(VK_SPACE))	targetDirection += glm::vec3(0, 1, 0);
	if (e.m_inputManager.IsKeyHeld(VK_SHIFT))	targetDirection -= glm::vec3(0, 1, 0);

	if (glm::length(targetDirection) > 0.0f) {
		targetDirection = glm::normalize(targetDirection);
	}

	glm::vec3 targetVelocity = targetDirection * movement->m_moveSpeed;

	float lerpFactor = movement->m_smoothness * dt;
	if (lerpFactor > 1.0f) lerpFactor = 1.0f;

	movement->m_velocity = glm::mix(movement->m_velocity, targetVelocity, lerpFactor);

	if (glm::length(movement->m_velocity) < 0.001f) {
		movement->m_velocity = glm::vec3(0.0f);
	}
	else {
		transform->m_position += movement->m_velocity * dt;
		transform->m_isDirty = true;
	}
}
