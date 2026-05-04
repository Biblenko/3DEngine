#pragma once

#include "ISystem.h"

namespace Engine {
	
	class CameraControlSystem : public ISystem
	{
		bool m_isRotating = false;

		void Update(ECS& e, float dt) override;
	};

}
