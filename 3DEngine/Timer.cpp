#include "pch.h"
#include "Timer.h"

namespace Engine {

    Timer::Timer() : m_timeScale(1.0f), m_deltaTime(0.0f) {
        Reset();
    }

    void Timer::Reset() {
        m_startTime = std::chrono::steady_clock::now();
        m_lastTime = m_startTime;
    }

    void Timer::Tick() {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> delta = currentTime - m_lastTime;
        m_lastTime = currentTime;

        float rawDt = delta.count();
        if (rawDt > 0.1f) rawDt = 0.1f;

        m_deltaTime = rawDt * m_timeScale;
    }

    float Timer::GetDeltaTime() const {
        return m_deltaTime;
    }

    float Timer::GetTotalTime() const {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> total = currentTime - m_startTime;
        return total.count();
    }

    void Timer::SetTimeScale(float scale) {
        m_timeScale = scale;
    }

    float Timer::GetTimeScale() const {
        return m_timeScale;
    }
}