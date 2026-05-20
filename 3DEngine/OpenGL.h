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
        CWnd* m_pWnd = nullptr;           
        HGLRC m_hRC = nullptr;            
        PIXELFORMATDESCRIPTOR m_pfd = {};

        bool SetupPixelFormat();
        HDC GetContextDC();
    };
}