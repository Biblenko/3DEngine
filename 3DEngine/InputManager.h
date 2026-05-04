#pragma once
#include <array>
#include <windows.h>
#include <windowsx.h>

namespace Engine {
    class InputManager {
    private:
        std::array<bool, 256> m_keys;
        std::array<bool, 256> m_prevKeys;

        bool m_firstMouse = true;
        int m_mouseX = 0;
        int m_mouseY = 0;
        int m_mouseDeltaX = 0;
        int m_mouseDeltaY = 0;
        int m_mouseScroll = 0;

    public:
        InputManager() {
            m_keys.fill(false);
            m_prevKeys.fill(false);
        }

        void ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
            switch (uMsg) {

            case WM_KEYDOWN:
                if (wParam < 256) m_keys[wParam] = true;
                break;
            case WM_KEYUP:
                if (wParam < 256) m_keys[wParam] = false;
                break;


            case WM_MOUSEMOVE: {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                if (m_firstMouse) {
                    m_mouseX = x; m_mouseY = y; m_firstMouse = false;
                }
                m_mouseDeltaX += x - m_mouseX;
                m_mouseDeltaY += y - m_mouseY;
                m_mouseX = x; m_mouseY = y;
                break;
            }


            case WM_MOUSEWHEEL:
                m_mouseScroll += GET_WHEEL_DELTA_WPARAM(wParam);
                break;


            case WM_LBUTTONDOWN: m_keys[VK_LBUTTON] = true; break;
            case WM_LBUTTONUP:   m_keys[VK_LBUTTON] = false; break;
            case WM_RBUTTONDOWN: m_keys[VK_RBUTTON] = true; break;
            case WM_RBUTTONUP:   m_keys[VK_RBUTTON] = false; break;
            case WM_MBUTTONDOWN: m_keys[VK_MBUTTON] = true; break;
            case WM_MBUTTONUP:   m_keys[VK_MBUTTON] = false; break;


            case WM_XBUTTONDOWN:
                if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) m_keys[VK_XBUTTON1] = true;
                else                                        m_keys[VK_XBUTTON2] = true;
                break;
            case WM_XBUTTONUP:
                if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) m_keys[VK_XBUTTON1] = false;
                else                                        m_keys[VK_XBUTTON2] = false;
                break;
            }
        }

        bool IsKeyHeld(int key) const {
            if (key < 0 || key >= 256) return false;
            return m_keys[key];
        }

        bool IsKeyPressed(int key) const {
            if (key < 0 || key >= 256) return false;
            return m_keys[key] && !m_prevKeys[key];
        }

        void GetMouseDelta(int& dx, int& dy) const {
            dx = m_mouseDeltaX;
            dy = m_mouseDeltaY;
        }

        void Update() {
            m_prevKeys = m_keys;
            m_mouseDeltaX = 0;
            m_mouseDeltaY = 0;
            m_mouseScroll = 0;
        }
    };
}