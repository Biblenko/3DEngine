#include "pch.h"
#include "MeshGenerator.h"

namespace Engine
{
    std::shared_ptr<Mesh> MeshGenerator::Cube(float size)
    {
        float h = size / 2.0f;

        std::vector<GLfloat> vertices = {
            // Координаты (X, Y, Z) | Нормали (Nx, Ny, Nz) | Текстуры (U, V)

            // Передняя грань (+Z)
            -h, -h,  h,    0.0f,  0.0f,  1.0f,    0.0f, 0.0f,
             h, -h,  h,    0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
             h,  h,  h,    0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
            -h,  h,  h,    0.0f,  0.0f,  1.0f,    0.0f, 1.0f,

            // Задняя грань (-Z)
             h, -h, -h,    0.0f,  0.0f, -1.0f,    0.0f, 0.0f,
            -h, -h, -h,    0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
            -h,  h, -h,    0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
             h,  h, -h,    0.0f,  0.0f, -1.0f,    0.0f, 1.0f,

             // Левая грань (-X)
             -h, -h, -h,  -1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
             -h, -h,  h,  -1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
             -h,  h,  h,  -1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
             -h,  h, -h,  -1.0f,  0.0f,  0.0f,    0.0f, 1.0f,

             // Правая грань (+X)
              h, -h,  h,   1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
              h, -h, -h,   1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
              h,  h, -h,   1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
              h,  h,  h,   1.0f,  0.0f,  0.0f,    0.0f, 1.0f,

              // Верхняя грань (+Y)
              -h,  h,  h,   0.0f,  1.0f,  0.0f,    0.0f, 0.0f,
               h,  h,  h,   0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
               h,  h, -h,   0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
              -h,  h, -h,   0.0f,  1.0f,  0.0f,    0.0f, 1.0f,

              // Нижняя грань (-Y)
              -h, -h, -h,   0.0f, -1.0f,  0.0f,    0.0f, 0.0f,
               h, -h, -h,   0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
               h, -h,  h,   0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
              -h, -h,  h,   0.0f, -1.0f,  0.0f,    0.0f, 1.0f,
        };

        std::vector<GLuint> indices;
        indices.reserve(36);

        for (int i = 0; i < 6; ++i)
        {
            GLuint offset = i * 4;

            indices.push_back(offset + 0);
            indices.push_back(offset + 1);
            indices.push_back(offset + 2);

            indices.push_back(offset + 2);
            indices.push_back(offset + 3);
            indices.push_back(offset + 0);
        }

        return std::make_shared<Mesh>(vertices, indices);
    }


    std::shared_ptr<Mesh> MeshGenerator::Sprite(const std::shared_ptr<Texture2D> pTexture, const std::string initialSubTextureName)
    {
        const float h = 0.5f;
        const float z = 0.0f;

        const auto& subTexture = pTexture->getSubTexture(initialSubTextureName);

        std::vector<GLfloat> vertexCoordinate =
        {
            // Позиция (X, Y, Z)   // Нормаль (Nx, Ny, Nz)  // UV (U, V)
            -h, -h,  z,            0.0f,  0.0f,  1.0f,      subTexture.leftBottomUV.x, subTexture.leftBottomUV.y,
             h, -h,  z,            0.0f,  0.0f,  1.0f,      subTexture.rightTopUV.x,   subTexture.leftBottomUV.y,
             h,  h,  z,            0.0f,  0.0f,  1.0f,      subTexture.rightTopUV.x,   subTexture.rightTopUV.y,
            -h,  h,  z,            0.0f,  0.0f,  1.0f,      subTexture.leftBottomUV.x, subTexture.rightTopUV.y,
        };


        std::vector<GLuint> indices = {
            0, 1, 2,
            2, 3, 0
        };

        return std::make_shared<Mesh>(vertexCoordinate, indices);
    }


    std::shared_ptr<Mesh> MeshGenerator::Text(const std::shared_ptr<Texture2D> pTexture, const std::string& text, float spacing = 0.0f)
    {
        std::vector<GLfloat> vertexCoordinates;
        std::vector<GLuint> indices;

        //
        // Rederve for text
        //
        vertexCoordinates.reserve(text.size() * 32);
        indices.reserve(text.size() * 6);

        float cursorX = 0.0f;
        const float z = 0.0f;

        for (size_t i = 0; i < text.size(); ++i)
        {
            char c = text[i];

            std::string charName(1, c);

            //if (c == ' ') {
            //    cursorX += 0.5f; // Space
            //    continue;
            //}

            const auto& subTexture = pTexture->getSubTexture(charName);

            float charWidth = 0.5f;
            float charHeight = 0.5f;

            float left = cursorX;
            float right = cursorX + charWidth;
            float bottom = -charHeight / 2.0f;
            float top = charHeight / 2.0f;

            vertexCoordinates.insert(vertexCoordinates.end(), {
                left,  bottom, z,   0.0f, 0.0f, 1.0f,   subTexture.leftBottomUV.x, subTexture.leftBottomUV.y,
                right, bottom, z,   0.0f, 0.0f, 1.0f,   subTexture.rightTopUV.x,   subTexture.leftBottomUV.y,
                right, top,    z,   0.0f, 0.0f, 1.0f,   subTexture.rightTopUV.x,   subTexture.rightTopUV.y,
                left,  top,    z,   0.0f, 0.0f, 1.0f,   subTexture.leftBottomUV.x, subTexture.rightTopUV.y
                });

            GLuint offset = static_cast<GLuint>(i * 4);
            indices.insert(indices.end(), {
                offset + 0, offset + 1, offset + 2,
                offset + 2, offset + 3, offset + 0
                });

            cursorX += charWidth + spacing;
        }

        return std::make_shared<Mesh>(vertexCoordinates, indices);
    }
}