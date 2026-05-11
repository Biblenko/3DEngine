#pragma once

#include "ECS.h"

enum
{
    IDC_PROP_GRID = 1000
};

class CEntityPropertyPane : public CDockablePane
{
    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CEntityPropertyPane)

public:
    CEntityPropertyPane();

    void SetECS(Engine::ECS* _ecs);
    void PopulateProperties(Engine::EntityID entity);

protected:
    CMFCPropertyGridCtrl m_wndPropGrid;

    afx_msg int OnCreate(LPCREATESTRUCT lp);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);

private:
    Engine::ECS* p_esc;
    Engine::EntityID m_selectedEntity;

    void InitPropGrid();
};

