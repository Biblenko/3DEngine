
// 3DEngineView.h : interface of the CMy3DEngineView class
//

#pragma once


#include "OpenGL.h"
#include "ResourceManager.h"
#include "MeshGenerator.h"
#include "Timer.h"
#include "InputManager.h"
#include "ECS.h"

class CMy3DEngineView : public CView
{
protected: // create from serialization only
	CMy3DEngineView() noexcept;
	DECLARE_DYNCREATE(CMy3DEngineView)

// Attributes
public:
	CMy3DEngineDoc* GetDocument() const;

	Engine::OpenGL m_openGL;

	Engine::ResourceManager m_resourceManager;

	Engine::Timer m_timer;

	Engine::ECS m_ECS;
// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CMy3DEngineView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void Render();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // debug version in 3DEngineView.cpp
inline CMy3DEngineDoc* CMy3DEngineView::GetDocument() const
   { return reinterpret_cast<CMy3DEngineDoc*>(m_pDocument); }
#endif

