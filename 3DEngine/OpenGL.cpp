#include "pch.h"
#include "OpenGL.h"

namespace Engine {

    OpenGL::OpenGL() {
        // Инициализируем PIXELFORMATDESCRIPTOR в конструкторе
        ZeroMemory(&m_pfd, sizeof(PIXELFORMATDESCRIPTOR));

        m_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        m_pfd.nVersion = 1;
        m_pfd.dwFlags = PFD_DRAW_TO_WINDOW |    // Рисуем в окне
            PFD_SUPPORT_OPENGL |                // Поддерживаем OpenGL
            PFD_DOUBLEBUFFER;                   // Двойная буферизация
        m_pfd.iPixelType = PFD_TYPE_RGBA;       // RGBA формат
        m_pfd.cColorBits = 32;                  // 32 бита на цвет (8-8-8-8)
        m_pfd.cAlphaBits = 8;                   // Alpha канал
        m_pfd.cDepthBits = 24;                  // Z-буфер 24 бита
        m_pfd.cStencilBits = 8;                 // Stencil буфер 8 бит
        m_pfd.iLayerType = PFD_MAIN_PLANE;
    }

    OpenGL::~OpenGL() {
        Cleanup();
    }

    bool OpenGL::InitOpenGL(CWnd* pWnd) {
        // ✅ Проверяем входные параметры
        if (!pWnd || !pWnd->GetSafeHwnd()) {
            TRACE0("Error: Invalid window pointer\n");
            return false;
        }

        m_pWnd = pWnd;

        // ✅ Устанавливаем формат пикселя
        if (!SetupPixelFormat()) {
            TRACE0("Error: Failed to setup pixel format\n");
            Cleanup();
            return false;
        }

        // ✅ Получаем HDC и создаём контекст
        HDC hdc = GetContextDC();
        if (!hdc) {
            TRACE0("Error: Failed to get device context\n");
            Cleanup();
            return false;
        }

        m_hRC = wglCreateContext(hdc);
        if (!m_hRC) {
            TRACE0("Error: Failed to create OpenGL context\n");
            Cleanup();
            return false;
        }

        // ✅ Делаем контекст активным
        if (!wglMakeCurrent(hdc, m_hRC)) {
            TRACE0("Error: Failed to make context current\n");
            Cleanup();
            return false;
        }

        // ✅ Инициализируем GLEW
        glewExperimental = GL_FALSE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            CString errMsg;
            errMsg.Format(_T("GLEW initialization failed: %S"), glewGetErrorString(err));
            TRACE1("%s\n", (LPCTSTR)errMsg);
            Cleanup();
            return false;
        }

        // ✅ Применяем настройки
        ApplySettings();

        return true;
    }

    bool OpenGL::SetupPixelFormat() {
        if (!m_pWnd || !m_pWnd->GetSafeHwnd()) {
            return false;
        }

        HDC hdc = GetContextDC();
        if (!hdc) return false;

        // ✅ Выбираем подходящий формат пикселя
        int pixelFormat = ChoosePixelFormat(hdc, &m_pfd);
        if (pixelFormat == 0) {
            TRACE0("Error: ChoosePixelFormat failed\n");
            return false;
        }

        // ✅ Устанавливаем формат пикселя
        if (!SetPixelFormat(hdc, pixelFormat, &m_pfd)) {
            TRACE0("Error: SetPixelFormat failed\n");
            return false;
        }

        return true;
    }

    HDC OpenGL::GetContextDC() {
        if (!m_pWnd || !m_pWnd->GetSafeHwnd()) {
            return nullptr;
        }

        // ✅ Получаем HDC из окна (MFC-способ)
        return ::GetDC(m_pWnd->GetSafeHwnd());
    }

    void OpenGL::MakeCurrent() {
        if (!IsInitialized()) {
            TRACE0("Warning: OpenGL context not initialized\n");
            return;
        }

        if (!m_pWnd || !m_pWnd->GetSafeHwnd()) {
            TRACE0("Warning: Window is invalid\n");
            return;
        }

        HDC hdc = GetContextDC();
        if (hdc) {
            wglMakeCurrent(hdc, m_hRC);
        }
    }

    void OpenGL::SwapBuffers() {
        if (!m_pWnd || !m_pWnd->GetSafeHwnd()) {
            TRACE0("Warning: Window is invalid\n");
            return;
        }

        HDC hdc = GetContextDC();
        if (hdc) {
            ::SwapBuffers(hdc);
        }
    }

    void OpenGL::SetViewport(int width, int height) {
        // ✅ Валидируем размеры
        if (width <= 0 || height <= 0) {
            TRACE2("Warning: Invalid viewport size: %dx%d\n", width, height);
            return;
        }

        glViewport(0, 0, width, height);
    }

    void OpenGL::ApplySettings() {
        // ✅ Основные настройки OpenGL
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);
        glClearDepth(1.0f);

        // ✅ Culling (удаление невидимых граней)
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        // ✅ Цвет очистки фона
        glClearColor(0.8f, 0.9f, 1.0f, 1.0f);

        // ✅ Если используете освещение
        //glEnable(GL_LIGHTING);
        //glEnable(GL_LIGHT0);
        //glEnable(GL_COLOR_MATERIAL);
        //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void OpenGL::Cleanup() {
        if (m_hRC) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(m_hRC);
            m_hRC = nullptr;
        }
        m_pWnd = nullptr;
    }
}