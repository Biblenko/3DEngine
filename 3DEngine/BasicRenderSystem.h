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
		struct ShaderUniform
		{
			GLint model = -1;
			GLint view = -1;
			GLint projection = -1;

			GLint viewPos = -1;

			GLint lightColor = -1;
			GLint lightPos = -1;

			GLint normalMatrix = -1;

			struct Material
			{
				GLint ambient = -1;
				GLint diffuse = -1;
				GLint specular = -1;
				GLint shininess = -1;

				GLint diffuseMap = -1;
			};

			Material material;
		};

		ShaderUniform m_shaderUniform;

		std::shared_ptr<ShaderProgram> m_shader;

		glm::mat4 m_view;
		glm::mat4 m_projection;


		BasicRenderSystem();

		void Init(ECS& e) override;

		void Render(ECS& e) override;
	};
}
