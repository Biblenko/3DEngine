#include "pch.h"
#include "CEntityPropertyPane.h"

IMPLEMENT_DYNAMIC(CEntityPropertyPane, CDockablePane)

BEGIN_MESSAGE_MAP(CEntityPropertyPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_SETFOCUS()
    ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &CEntityPropertyPane::OnPropertyChanged)
END_MESSAGE_MAP()



namespace {
    enum PropID {
        Trans_PosX = 101, Trans_PosY = 102, Trans_PosZ = 103,
        Trans_RotX = 111, Trans_RotY = 112, Trans_RotZ = 113,
        Trans_ScaleX = 121, Trans_ScaleY = 122, Trans_ScaleZ = 123,

        Name_Value = 201,

        Mat_Ambient = 301, Mat_Diffuse = 302, Mat_Specular = 303, Mat_Shininess = 304,

        Light_Color = 401, Light_Intensity = 402
    };
}



CEntityPropertyPane::CEntityPropertyPane()
{
    p_esc = nullptr;
    m_selectedEntity = -1;
}

void CEntityPropertyPane::SetECS(Engine::ECS* _ecs)
{
    p_esc = _ecs;
    if (!p_esc || !GetSafeHwnd()) return;

    p_esc->m_events.Subscribe<Engine::EventEntitySelect>([this](const Engine::EventEntitySelect& event) 
        {
            m_selectedEntity = event.entity;
            this->PopulateProperties(event.entity);
        });
}

void CEntityPropertyPane::PopulateProperties(Engine::EntityID entity)
{
    if (!m_wndPropGrid.GetSafeHwnd()) return;
    m_wndPropGrid.RemoveAll();
    if (!p_esc) return;

    // NAME
    const auto& nameComponent = p_esc->m_registry.GetComponent<Engine::NameComponent>(entity);
    if (nameComponent)
    {
        auto* pGroup = new CMFCPropertyGridProperty(_T("NameComponent"));

        auto* pName = new CMFCPropertyGridProperty(_T("Name"), (_variant_t)CString(nameComponent->m_name.c_str()), _T("І'мя ентіті"));
        pName->SetData(PropID::Name_Value);

        pGroup->AddSubItem(pName);
        m_wndPropGrid.AddProperty(pGroup);
    }

    // TRANSFORM
    const auto& transformComponent = p_esc->m_registry.GetComponent<Engine::TransformComponent>(entity);
    if (transformComponent)
    {
        auto* pGroup = new CMFCPropertyGridProperty(_T("TransformComponent"));

        auto addPos = [&](const CString& name, float val, PropID id, const CString& desc) {
            auto* pProp = new CMFCPropertyGridProperty(name, (_variant_t)val, desc);
            pProp->SetData(id);
            pGroup->AddSubItem(pProp);
            };

        addPos(_T("Position X"), transformComponent->m_position.x, PropID::Trans_PosX, _T("Ось X"));
        addPos(_T("Position Y"), transformComponent->m_position.y, PropID::Trans_PosY, _T("Ось Y"));
        addPos(_T("Position Z"), transformComponent->m_position.z, PropID::Trans_PosZ, _T("Ось Z"));

        addPos(_T("Rotation X"), transformComponent->m_rotation.x, PropID::Trans_RotX, _T("Ось X"));
        addPos(_T("Rotation Y"), transformComponent->m_rotation.y, PropID::Trans_RotY, _T("Ось Y"));
        addPos(_T("Rotation Z"), transformComponent->m_rotation.z, PropID::Trans_RotZ, _T("Ось Z"));

        addPos(_T("Scale X"), transformComponent->m_scale.x, PropID::Trans_ScaleX, _T("Ось X"));
        addPos(_T("Scale Y"), transformComponent->m_scale.y, PropID::Trans_ScaleY, _T("Ось Y"));
        addPos(_T("Scale Z"), transformComponent->m_scale.z, PropID::Trans_ScaleZ, _T("Ось Z"));

        m_wndPropGrid.AddProperty(pGroup);
    }

    m_wndPropGrid.AdjustLayout();
}

