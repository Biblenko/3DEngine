#pragma once

#include "ISystem.h"

namespace Engine
{
    class ECS;

    class SpinAroundSystem : public ISystem
    {
    public:
        void Update(ECS& e, float dt) override;
    };
}