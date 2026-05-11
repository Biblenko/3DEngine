#include "pch.h"
#include "CEntityTreePane.h"

#include "Components.h"

IMPLEMENT_DYNAMIC(CEntityTreePane, CDockablePane)

BEGIN_MESSAGE_MAP(CEntityTreePane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CTRL, &CEntityTreePane::OnEntityTreePaneSelect)
END_MESSAGE_MAP()

CEntityTreePane::CEntityTreePane()
{
    p_esc = nullptr;
    m_selectedEntity = -1;
}

void CEntityTreePane::SetECS(Engine::ECS* _ecs)
{
    p_esc = _ecs;
    if (!p_esc || !GetSafeHwnd()) return;

    RefreshEntityTree();

    p_esc->m_events.Subscribe<Engine::EventEntityChanged>([this](const Engine::EventEntityChanged& event)
        {
            if (!GetSafeHwnd()) return;

            auto nameComponent = p_esc->m_registry.GetComponent<Engine::NameComponent>(event.entity);
            if (nameComponent)
            {
                auto it = m_entityMap.find(event.entity);
                if (it != m_entityMap.end())
                {
                    m_wndTree.SetItemText(it->second, CString(nameComponent->m_name.c_str()));
                }
            }
        });
}

void CEntityTreePane::RefreshEntityTree()
{
    if (!p_esc || !GetSafeHwnd()) return;

    m_wndTree.DeleteAllItems();
    m_entityMap.clear();

    auto& entities = p_esc->m_registry.Entities<Engine::NameComponent>();

    for (auto& entity : entities)
    {
        CString name(p_esc->m_registry.GetComponent<Engine::NameComponent>(entity)->m_name.c_str());

        HTREEITEM hItem = m_wndTree.InsertItem(name);
        m_wndTree.SetItemData(hItem, (DWORD_PTR)entity);

        m_entityMap[entity] = hItem;
    }
}

int CEntityTreePane::OnCreate(LPCREATESTRUCT lp)
{
    if (CDockablePane::OnCreate(lp) == -1)
        return -1;

    DWORD style = TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT |
        WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT;

    CRect dump(0, 0, 0, 0);

    if (!m_wndTree.Create(style, dump, this, IDC_TREE_CTRL))
        return -1;

    return 0;
}

void CEntityTreePane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    if (m_wndTree.GetSafeHwnd())
        m_wndTree.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CEntityTreePane::OnEntityTreePaneSelect(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    *pResult = 0;

    if (!p_esc || !pNMTreeView->itemNew.hItem) return;

    m_selectedEntity = (Engine::EntityID)m_wndTree.GetItemData(pNMTreeView->itemNew.hItem);
    p_esc->m_events.Trigger(Engine::EventEntitySelect{ m_selectedEntity });
}
