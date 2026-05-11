#pragma once

#include "ECS.h"

enum 
{ 
    IDC_TREE_CTRL = 1000 
};

class CEntityTreePane : public CDockablePane
{

    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CEntityTreePane)
public:

    CEntityTreePane();

    void SetECS(Engine::ECS* _ecs);
    void RefreshEntityTree();

protected:

    afx_msg int OnCreate(LPCREATESTRUCT lp);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    afx_msg void OnEntityTreePaneSelect(NMHDR* pNMHDR, LRESULT* pResult);
private:

    Engine::ECS* p_esc;
    Engine::EntityID m_selectedEntity;
    std::unordered_map<Engine::EntityID, HTREEITEM> m_entityMap;

    CTreeCtrl m_wndTree;
};


