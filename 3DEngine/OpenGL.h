#pragma once
#include <GL/glew.h>

namespace Engine {
    class OpenGL {
    public:
        OpenGL();
        ~OpenGL();

        OpenGL(const OpenGL&) = delete;
        OpenGL& operator=(const OpenGL&) = delete;

        bool InitOpenGL(CWnd* pWnd);
        void Cleanup();

        void MakeCurrent();
        void SwapBuffers();
        void SetViewport(int width, int height);
        void ApplySettings();

        bool IsInitialized() const { return m_hRC != nullptr; }

    private:
        CWnd* m_pWnd = nullptr;           // Указатель на окно MFC
        HGLRC m_hRC = nullptr;            // OpenGL контекст
        PIXELFORMATDESCRIPTOR m_pfd = {}; // Формат пикселя

        bool SetupPixelFormat();
        HDC GetContextDC();
    };
}