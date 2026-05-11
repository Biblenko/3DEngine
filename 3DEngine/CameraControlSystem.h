#pragma once

#include "ISystem.h"
#include "Registry.h"

namespace Engine {
	
	class CameraControlSystem : public ISystem
	{
		bool m_isRotating = false;

		void Init(ECS& e) override;

		void Update(ECS& e, float dt) override;

		void ChangeActiveCamera(ECS& e, EntityID newActiveCamera);

		void NextCamera(ECS& e);
	};

}
