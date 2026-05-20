#include "pch.h"
#include "OpenGL.h"

namespace Engine {

    OpenGL::OpenGL() {
        ZeroMemory(&m_pfd, sizeof(PIXELFORMATDESCRIPTOR));

        m_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        m_pfd.nVersion = 1;
        m_pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                        PFD_SUPPORT_OPENGL |
                        PFD_DOUBLEBUFFER;
        m_pfd.iPixelType = PFD_TYPE_RGBA;       
        m_pfd.cColorBits = 32;                  
        m_pfd.cAlphaBits = 8;                   
        m_pfd.cDepthBits = 24;                  
        m_pfd.cStencilBits = 8;                 
        m_pfd.iLayerType = PFD_MAIN_PLANE;
    }

    OpenGL::~OpenGL() {
        Cleanup();
    }

    bool OpenGL::InitOpenGL(CWnd* pWnd) {

        if (!pWnd || !pWnd->GetSafeHwnd()) {
            TRACE0("Error: Invalid window pointer\n");
            return false;
        }

        m_pWnd = pWnd;

        if (!SetupPixelFormat()) {
            TRACE0("Error: Failed to setup pixel format\n");
            Cleanup();
            return false;
        }

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

        if (!wglMakeCurrent(hdc, m_hRC)) {
            TRACE0("Error: Failed to make context current\n");
            Cleanup();
            return false;
        }

        glewExperimental = GL_FALSE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            CString errMsg;
            errMsg.Format(_T("GLEW initialization failed: %S"), glewGetErrorString(err));
            TRACE1("%s\n", (LPCTSTR)errMsg);
            Cleanup();
            return false;
        }


        ApplySettings();

        return true;
    }

    bool OpenGL::SetupPixelFormat() {
        if (!m_pWnd || !m_pWnd->GetSafeHwnd()) {
            return false;
        }

        HDC hdc = GetContextDC();
        if (!hdc) return false;

        int pixelFormat = ChoosePixelFormat(hdc, &m_pfd);
        if (pixelFormat == 0) {
            TRACE0("Error: ChoosePixelFormat failed\n");
            return false;
        }

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
        if (width <= 0 || height <= 0) {
            TRACE2("Warning: Invalid viewport size: %dx%d\n", width, height);
            return;
        }

        glViewport(0, 0, width, height);
    }

    void OpenGL::ApplySettings() {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);
        glClearDepth(1.0f);


        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);


        glClearColor(0.8f, 0.9f, 1.0f, 1.0f);


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