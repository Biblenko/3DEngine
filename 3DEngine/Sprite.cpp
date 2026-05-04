#include "pch.h"
#include "Sprite.h"


#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "Texture2D.h"


Engine::Sprite::Sprite(	const std::shared_ptr<Texture2D> pTexture, 
						const std::string initialSubTextureName,
						const std::shared_ptr<ShaderProgram> pShaderProgram, 
						const glm::vec2& position, 
						const glm::vec2& scale, 
						const float rotation)
	: m_pTexture(pTexture)
	, m_pShaderProgram(pShaderProgram)
	, m_position(position)
	, m_scale(scale)
	, m_rotation(rotation)
{
	GLfloat vertexCoordinate[] =
	{
		0.f, 0.f,
		1.f, 0.f,
		1.f, 1.f,

		1.f, 1.f,
		0.f, 1.f,
		0.f, 0.f
	};

	const auto& subTexture = pTexture->getSubTexture(initialSubTextureName);

	GLfloat ftextureCoordinate[] =
	{
		0.f, 0.f,
		1.f, 0.f,
		1.f, 1.f,

		1.f, 1.f,
		0.f, 1.f,
		0.f, 0.f
	};

	GLfloat textureCoordinate[] =
	{
		subTexture.leftBottomUV.x, subTexture.leftBottomUV.y,
		subTexture.rightTopUV.x, subTexture.leftBottomUV.y,
		subTexture.rightTopUV.x, subTexture.rightTopUV.y,

		subTexture.rightTopUV.x, subTexture.rightTopUV.y,
		subTexture.leftBottomUV.x, subTexture.rightTopUV.y,
		subTexture.leftBottomUV.x, subTexture.leftBottomUV.y
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexCoordinate), &vertexCoordinate, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &m_textureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_textureVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinate), &textureCoordinate, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Engine::Sprite::~Sprite()
{
	glDeleteBuffers(1, &m_vertexVBO);
	glDeleteBuffers(1, &m_textureVBO);
	glDeleteVertexArrays(1, &m_VAO);
}

void Engine::Sprite::render() const
{
	m_pShaderProgram->use();

	glm::mat4 model(1.f);

	model = glm::translate(model, glm::vec3(m_position, 0.f));
	model = glm::translate(model, glm::vec3(0.5f * m_scale.x, 0.5f * m_scale.y, 0.f));
	model = glm::rotate(model, glm::radians(m_rotation), glm::vec3(0.f, 0.f, 1.f));
	model = glm::translate(model, glm::vec3(-0.5f * m_scale.x, -0.5f * m_scale.y, 0.f));
	model = glm::scale(model, glm::vec3(m_scale, 1.f));

	glBindVertexArray(m_VAO);
	m_pShaderProgram->setMatrix4("model", model);

	glActiveTexture(GL_TEXTURE0);
	m_pTexture->bind();

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Engine::Sprite::setPosition(const glm::vec2& position)
{
	m_position = position;
}

void Engine::Sprite::setScale(const glm::vec2& scale)
{
	m_scale = scale;
}

void Engine::Sprite::setRotation(const float rotation)
{
	m_rotation = rotation;
}
