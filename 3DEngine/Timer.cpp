#include "pch.h"
#include "Timer.h"

namespace Engine {

    /// Конструктор. Ініціалізує початкові значення та скидає таймер на поточний момент.
    Timer::Timer() : m_timeScale(1.0f), m_deltaTime(0.0f) {
        Reset();
    }

    /// Повністю скидає початкову точку відліку та час останнього кадру на поточний момент часу.
    void Timer::Reset() {
        m_startTime = std::chrono::steady_clock::now();
        m_lastTime = m_startTime;
    }

    /// Оновлює дельту часу (Delta Time) між поточним та попереднім кадрами.
    /// Має захист від великих стрибків часу та підтримує масштабування.
    void Timer::Tick() {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> delta = currentTime - m_lastTime;
        m_lastTime = currentTime;

        float rawDt = delta.count();

        // Жорстке обмеження макс. кроку (100 мс), щоб запобігти провалюванню крізь текстури 
        // або стрибкам фізики після підвисання програми чи зупинки на брейкпоінті під час дебагу.
        if (rawDt > 0.1f) rawDt = 0.1f;

        // Розрахунок фінальної дельти з урахуванням коефіцієнта швидкості (наприклад, для slow-mo)
        m_deltaTime = rawDt * m_timeScale;
    }

    /// Повертає обчислену дельту часу (в секундах) для поточного кадру з урахуванням TimeScale.
    float Timer::GetDeltaTime() const {
        return m_deltaTime;
    }

    /// Розраховує та повертає сумарний час у секундах, який минув з моменту запуску/скидання таймера.
    float Timer::GetTotalTime() const {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> total = currentTime - m_startTime;
        return total.count();
    }

    /// Встановлює масштаб часу. Значення < 1.0 сповільнюють час (slow-mo), > 1.0 — прискорюють.
    void Timer::SetTimeScale(float scale) {
        m_timeScale = scale;
    }

    /// Повертає поточний коефіцієнт масштабування часу.
    float Timer::GetTimeScale() const {
        return m_timeScale;
    }
}