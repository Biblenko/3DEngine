#include "pch.h"
#include "ResourceManager.h"

#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Sprite.h"
#include "Mesh.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace Engine
{
    ResourceManager* ResourceManager::Instance = nullptr;

    ResourceManager::ResourceManager()
    {
        // ВНИМАНИЕ: current_path != executable path. 
        // Для реального пути к .exe в Windows используют GetModuleFileName.
        // Пока оставляем current_path, если так было задумано.
		m_executablePath = std::filesystem::current_path().string();
    }

    ResourceManager::ResourceManager(const std::string& executablePath)
        : m_executablePath(executablePath)
    {
    }

    std::shared_ptr<ShaderProgram> ResourceManager::loadShader(const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath)
    {
        // 1. Проверяем, не загружен ли уже такой шейдер (чтобы не делать лишнюю работу)
        auto it = m_shaderPrograms.find(shaderName);
        if (it != m_shaderPrograms.end())
        {
            return it->second;
        }

        // 2. Читаем исходники
        std::string vertexSource = getFileString(vertexPath);
        if (vertexSource.empty())
        {
            std::cerr << "ResourceManager: Ошибка загрузки vertex shader файла: " << vertexPath << std::endl;
            return nullptr;
        }

        std::string fragmentSource = getFileString(fragmentPath);
        if (fragmentSource.empty())
        {
            std::cerr << "ResourceManager: Ошибка загрузки fragment shader файла: " << fragmentPath << std::endl;
            return nullptr;
        }

        // 3. Пытаемся создать и скомпилировать шейдер
        try
        {
            auto shader = std::make_shared<ShaderProgram>(vertexSource, fragmentSource);

            // 4. Если всё успешно, кладем в кэш и возвращаем
            m_shaderPrograms[shaderName] = shader;
            return shader;
        }
        catch (const std::exception& e)
        {
            std::cerr << "ResourceManager: Ошибка компиляции шейдера '" << shaderName << "':\n" << e.what() << std::endl;
            return nullptr;
        }
    }

    std::shared_ptr<ShaderProgram> ResourceManager::getShader(const std::string& shaderName) const
    {
        auto it = m_shaderPrograms.find(shaderName);
        if (it != m_shaderPrograms.end())
        {
            return it->second;
        }

        std::cerr << "ResourceManager: Шейдер не найден в кэше: " << shaderName << std::endl;
        return nullptr;
    }

    std::shared_ptr<Texture2D> ResourceManager::loadTexture(const std::string& textureName, const std::string& filePath)
    {
        int channels = 0, width = 0, height = 0;

		stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load((m_executablePath + "/" + filePath).c_str(), &width, &height, &channels, 0);

        if (!data)
        {
            std::cerr << "ResourceManager: Ошибка загрузки текстуры: " << filePath << std::endl;
            return nullptr;
        }

        std::shared_ptr<Texture2D> newTexture = m_textures.emplace(textureName, std::make_shared<Texture2D>(width, height, data, channels)).first->second;

		stbi_image_free(data);
        
        return newTexture;
    }

    std::shared_ptr<Texture2D> ResourceManager::getTexture(const std::string& textureName) const
    {
        auto it = m_textures.find(textureName);
        if (it != m_textures.end())
        {
            return it->second;
        }

        std::cerr << "ResourceManager: Texture не найден в кэше: " << textureName << std::endl;
        return nullptr;
    }

    std::shared_ptr<Sprite> ResourceManager::loadSprite(
        const std::string& spriteName, 
        const std::string& textureName, 
        const std::string& shaderName, 
        const unsigned int spriteWidth, 
        const unsigned int spriteHeight,
        const std::string& subTextureName)
    {
        auto pTexture = getTexture(textureName);
        if (!pTexture) {
            std::cerr << "ResourceManager: Ошибка загрузки texture: " << textureName << "for the sprite:" << spriteName << std::endl;
            return nullptr;
        }

        auto pShader = getShader(shaderName);
        if (!pShader) {
            std::cerr << "ResourceManager: Ошибка загрузки shader: " << shaderName << "for the sprite:" << spriteName << std::endl;
            return nullptr;
        }

        std::shared_ptr<Sprite> sprite = m_sprites.emplace(spriteName, std::make_shared<Sprite>(
            pTexture,
            subTextureName,
            pShader, 
            glm::vec2(0.f, 0.f),
            glm::vec2(spriteWidth, spriteHeight))).first->second;


        return sprite;
    }

    std::shared_ptr<Sprite> ResourceManager::getSprite(const std::string& spriteName) const
    {
        auto it = m_sprites.find(spriteName);
        if (it != m_sprites.end())
        {
            return it->second;
        }

        std::cerr << "ResourceManager: Sprite не найден в кэше: " << spriteName << std::endl;
        return nullptr;
    }

    std::shared_ptr<Texture2D> ResourceManager::loadTextureAtlas(const std::string& textureName, 
                                                                const std::string& texturePath, 
                                                                std::vector<std::string> subTextures,
                                                                const unsigned int subTextureWidth, 
                                                                const unsigned int subTextureHeight)
    {
        auto pTexture = loadTexture(textureName, texturePath);
        if (pTexture)
        {
            const int textureWidth = pTexture->width();
            const int textureHeight = pTexture->height();

            int currentTextureOffsetX = 0;
            int currentTextureOffsetY = textureHeight;

            for (const auto& currentSubTextureName : subTextures)
            {
                // Защита: если мы спустились ниже картинки, прерываем цикл
                if (currentTextureOffsetY - subTextureHeight < 0) {
                    // Можно добавить лог: "Warning: not enough space in atlas for all subtextures!"
                    break;
                }

                glm::vec2 leftBottomUV(
                    (float)currentTextureOffsetX / textureWidth,
                    (float)(currentTextureOffsetY - subTextureHeight) / textureHeight
                );

                glm::vec2 rightTopUV(
                    (float)(currentTextureOffsetX + subTextureWidth) / textureWidth,
                    (float)currentTextureOffsetY / textureHeight
                );

                pTexture->addSubTexture(currentSubTextureName, leftBottomUV, rightTopUV);

                currentTextureOffsetX += subTextureWidth;

                // Если следующий спрайт уже не влезет в строку, переносим "каретку"
                if (currentTextureOffsetX + subTextureWidth > textureWidth)
                {
                    currentTextureOffsetX = 0;
                    currentTextureOffsetY -= subTextureHeight;
                }
            }
        }
        return pTexture;
    }

    std::shared_ptr<Mesh> ResourceManager::loadMesh(const std::string& mashName, const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices)
    {
        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(vertices, indices);
        m_meshes[mashName] = mesh;
        return mesh;
    }

    std::shared_ptr<Mesh> ResourceManager::loadMesh(const std::string& mashName, std::shared_ptr<Mesh> pmash)
    {
        m_meshes[mashName] = pmash;
        return pmash;
    }

    std::shared_ptr<Mesh> ResourceManager::getMesh(const std::string& mashName) const
    {
        auto it = m_meshes.find(mashName);
        if (it != m_meshes.end())
        {
            return it->second;
        }

        std::cerr << "ResourceManager: Mash не найден в кэше: " << mashName << std::endl;
        return nullptr;
    }

    std::string ResourceManager::getFileString(const std::string& relativePath) const
    {
        std::filesystem::path fullPath = std::filesystem::path(m_executablePath) / relativePath;

        std::ifstream file(fullPath, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "ResourceManager: Не удалось открыть файл: " << fullPath.string() << std::endl;
            return std::string{};
        }

        // Оптимизированное чтение файла за один раз
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        std::string contents(size, ' ');
        file.seekg(0, std::ios::beg);

        // Читаем блок данных. Используем contents.data() для C++17
        file.read(contents.data(), size);
        file.close();

        return contents;
    }
}