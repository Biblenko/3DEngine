#pragma once
#include <array>
#include <windows.h>
#include <windowsx.h>

namespace Engine {

    // =========================================================================
    //  InputManager (Менеджер введення)
    //  Обробляє системні повідомлення Win32 API для відстеження стану 
    //  клавіатури та миші (абсолютні позиції, дельти, коліщатко).
    // =========================================================================
    class InputManager {
    private:
        std::array<bool, 256> m_keys;     // Поточний стан усіх віртуальних клавіш (true = натиснуто)
        std::array<bool, 256> m_prevKeys; // Стан клавіш на попередньому кадрі (для визначення одиночних кліків)

        bool m_firstMouse = true;         // Прапорець першого руху миші (запобігає стрибку дельти при старті додатка)
        int m_mouseX = 0;                 // Поточна абсолютна X-координата курсора у вікні
        int m_mouseY = 0;                 // Поточна абсолютна Y-координата курсора у вікні
        int m_mouseDeltaX = 0;            // Накопичене зміщення миші по осі X за поточний кадр
        int m_mouseDeltaY = 0;            // Накопичене зміщення миші по осі Y за поточний кадр
        int m_mouseScroll = 0;            // Накопичене значення прокручування коліщатка миші

    public:
        /// Конструктор. Заповнює масиви станів клавіш початковими значеннями false.
        InputManager() {
            m_keys.fill(false);
            m_prevKeys.fill(false);
        }

        /// Обробник системних повідомлень вікна (WndProc). 
        /// Записує сирі дані від ОС у внутрішні структури класу.
        void ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
            switch (uMsg) {

                // Клавіатура: натискання та відпускання клавіш
            case WM_KEYDOWN:
                if (wParam < 256) m_keys[wParam] = true;
                break;
            case WM_KEYUP:
                if (wParam < 256) m_keys[wParam] = false;
                break;

                // Рух миші
            case WM_MOUSEMOVE: {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);

                if (m_firstMouse) {
                    m_mouseX = x; m_mouseY = y;
                    m_firstMouse = false;
                }

                // Використовується оператор +=, оскільки Win32 може надсилати 
                // кілька повідомлень WM_MOUSEMOVE за один кадр додатка
                m_mouseDeltaX += x - m_mouseX;
                m_mouseDeltaY += y - m_mouseY;
                m_mouseX = x; m_mouseY = y;
                break;
            }

                             // Коліщатко миші
            case WM_MOUSEWHEEL:
                m_mouseScroll += GET_WHEEL_DELTA_WPARAM(wParam);
                break;

                // Мапінг стандартних кнопок миші на віртуальні коди клавіш Windows (VK_*)
            case WM_LBUTTONDOWN: m_keys[VK_LBUTTON] = true; break;
            case WM_LBUTTONUP:   m_keys[VK_LBUTTON] = false; break;
            case WM_RBUTTONDOWN: m_keys[VK_RBUTTON] = true; break;
            case WM_RBUTTONUP:   m_keys[VK_RBUTTON] = false; break;
            case WM_MBUTTONDOWN: m_keys[VK_MBUTTON] = true; break;
            case WM_MBUTTONUP:   m_keys[VK_MBUTTON] = false; break;

                // Додаткові бічні кнопки миші (X-buttons)
            case WM_XBUTTONDOWN:
                if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) m_keys[VK_XBUTTON1] = true;
                else                                         m_keys[VK_XBUTTON2] = true;
                break;
            case WM_XBUTTONUP:
                if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) m_keys[VK_XBUTTON1] = false;
                else                                         m_keys[VK_XBUTTON2] = false;
                break;
            }
        }

        /// Перевіряє, чи утримується клавіша натиснутою в поточному кадрі.
        bool IsKeyHeld(int key) const {
            if (key < 0 || key >= 256) return false;
            return m_keys[key];
        }

        /// Перевіряє, чи була клавіша натиснута саме в цьому кадрі (тригер по передньому фронту).
        /// Повертає true лише в тому кадрі, коли стан змінився з відпущеного на натиснутий.
        bool IsKeyPressed(int key) const {
            if (key < 0 || key >= 256) return false;
            return m_keys[key] && !m_prevKeys[key];
        }

        /// Записує накопичені зміщення миші за кадр у вихідні змінні dx та dy.
        void GetMouseDelta(int& dx, int& dy) const {
            dx = m_mouseDeltaX;
            dy = m_mouseDeltaY;
        }

        /// Синхронізує стани кадрів. Має викликатися наприкінці кожного ігрового циклу.
        /// Копіює поточний стан клавіш у буфер попереднього кадру та обнуляє дельти миші.
        void Update() {
            m_prevKeys = m_keys;
            m_mouseDeltaX = 0;
            m_mouseDeltaY = 0;
            m_mouseScroll = 0;
        }
    };
}