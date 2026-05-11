#pragma once

#include <memory>
#include "Mesh.h"
#include "Texture2D.h"

namespace Engine
{
    class MeshGenerator
    {
    public:

        static std::shared_ptr<Mesh> Cube(float size);
        static std::shared_ptr<Mesh> Sprite(const std::shared_ptr<Texture2D> pTexture, const std::string initialSubTextureName);
        static std::shared_ptr<Mesh> Text(const std::shared_ptr<Texture2D> pTexture, const std::string& text, float spacing);
    };
}

