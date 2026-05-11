
// 3DEngineView.cpp : implementation of the CMy3DEngineView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "3DEngine.h"
#endif

#include "3DEngineDoc.h"
#include "3DEngineView.h"

#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Sprite.h"


#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "MainFrm.h"


// CMy3DEngineView

IMPLEMENT_DYNCREATE(CMy3DEngineView, CView)

BEGIN_MESSAGE_MAP(CMy3DEngineView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CMy3DEngineView construction/destruction

CMy3DEngineView::CMy3DEngineView() noexcept
{
	// TODO: add construction code here

}

CMy3DEngineView::~CMy3DEngineView()
{

}

BOOL CMy3DEngineView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	cs.lpszClass = AfxRegisterWndClass(CS_OWNDC, ::LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr);

	return CView::PreCreateWindow(cs);
}

// CMy3DEngineView printing

BOOL CMy3DEngineView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMy3DEngineView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMy3DEngineView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CMy3DEngineView diagnostics

#ifdef _DEBUG
void CMy3DEngineView::AssertValid() const
{
	CView::AssertValid();
}

void CMy3DEngineView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMy3DEngineDoc* CMy3DEngineView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMy3DEngineDoc)));
	return (CMy3DEngineDoc*)m_pDocument;
}
#endif //_DEBUG


// CMy3DEngineView message handlers

void CMy3DEngineView::OnDraw(CDC* /*pDC*/)
{
	
}


void CMy3DEngineView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0) return;

	m_openGL.SetViewport(cx, cy);

	m_ECS.m_registry.Each<Engine::CameraComponent>([&cx, &cy](Engine::EntityID entity, Engine::CameraComponent  &camera) {
		camera.SetAspect((float)cx, (float)cy);
	});

	Render();
}

int CMy3DEngineView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_openGL.InitOpenGL(this)) {
		MessageBox(_T("Failed to initialize OpenGL!"), _T("Error"));
		return -1;
	}

	Engine::ResourceManager::Instance = &m_resourceManager;

	m_ECS.Init();

	return 0;
}


void CMy3DEngineView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CMainFrame* pFrame = (CMainFrame*)GetParentFrame();
	if (pFrame != nullptr)
	{
		CEntityTreePane* pEntityTreePane = pFrame->m_entityTreePane;
		if (pEntityTreePane != nullptr)
		{
			pEntityTreePane->SetECS(&m_ECS);
		}

		CEntityPropertyPane* pEntityPropertyPane = pFrame->m_entityPropertyPane;
		if (pEntityPropertyPane != nullptr)
		{
			pEntityPropertyPane->SetECS(&m_ECS);
		}
	}

	//UINT_PTR myTimer = SetTimer(1, 1, NULL);
}
void CMy3DEngineView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		
	}

	CView::OnTimer(nIDEvent);
}

void CMy3DEngineView::Render()
{
	m_timer.Tick();

	m_openGL.MakeCurrent();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_ECS.FixedUpdate(m_timer.GetDeltaTime());

	m_ECS.Update(m_timer.GetDeltaTime());

	m_ECS.Render();

	m_openGL.SwapBuffers();
}

LRESULT CMy3DEngineView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	m_ECS.m_inputManager.ProcessMessage(message, wParam, lParam);

	return CView::WindowProc(message, wParam, lParam);
}