LRESULT CEntityPropertyPane::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
    auto* pProp = reinterpret_cast<CMFCPropertyGridProperty*>(lParam);

    if (!pProp || !p_esc || m_selectedEntity == Engine::NULL_INDEX) return 0;

    int propertyID = (int)pProp->GetData();
    COleVariant var = pProp->GetValue();

    // Получаем указатели на компоненты
    auto* transform = p_esc->m_registry.GetComponent<Engine::TransformComponent>(m_selectedEntity);
    auto* nameComp = p_esc->m_registry.GetComponent<Engine::NameComponent>(m_selectedEntity);

    try
    {
        switch (propertyID)
        {
            // --- NAME ---
        case PropID::Name_Value:
            if (nameComp)
            {
                var.ChangeType(VT_BSTR);
                nameComp->m_name = CW2A(var.bstrVal); // Конвертация из wide string в std::string
            }
            break;

            // --- POSITION ---
        case PropID::Trans_PosX:
            if (transform) { var.ChangeType(VT_R4); transform->m_position.x = var.fltVal; transform->m_isDirty = true; }
            break;
        case PropID::Trans_PosY:
            if (transform) { var.ChangeType(VT_R4); transform->m_position.y = var.fltVal; transform->m_isDirty = true; }
            break;
        case PropID::Trans_PosZ:
            if (transform) { var.ChangeType(VT_R4); transform->m_position.z = var.fltVal; transform->m_isDirty = true; }
            break;

            // --- ROTATION ---
        case PropID::Trans_RotX:
            if (transform) { var.ChangeType(VT_R4); transform->m_rotation.x = var.fltVal; transform->m_isDirty = true; }
            break;
        case PropID::Trans_RotY:
            if (transform) { var.ChangeType(VT_R4); transform->m_rotation.y = var.fltVal; transform->m_isDirty = true; }
            break;
        case PropID::Trans_RotZ:
            if (transform) { var.ChangeType(VT_R4); transform->m_rotation.z = var.fltVal; transform->m_isDirty = true; }
            break;

            // --- SCALE ---
        case PropID::Trans_ScaleX:
            if (transform) { var.ChangeType(VT_R4); transform->m_scale.x = var.fltVal; transform->m_isDirty = true; }
            break;
        case PropID::Trans_ScaleY:
            if (transform) { var.ChangeType(VT_R4); transform->m_scale.y = var.fltVal; transform->m_isDirty = true; }
            break;
        case PropID::Trans_ScaleZ:
            if (transform) { var.ChangeType(VT_R4); transform->m_scale.z = var.fltVal; transform->m_isDirty = true; }
            break;
        }
    }
    catch (COleException* e)
    {
        // Игнорируем ошибку конвертации (спасет от краша при неверном вводе пользователя)
        e->Delete();
    }

    p_esc->m_events.Trigger<Engine::EventEntityChanged>(Engine::EventEntityChanged{ m_selectedEntity });

    return LRESULT();
}

int CEntityPropertyPane::OnCreate(LPCREATESTRUCT lp)
{
    if (CDockablePane::OnCreate(lp) == -1)
        return -1;

    DWORD style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER;

    CRect dump(0, 0, 0, 0);

    if (!m_wndPropGrid.Create(style, dump, this, IDC_PROP_GRID))
        return -1;

    InitPropGrid();

    return 0;
}

void CEntityPropertyPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    if (m_wndPropGrid.GetSafeHwnd())
        m_wndPropGrid.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);

}

void CEntityPropertyPane::OnSetFocus(CWnd* pOldWnd)
{
    CDockablePane::OnSetFocus(pOldWnd);
    m_wndPropGrid.SetFocus();
}


void CEntityPropertyPane::InitPropGrid()
{
    m_wndPropGrid.EnableHeaderCtrl(TRUE);
    m_wndPropGrid.EnableDescriptionArea();
    m_wndPropGrid.SetVSDotNetLook();
    m_wndPropGrid.MarkModifiedProperties();
}