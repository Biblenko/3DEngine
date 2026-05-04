#pragma once
#include <chrono>

namespace Engine {

    class Timer
    {
    public:
        Timer();

        void Reset();
        void Tick();

        float GetDeltaTime() const;
        float GetTotalTime() const;

        void SetTimeScale(float scale);
        float GetTimeScale() const;

    private:
        std::chrono::steady_clock::time_point m_startTime;
        std::chrono::steady_clock::time_point m_lastTime;

        float m_deltaTime;
        float m_timeScale;
    };

}