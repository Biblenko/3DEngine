#pragma once

#include "ISystem.h"

#include <memory>

namespace Engine
{
	class ECS;

	class ShaderProgram;

	class BasicRenderSystem : public ISystem
	{
	public:
		std::shared_ptr<ShaderProgram> m_shader;

		glm::mat4 m_view;
		glm::mat4 m_projection;


		BasicRenderSystem();

		void Init(ECS& e) override;

		void Render(ECS& e) override;
	};
}
