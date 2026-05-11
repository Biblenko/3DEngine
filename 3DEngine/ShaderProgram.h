#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Engine
{
    class ShaderProgram {
    public:
        ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
        ~ShaderProgram();

        ShaderProgram(const ShaderProgram&) = delete;
        ShaderProgram& operator=(const ShaderProgram&) = delete;

        ShaderProgram(ShaderProgram&& other) noexcept;
        ShaderProgram& operator=(ShaderProgram&& other) noexcept;

        bool isCompiled() const { return m_isCompiled; }
        void use() const;

        void setInt(const std::string& name, GLint value);
        void setFloat(const std::string& name, GLfloat value);
        void setVec3(const std::string& name, const glm::vec3& value);
        void setMatrix4(const std::string& name, const glm::mat4& matrix);
        void setMatrix3(const std::string& name, const glm::mat3& matrix);

        void setInt(GLint location, GLint value);
        void setFloat(GLint location, GLfloat value);
        void setVec3(GLint location, const glm::vec3& value);
        void setMatrix4(GLint location, const glm::mat4& matrix);
        void setMatrix3(GLint location, const glm::mat3& matrix);

        GLint getUniformLocation(const std::string& name);

    private:
        bool createShader(const std::string& source, GLenum shaderType, GLuint& shaderID);

        bool m_isCompiled = false;
        GLuint m_ID = 0;

        std::unordered_map<std::string, GLint> m_uniformLocationCache;
    };
}