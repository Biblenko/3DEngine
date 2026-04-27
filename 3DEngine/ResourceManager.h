#pragma once
#include <string>
#include <memory>
#include <unordered_map>

#include <GL/glew.h>


namespace Engine
{
    class ShaderProgram;
    class Texture2D;
    class Sprite;
    class Mesh;

    class ResourceManager
    {
    public:
        static ResourceManager* Instance;

        ResourceManager();
        ResourceManager(const std::string& executablePath);
        ~ResourceManager() = default;

        ResourceManager(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

        std::shared_ptr<ShaderProgram> loadShader(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath);
        std::shared_ptr<ShaderProgram> getShader(const std::string& shaderName) const;

        std::shared_ptr<Texture2D> loadTexture(const std::string& textureName, const std::string& fragmentPath);
        std::shared_ptr<Texture2D> getTexture(const std::string& textureName) const;

        std::shared_ptr<Sprite> loadSprite(
            const std::string& spriteName, 
            const std::string& textureName, 
            const std::string& shaderName, 
            const unsigned int spriteWidth, 
            const unsigned int spriteHeight);

        std::shared_ptr<Sprite> getSprite(const std::string& spriteName) const;

        std::shared_ptr<Mesh> loadMesh(const std::string& mashName, const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices);
        std::shared_ptr<Mesh> loadMesh(const std::string& mashName, std::shared_ptr<Mesh>);
        std::shared_ptr<Mesh> getMesh(const std::string& mashName) const;

    private:
        std::string getFileString(const std::string& relativePath) const;

        using ShaderProgramMap = std::unordered_map<std::string, std::shared_ptr<ShaderProgram>>;
        ShaderProgramMap m_shaderPrograms;

        using TextureMap = std::unordered_map<std::string, std::shared_ptr<Texture2D>>;
        TextureMap m_textures;

        using SpritesMap = std::unordered_map<std::string, std::shared_ptr<Sprite>>;
        SpritesMap m_sprites;

        using MeshMap = std::unordered_map<std::string, std::shared_ptr<Mesh>>;
        MeshMap m_meshes;

        std::string m_executablePath;
    };
}