#include "pch.h"
#include "ShaderProgram.h"

#include <iostream>
#include <stdexcept>
#include <cassert>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
    ShaderProgram::ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource)
    {
        GLuint vertexShaderID;
        if (!createShader(vertexSource, GL_VERTEX_SHADER, vertexShaderID))
        {
            throw std::runtime_error("VERTEX SHADER: Compilation failed");
        }

        GLuint fragmentShaderID;
        if (!createShader(fragmentSource, GL_FRAGMENT_SHADER, fragmentShaderID))
        {
            glDeleteShader(vertexShaderID);
            throw std::runtime_error("FRAGMENT SHADER: Compilation failed");
        }

        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertexShaderID);
        glAttachShader(m_ID, fragmentShaderID);
        glLinkProgram(m_ID);

        GLint success;
        glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLint length;
            glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &length);
            std::vector<GLchar> infoLog(length > 0 ? length : 1024);
            glGetProgramInfoLog(m_ID, length, nullptr, infoLog.data());

            glDeleteShader(vertexShaderID);
            glDeleteShader(fragmentShaderID);
            glDeleteProgram(m_ID);
            m_ID = 0;

            throw std::runtime_error(std::string("SHADER PROGRAM: Link-time error:\n") + infoLog.data());
        }

        m_isCompiled = true;

        // Отвязываем и удаляем шейдеры, они больше не нужны после успешной линковки
        glDetachShader(m_ID, vertexShaderID);
        glDetachShader(m_ID, fragmentShaderID);
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
    }

    ShaderProgram::~ShaderProgram()
    {
        if (m_ID != 0) {
            glDeleteProgram(m_ID);
        }
    }

    ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
        : m_isCompiled(other.m_isCompiled),
        m_ID(other.m_ID),
        m_uniformLocationCache(std::move(other.m_uniformLocationCache))
    {
        other.m_ID = 0;
        other.m_isCompiled = false;
    }

    ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
    {
        if (this != &other)
        {
            if (m_ID != 0) {
                glDeleteProgram(m_ID);
            }

            m_ID = other.m_ID;
            m_isCompiled = other.m_isCompiled;
            m_uniformLocationCache = std::move(other.m_uniformLocationCache);

            other.m_ID = 0;
            other.m_isCompiled = false;
        }
        return *this;
    }

    bool ShaderProgram::createShader(const std::string& source, GLenum shaderType, GLuint& shaderID)
    {
        shaderID = glCreateShader(shaderType);
        const char* code = source.c_str();
        glShaderSource(shaderID, 1, &code, nullptr);
        glCompileShader(shaderID);

        GLint success;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLint length;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
            std::vector<GLchar> infoLog(length > 0 ? length : 1024);
            glGetShaderInfoLog(shaderID, length, nullptr, infoLog.data());

            std::cerr << "SHADER COMPILATION ERROR:\n" << infoLog.data() << std::endl;
            glDeleteShader(shaderID);
            return false;
        }
        return true;
    }

    void ShaderProgram::use() const
    {
        assert(m_isCompiled && "Trying to use uncompiled shader program!");
        glUseProgram(m_ID);
    }

    void ShaderProgram::setInt(const std::string& name, GLint value)
    {
        assert(m_isCompiled);
        glUniform1i(getUniformLocation(name), value);
    }

    void ShaderProgram::setFloat(const std::string& name, GLfloat value)
    {
        assert(m_isCompiled);
        glUniform1f(getUniformLocation(name), value);
    }

    void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value)
    {
        assert(m_isCompiled);
        glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
    }

    void ShaderProgram::setMatrix4(const std::string& name, const glm::mat4& matrix)
    {
        assert(m_isCompiled);
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void ShaderProgram::setMatrix3(const std::string& name, const glm::mat3& matrix)
    {
        assert(m_isCompiled);
        glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    GLint ShaderProgram::getUniformLocation(const std::string& name)
    {
        assert(m_isCompiled);
        auto it = m_uniformLocationCache.find(name);
        if (it != m_uniformLocationCache.end())
        {
            return it->second;
        }

        GLint location = glGetUniformLocation(m_ID, name.c_str());
        m_uniformLocationCache[name] = location;
        return location;
    }
}