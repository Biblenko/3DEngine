#include "pch.h"
#include "Mesh.h"

#include <GL/glew.h>

namespace Engine
{
    Mesh::Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices)
        : m_indexCount(static_cast<unsigned int>(indices.size()))
    {
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        glGenBuffers(1, &m_VBO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &m_EBO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        // Общий шаг (stride) теперь 8 float (3 позиция + 3 нормаль + 2 текстура)
        GLsizei stride = 8 * sizeof(GLfloat);

        // Атрибут 0: Координаты (X, Y, Z) - 3 float, смещение 0
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

        // Атрибут 1: Нормали (Nx, Ny, Nz) - 3 float, смещение 3 float
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));

        // Атрибут 2: Текстурные координаты (U, V) - 2 float, смещение 6 float
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));

        glBindVertexArray(0);
    }

    Mesh::Mesh(Mesh&& other) noexcept
        : m_VAO(other.m_VAO), m_VBO(other.m_VBO), m_EBO(other.m_EBO), m_indexCount(other.m_indexCount)
    {
        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_EBO = 0;
        other.m_indexCount = 0;
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this != &other)
        {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_VBO);
            glDeleteBuffers(1, &m_EBO);

            m_VAO = other.m_VAO;
            m_VBO = other.m_VBO;
            m_EBO = other.m_EBO;
            m_indexCount = other.m_indexCount;

            other.m_VAO = 0;
            other.m_VBO = 0;
            other.m_EBO = 0;
            other.m_indexCount = 0;
        }
        return *this;
    }

    Mesh::~Mesh()
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }

    void Mesh::render() const
    {
        glBindVertexArray(m_VAO);

        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);

        //glBindVertexArray(0);
    }
}