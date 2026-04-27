#pragma once

#include <memory>
#include "Mesh.h"

namespace Engine
{
    class MeshGenerator
    {
    public:

        static std::shared_ptr<Mesh> Cube(float size);
    };
}

