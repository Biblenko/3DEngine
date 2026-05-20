
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "3DEngine.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_ENTITYTREE, &CMainFrame::OnViewEntityTree)
	ON_COMMAND(ID_VIEW_ENTITYPROPERTIES, &CMainFrame::OnViewEntityProperties)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	m_entityTreePane = nullptr;
	m_entityPropertyPane = nullptr;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDockingManager::SetDockingMode(DT_SMART);
	EnableDocking(CBRS_ALIGN_ANY);

	//if (!m_wndStatusBar.Create(this))
	//{
	//	TRACE0("Failed to create status bar\n");
	//	return -1;      // fail to create
	//}

	//m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));


	OnViewEntityTree();
	OnViewEntityProperties();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_OVERLAPPED | WS_CAPTION   /*| FWS_ADDTOTITLE*/
		| WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
		| WS_SYSMENU | WS_CLIPCHILDREN;


	cs.dwExStyle |= WS_EX_ACCEPTFILES;

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers


void CMainFrame::OnViewEntityTree()
{
	if (m_entityTreePane && m_entityTreePane->GetSafeHwnd())
	{
		m_entityTreePane->ShowPane(!m_entityTreePane->IsVisible(), FALSE, TRUE);
		return;
	}

	m_entityTreePane = new CEntityTreePane;

	UINT style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI;
	CString strTitle = _T("Entity Tree");

	if (!m_entityTreePane->Create(strTitle, this,
		CRect(0, 0, 200, 400), TRUE, ID_VIEW_ENTITYTREE, style))
	{
		delete m_entityTreePane;
		m_entityTreePane = NULL;
		return;
	}

	m_entityTreePane->EnableDocking(CBRS_ALIGN_ANY);

	DockPane((CBasePane*)m_entityTreePane, AFX_IDW_DOCKBAR_LEFT);

	m_entityTreePane->ShowPane(TRUE, FALSE, TRUE);

	RecalcLayout();


}

void CMainFrame::OnViewEntityProperties()
{
	if (m_entityPropertyPane && m_entityPropertyPane->GetSafeHwnd())
	{
		m_entityPropertyPane->ShowPane(!m_entityPropertyPane->IsVisible(), FALSE, TRUE);
		return;
	}

	m_entityPropertyPane = new CEntityPropertyPane;

	UINT style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI;
	CString strTitle = _T("Entity Properties");

	if (!m_entityPropertyPane->Create(strTitle, this,
		CRect(0, 0, 200, 400), TRUE, ID_VIEW_ENTITYPROPERTIES, style))
	{
		delete m_entityPropertyPane;
		m_entityPropertyPane = NULL;
		return;
	}

	m_entityPropertyPane->EnableDocking(CBRS_ALIGN_ANY);

	DockPane((CBasePane*)m_entityPropertyPane, AFX_IDW_DOCKBAR_LEFT);

	m_entityPropertyPane->ShowPane(TRUE, FALSE, TRUE);

	RecalcLayout();
}
