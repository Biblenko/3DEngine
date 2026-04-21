
// 3DEngineView.h : interface of the CMy3DEngineView class
//

#pragma once


class CMy3DEngineView : public CView
{
protected: // create from serialization only
	CMy3DEngineView() noexcept;
	DECLARE_DYNCREATE(CMy3DEngineView)

// Attributes
public:
	CMy3DEngineDoc* GetDocument() const;

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
};

#ifndef _DEBUG  // debug version in 3DEngineView.cpp
inline CMy3DEngineDoc* CMy3DEngineView::GetDocument() const
   { return reinterpret_cast<CMy3DEngineDoc*>(m_pDocument); }
#endif

