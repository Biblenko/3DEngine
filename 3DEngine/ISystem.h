#pragma once

namespace Engine
{
    class ECS;

    class ISystem {
    public:
        virtual ~ISystem() = default;

        virtual void Init(ECS& e) {}
        virtual void Shutdown(ECS& e) {}

        virtual void FixedUpdate(ECS& e, float fixed_dt) {}
        virtual void Update(ECS& e, float dt) {}
        virtual void Render(ECS& e) {}

        void SetEnabled(bool state) { m_enabled = state; }
        bool IsEnabled() const { return m_enabled; }

    private:
        bool m_enabled = true;
    };
}