#pragma once

#include <GL/glew.h>
#include <vector>


namespace Engine
{
    class Mesh
    {
    private:
        GLuint m_VAO = 0;
        GLuint m_VBO = 0;
        GLuint m_EBO = 0;
        unsigned int m_indexCount = 0;

    public:
        

        Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices);

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(Mesh&& other) noexcept;

        ~Mesh();

        void render() const;
    };
}