#include "stdafx.h"
#include "XMyActionDye.h"
#include "XController.h"
#include "XModuleMyControl.h"
#include "XItemData.h"
#include "XPost_Item.h"
#include "XModuleMotion.h"
#include "XStrings.h"

XMyActionDye::XMyActionDye( XModuleMyControl* pOwner )
: XMyActionState(pOwner)
, m_bActionDone(false)
, m_bPostDone(false)
{
	Init();
}

void XMyActionDye::Init()
{
	m_Regulator.Stop();
}

bool XMyActionDye::CheckEnterable( void* pParam )
{
	ACTION_STATE nPrevState = m_pOwner->GetCurrentActionStateID();
	if ( nPrevState != ACTION_STATE_IDLE)	return false;

	m_DyeData = *(DYE_DATA*)(pParam);
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( m_DyeData.m_nDyeItemIndex);
	if ( pItem == NULL  ||  pItem->m_pItemData->m_ItemType != ITEMTYPE_DYE)
		return false;

	return true;
}

XEventResult XMyActionDye::Event( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_VIRTUAL_KEY_PRESSEDONCE:
		{
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
			switch (nVirtualKey)
			{
			case VKEY_FORWARD:
			case VKEY_BACKWARD:
			case VKEY_LEFT:
			case VKEY_RIGHT:

				DyeCancel();
				return MR_TRUE;
			}
		}
		break;
	case XEVTD_TALENT_HIT:
		{
			DyeCancel();
			return MR_TRUE;
		}
		break;

	case XEVTL_DYE_ITEM_SELECT:
		{
			m_DyeData = *(DYE_DATA*)(msg.m_pData);
			XPostItem_DyePrepare(m_DyeData.m_nItemSlotType, m_DyeData.m_nItemSlotIndex, m_DyeData.m_nDyeItemIndex);
		}
		break;

	case XEVTL_DYE_START_ITEM:
		{
			m_Regulator.SetTickTime(2.f);
			m_Regulator.Start();

			m_DyeData = *(DYE_DATA*)(msg.m_pData);

			XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
			if (pModuleMotion)
			{
				pModuleMotion->ChangeMotion(L"operation", MT_DEFAULT, true);
			}

			return MR_TRUE;
		}
		break;
	case XEVTL_DYE_END_ITEM:
		{
			DyeEnd();
			return MR_TRUE;
		}
		break;
	case XEVTL_DYE_CANCEL:
		{
			DyeCancel();

			m_pOwner->DoActionIdle();

			return MR_TRUE;
		}
		break;
	}

	return MR_FALSE;
}

void XMyActionDye::Enter( void* pParam/*=NULL*/ )
{
	m_bPostDone	= false;
	m_bActionDone = false;

	Init();
	m_pOwner->StopRun();

	global.script->GetGlueGameEvent().OnGameEvent( "DYEING", "BEGIN");
}

void XMyActionDye::Exit()
{
	Init();

	global.script->GetGlueGameEvent().OnGameEvent( "DYEING", "END");
}

void XMyActionDye::Update( float fDelta )
{
	if ( m_bPostDone == false
		&& m_Regulator.IsActive() == false
		&& gg.controller->IsVirtualKeyPressedAsync(VKEY_ACTION2))
	{
		if(global.ui->IsMouseVisible() == false)
		{
			DyeCancel();
			return;
		}
	}

	if(m_Regulator.IsReady(fDelta))
	{
		m_Regulator.Stop();

		XPostItem_Dye(m_DyeData.m_nItemSlotType, m_DyeData.m_nItemSlotIndex, m_DyeData.m_nDyeItemIndex);

		m_bPostDone = true;
	}
}

void XMyActionDye::DyeCancel()
{
	m_bActionDone = true;

	XPostItem_DyePrepareCancel();

	const wchar_t* szMsg = XGetStr(L"SMSG_DYE_CANCEL");
	global.ui->OnPresentationLower(szMsg, PRESENTATION_ICON_EXCLAMATION);
	m_pOwner->DoActionIdle();
}

bool XMyActionDye::CheckExitable( int nNextStateID )
{
	if (nNextStateID == ACTION_STATE_IDLE)
	{
		if (!m_bActionDone)
		{
			return false;
		}
	}

	return true;
}

void XMyActionDye::DyeEnd()
{
	XItem* pItem = NULL;
	if(m_DyeData.m_nItemSlotType == SLOTTYPE_EQUIP)
	{
		pItem = gvar.MyInfo.EquipmentSlot.GetItem( (SH_ITEM_SLOT)m_DyeData.m_nItemSlotIndex);
	}
	else if(m_DyeData.m_nItemSlotType == SLOTTYPE_INVENTORY)
	{
		pItem =gvar.MyInfo.Inventory.GetItem( m_DyeData.m_nItemSlotIndex);
	}

	if ( pItem )
	{
		wstring strMsg = CSFormatString::Format( XGetStr( L"ITEM_DYED"), FSParam(pItem->m_pItemData->GetName()));
		global.ui->OnPresentationLower(strMsg, PRESENTATION_ICON_EXCLAMATION);
	}

	m_pOwner->OnEffectDyeDone();

	m_bActionDone = true;

	m_pOwner->DoActionIdle();
}


