
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMy3DEngineView

IMPLEMENT_DYNCREATE(CMy3DEngineView, CView)

BEGIN_MESSAGE_MAP(CMy3DEngineView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
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
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMy3DEngineView drawing

void CMy3DEngineView::OnDraw(CDC* /*pDC*/)
{
	CMy3DEngineDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
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
