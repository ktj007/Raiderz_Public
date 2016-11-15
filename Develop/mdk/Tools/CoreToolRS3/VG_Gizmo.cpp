#include "stdafx.h"
#include "Gizmo.h"
#include "VG_Gizmo.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlMediatorHelperRender.h"

VG_Gizmo::VG_Gizmo(void)
: m_pCurrentGizmo_(NULL)
, m_bStateLocked(false)
, m_bEnabled(false)
{
}

VG_Gizmo::~VG_Gizmo(void)
{
	SAFE_DELETE(m_pGizmoPosition_);
	SAFE_DELETE(m_pGizmoRotation_);
}

void VG_Gizmo::OnInitialize()
{
	m_pGizmoPosition_ = new CControlMediatorHelperGizmo_Position( m_pControlMediator_ );
	m_pGizmoRotation_ = new CControlMediatorHelperGizmo_Rotation( m_pControlMediator_ );
}

void VG_Gizmo::Listen(CoreMessage& _message)
{
	// 전역 처리
	switch( _message.nID)
	{
	case VIEW_MESSAGE::DEVICE_CREATED :
		OnInitialize();
		break;

	case VIEW_MESSAGE::LOGIC_RENDER : 
		OnLogicObjectRender(); 
		break;

	case VIEW_MESSAGE::LOGIC_MOVE : 
		m_pControlMediator_->GetHelperAction()->SetSelectedObjectCenterAndActionFrame();
		break;

	case VIEW_MESSAGE::SELECTION_CHANGED:
		OnLogicObjectSelected( );
		break;

	case VIEW_MESSAGE::WB_STATE_CHANGED:
		OnWBStateChanged();
		break;
	}
}

void VG_Gizmo::OnLogicObjectSelected()
{
	Gizmo* pGizmo = GetCurrentGizmo();
	if( pGizmo )
	{
		int nSelect = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject()->size();
		if(nSelect)
		{
// 			mlog( "VG_Gizmo enabled\n");
			m_bEnabled = true;
		}
		else
		{
// 			mlog( "VG_Gizmo disabled\n");
			m_bEnabled = false;
		}
	}
}

void VG_Gizmo::OnWBStateChanged()
{
	if( m_pControlMediator_->GetCurrentWBState() == WORKBENCH_STATE::OBJECT_MOVE )
	{
		SetCurrentGizmo(m_pGizmoPosition_);
	}
	else if( m_pControlMediator_->GetCurrentWBState() == WORKBENCH_STATE::OBJECT_ROTATE)
	{
		SetCurrentGizmo(m_pGizmoRotation_);
	}
	else
	{
		SetCurrentGizmo( NULL );
	}

	OnLogicObjectSelected(); // 선택 된채로 state 가 바뀔수도 있으므로 갱신
}

void VG_Gizmo::OnLogicObjectRender()
{
	if( m_bEnabled == false ) return;

	//////////////////////////////////////////////////////////////////////////
	// update & render gizmo

	Gizmo* pGizmo = GetCurrentGizmo();
	if( pGizmo )
	{
		rs3::RVector* pCenter = m_pControlMediator_->GetHelperAction()->GetSelectNodeCenter();
		if(pCenter)
		{

			rs3::RMatrix matTranslation_;
			matTranslation_.MakeIdentity();
			matTranslation_.SetTranslation(*pCenter);
			pGizmo->SetTransform( matTranslation_ );

			if( m_bStateLocked == false )
				pGizmo->UpdateStates();

			pGizmo->Render();
		}
	}
}

AXIS_STATE VG_Gizmo::GetGizmoState()
{
	if( m_pCurrentGizmo_ == NULL ) return AXIS_NULL;

	return m_pCurrentGizmo_->GetGizmoState();
}

rs3::RVector VG_Gizmo::GetGizmoPosition()
{
	if( m_pCurrentGizmo_ == NULL ) return rs3::RVector(0,0,0);

	return m_pCurrentGizmo_->GetPosition();
}
