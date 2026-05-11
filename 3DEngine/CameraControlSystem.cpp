#include "pch.h"

#include "CameraControlSystem.h"

#include "ECS.h"
#include "Components.h"

void Engine::CameraControlSystem::Init(ECS& e)
{
	auto& cameras = e.m_registry.Entities<CameraComponent>();

	if (cameras.empty()) return;

	auto& activeCameras = e.m_registry.Entities<ActiveCamera>();

	if (!activeCameras.empty()) return;

	e.m_registry.EmplaceComponent<ActiveCamera>(cameras.front());
}

void Engine::CameraControlSystem::Update(ECS& e, float dt)
{
	auto& activeCameras = e.m_registry.Entities<ActiveCamera>();

	if (activeCameras.empty()) return;

	TransformComponent* transform	= e.m_registry.GetComponent<TransformComponent>	(activeCameras.back());
	MovementComponent* movement		= e.m_registry.GetComponent<MovementComponent>	(activeCameras.back());

	auto& cameras = e.m_registry.Entities<CameraComponent>();

	if (!transform || !movement) return;

	//Camera rotating 
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


	//Calculate camera direction
	glm::vec3 targetDirection(0.0f);
	glm::vec3 forward = transform->GetForward();

	forward.y = 0;
	if (glm::length(forward) > 0.0001f) {
		forward = glm::normalize(forward);
	}

	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

	//Camera control
	if (e.m_inputManager.IsKeyHeld('W'))		targetDirection += forward;
	if (e.m_inputManager.IsKeyHeld('S'))		targetDirection -= forward;
	if (e.m_inputManager.IsKeyHeld('A'))		targetDirection -= right;
	if (e.m_inputManager.IsKeyHeld('D'))		targetDirection += right;

	if (e.m_inputManager.IsKeyHeld(VK_SPACE))	targetDirection += glm::vec3(0, 1, 0);
	if (e.m_inputManager.IsKeyHeld(VK_SHIFT))	targetDirection -= glm::vec3(0, 1, 0);

	if (glm::length(targetDirection) > 0.0f) {
		targetDirection = glm::normalize(targetDirection);
	}

	if (e.m_inputManager.IsKeyPressed('N')) NextCamera(e);

	//Apply movement
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

void Engine::CameraControlSystem::ChangeActiveCamera(ECS& e, EntityID newActiveCamera)
{
	auto& curentActiveCameras = e.m_registry.Entities<ActiveCamera>();

	for (const auto& activeCamera : curentActiveCameras)
	{
		e.m_registry.RemoveComponent<ActiveCamera>(activeCamera);
	}

	if (e.m_registry.HasComponent<CameraComponent>(newActiveCamera))
	{
		e.m_registry.EmplaceComponent<ActiveCamera>(newActiveCamera);
	}
}

void Engine::CameraControlSystem::NextCamera(ECS& e)
{
	auto& allCameras = e.m_registry.Entities<CameraComponent>();

	size_t camerasCount = allCameras.size();

	for (size_t i = 0; i < camerasCount; i++)
	{
		if (e.m_registry.HasComponent<ActiveCamera>(allCameras[i]))
		{
			ChangeActiveCamera(e, allCameras[(i + 1) % camerasCount]);
			return;
		}
	}

	if (camerasCount)
	{
		ChangeActiveCamera(e, allCameras[0]);
	}
}
