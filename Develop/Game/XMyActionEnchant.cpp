#include "stdafx.h"
#include "XMyActionEnchant.h"
#include "XModuleMyControl.h"
#include "XModuleBuff.h"
#include "XMyPlayer.h"
#include "XItemHelper.h"
#include "XPost_Enchant.h"
#include "XCommonData.h"
#include "XFeedbackInteractor.h"
#include "XModuleMotion.h"
#include "XStrings.h"

XMyActionEnchant::XMyActionEnchant( XModuleMyControl* pOwner )
: XMyActionState(pOwner)
{
	m_nEnchantItemInvenSlotID = -1;
	m_Regulator.Stop();

	m_eEnchantState = ES_READY;
}

bool XMyActionEnchant::CheckEnterable( void* pParam )
{
	ACTION_STATE nPrevState = m_pOwner->GetCurrentActionStateID();

	if (nPrevState != ACTION_STATE_IDLE &&
		nPrevState != ACTION_STATE_GESTURE)
		return false;

	if(m_nEnchantItemInvenSlotID >= 0)
		return false;

	int nSlotID = (pParam) ? *(int*)pParam : 0;

	XItem* pItem = gvar.MyInfo.Inventory.GetItem(nSlotID);
	if (pItem == NULL) return false;

	XItemData* pItemData = pItem->m_pItemData;
	if (pItemData == NULL) return false;

	// 버프에 의한 상태 확인
	XBuffAttribute attrBuff = m_pOwner->GetMyPlayerOwner()->GetModuleBuff()->GetBuffAttribute();
	if ( attrBuff.IsMesmerizable())	return false;

	if (true == m_pOwner->GetMyPlayerOwner()->IsDead())
	{
		return false;
	}

//	global.script->GetGlueGameEvent().OnGameEvent( "ENCHANT", "CHANGEDONE");

	return true;
}

void XMyActionEnchant::Enter(void* pParam)
{
	XMyActionState::Enter(pParam);
	m_Regulator.Reset();

	m_nEnchantItemInvenSlotID = -1;

	int nSlotID = (pParam) ? *(int*)pParam : 0;
	XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
	if ( pItem != NULL)
	{
		XItemData* pItemData = pItem->m_pItemData;
		if(pItemData->m_ItemType == ITEMTYPE_ENCHANTSTONE)
		{
			m_nEnchantItemInvenSlotID = nSlotID;
//			MCursorSystem::Set("defEnchantCursor");

			m_eEnchantState = ES_START;
		}
	}
}

void XMyActionEnchant::Exit()
{
//	MCursorSystem::Set("defCursor");
	XMyActionState::Exit();
}

XEventResult XMyActionEnchant::Event(XEvent& msg)
{
	switch (msg.m_nID)
	{
	case XEVTL_ENCHANT_ITEM_SELECT:
		{
			if(m_nEnchantItemInvenSlotID >= 0)
			{
//				MCursorSystem::Set("defCursor");

				ENCHANT_DATA enchant_data = *(ENCHANT_DATA*)(msg.m_pData);
				if(CheckEnchantAbleToDo(enchant_data.nItemSlotID, m_nEnchantItemInvenSlotID, enchant_data.bEquip) == false)
					EndEnchant(true);
				else
					XPostItem_EnchantPrepare();

				global.script->GetGlueGameEvent().OnGameEvent( "ENCHANT", "BEGIN");

				return MR_TRUE;
			}
		}
		break;
	case XEVTL_ENCHANT_START:
		{
// 			if(m_nEnchantItemInvenSlotID >= 0)
// 			{
// 				ENCHANT_DATA enchant_data = *(ENCHANT_DATA*)(msg.m_pData);
// 
// 				if(CheckEnchantEnable(enchant_data.nItemSlotID))
// 				{
// 					if(enchant_data.bEquip)
// 						SendEnchantInfo(SLOTTYPE_EQUIP, enchant_data.nItemSlotID, m_nEnchantItemInvenSlotID, enchant_data.nAgentSlotID);
// 					else
// 						SendEnchantInfo(SLOTTYPE_INVENTORY, enchant_data.nItemSlotID, m_nEnchantItemInvenSlotID, enchant_data.nAgentSlotID);
// 				}
// 
// 				return MR_TRUE;
// 			}
		}
		break;
	case XEVTL_VIRTUAL_KEY_PRESSED:
		{
			XVirtualKey nVirtualKey = *static_cast<XVirtualKey*>(msg.m_pData);
			switch (nVirtualKey)
			{
			case VKEY_JUMP:
			case VKEY_FORWARD:
			case VKEY_BACKWARD:
			case VKEY_LEFT:
			case VKEY_RIGHT:
			
				CancelEnchant();

				global.script->GetGlueGameEvent().OnGameEvent( "ENCHANT", "END");

				break;
			}
		}
		break;
	case XEVTL_ENCHANT_CHECK_REQ:
		{
			if(m_nEnchantItemInvenSlotID >= 0)
			{
				ENCHANT_DATA enchant_data = *(ENCHANT_DATA*)(msg.m_pData);

				if(CheckEnchantEnable(enchant_data.nItemSlotID))
				{
					if(enchant_data.bEquip)
						XPostItem_EnchantCheck(SLOTTYPE_EQUIP, enchant_data.nItemSlotID, m_nEnchantItemInvenSlotID, enchant_data.nAgentSlotID);
					else
						XPostItem_EnchantCheck(SLOTTYPE_INVENTORY, enchant_data.nItemSlotID, m_nEnchantItemInvenSlotID, enchant_data.nAgentSlotID);
				}

				m_eEnchantState = ES_CHECK;

				m_EnchantData.bEquipped = enchant_data.bEquip;
				m_EnchantData.nItemIndex = enchant_data.nItemSlotID;
				m_EnchantData.nAgentInvenIndex = enchant_data.nAgentSlotID;


				// 정지
				m_pOwner->StopRun();
				m_pOwner->InitAutoMove();

				return MR_TRUE;
			}

		}
		break;
	case XEVTL_ENCHANT_CHECK_SUCCESS:
		{
			m_Regulator.SetTickTime(3.f);
			m_Regulator.Start();

//			global.script->GetGlueGameEvent().OnGameEvent( "ACTIONPROGRESSBAR", "BEGIN", MAPT_INTERACTION, 3 * 1000.0f);

			XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
			if (pModuleMotion)
			{
				pModuleMotion->ChangeMotion(L"operation", MT_DEFAULT, true);
			}

			m_eEnchantState = ES_ENCHANT;
			return MR_TRUE;
		}
		break;
	case XEVTL_ENCHANT_RESULT:
		{
			// 다음 제스쳐에서 모션 처리하므로 데이터만 초기화
			Init();

			m_eEnchantState = ES_DONE;

			int nItemID = *(int*)(msg.m_pData);
			int nEnchantItemID = *(int*)(msg.m_pResult);

			if(nEnchantItemID == ENCHANT_BROKEN_SLOT)
			{
				global.ui->OnPresentationLower( XGetStr( L"ITEM_ENCHANTFAILED"), PRESENTATION_ICON_EXCLAMATION);
				global.script->GetGlueGameEvent().OnGameEvent( "ENCHANT", "RESULT", 1);

				XFeedbackInteractor feedback;
				feedback.DoEnchantFailAction(m_pOwner->GetOwner());
			}
			else
			{
				global.ui->OnPresentationLower( XGetStr( L"ITEM_ENCHANTSUCCESSFUL"), PRESENTATION_ICON_EXCLAMATION);
				global.script->GetGlueGameEvent().OnGameEvent( "ENCHANT", "RESULT", 0);

				XFeedbackInteractor feedback;
				feedback.DoEnchantSuccessAction(m_pOwner->GetOwner());
			}

//			global.script->GetGlueGameEvent().OnGameEvent( "ACTIONPROGRESSBAR", "END");
			global.script->GetGlueGameEvent().OnGameEvent( "ENCHANT", "END");

			return MR_TRUE;
		}
		break;
	case XEVTD_TALENT_HIT:
		{
			CancelEnchant();
		}
		break;
	case XEVTL_ENCHANT_CANCEL:
		{
			CancelEnchant();
			return MR_TRUE;
		}
		break;
	}

	return XMyActionState::Event(msg);
}

void XMyActionEnchant::CancelEnchant()
{
	m_eEnchantState = ES_CANCEL;
	XPostItem_EnchantCancel();
	EndEnchant(true);

	const wchar_t* szMsg = XGetStr(L"SMSG_ENCHANT_CANCEL");
	global.ui->OnPresentationLower(szMsg, PRESENTATION_ICON_EXCLAMATION);
}

void XMyActionEnchant::SendEnchantInfo( SH_ITEM_SLOT_TYPE eSlotType, int nTargetSlotID, int nEnchantStoneSlotID, int nEnchantAgentSlotID)
{
	XPostItem_EnchantItem(eSlotType, nTargetSlotID, nEnchantStoneSlotID, nEnchantAgentSlotID);
}

bool XMyActionEnchant::CheckEnchantEnable( int nSlotID )
{
	if(nSlotID == -1)
		return false;

	return true;
}

void XMyActionEnchant::EndEnchant(bool bUseIdleMotion)
{
	Init();

	global.script->GetGlueGameEvent().OnGameEvent( "ENCHANT", "END");
//	global.script->GetGlueGameEvent().OnGameEvent( "ACTIONPROGRESSBAR", "END");

	if(bUseIdleMotion)
		m_pOwner->DoActionIdle();
}

void XMyActionEnchant::Update( float fDelta )
{
 	if(m_Regulator.IsReady(fDelta))
  	{
  		m_Regulator.Stop();
  
 		// 강화 시작
// 		global.script->GetGlueGameEvent().OnGameEvent( "ENCHANT", "START");		

		if(m_nEnchantItemInvenSlotID >= 0)
		{
			if(CheckEnchantEnable(m_EnchantData.nItemIndex))
			{
				if(m_EnchantData.bEquipped)
					SendEnchantInfo(SLOTTYPE_EQUIP, m_EnchantData.nItemIndex, m_nEnchantItemInvenSlotID, m_EnchantData.nAgentInvenIndex);
				else
					SendEnchantInfo(SLOTTYPE_INVENTORY, m_EnchantData.nItemIndex, m_nEnchantItemInvenSlotID, m_EnchantData.nAgentInvenIndex);
			}
		}
 	}
}

void XMyActionEnchant::Init()
{
	m_nEnchantItemInvenSlotID = -1;

	m_Regulator.Stop();
}

bool XMyActionEnchant::CheckExitable( int nNextStateID )
{
	// 다음 같은 경우는 나갈수 없다.
	if(m_eEnchantState < ES_DONE &&
		(nNextStateID == ACTION_STATE_USE_TALENT ||
		nNextStateID == ACTION_STATE_USE_ITEM ||
		nNextStateID == ACTION_STATE_WEAPON_CHANGE ||
		nNextStateID == ACTION_STATE_LOOTINGITEM))
		return false;

	if(nNextStateID == ACTION_STATE_IDLE && m_eEnchantState == ES_CHECK)
		return false;

	EndEnchant(false);

	return true;
}

bool XMyActionEnchant::CheckEnchantAbleToDo( int nTargetSlotID, int nEnchantStoneSlotID, bool bEquipSlot )
{
	XItem* pStoneItem = gvar.MyInfo.Inventory.GetItem(nEnchantStoneSlotID);
	XItem* pItem = NULL;
	if(bEquipSlot)
	{
		pItem = gvar.MyInfo.EquipmentSlot.GetItem((SH_ITEM_SLOT)nTargetSlotID);
	}
	else
	{
		pItem = gvar.MyInfo.Inventory.GetItem(nTargetSlotID);
	}

	// 레벨 제한
	if(pStoneItem->m_pItemData->m_EnchantStone.nLimitLevel > gvar.MyInfo.nLevel)
	{
		global.ui->OnPresentationLower( XGetStr( L"ITEM_NOTENCHANTBYLEVEL"), PRESENTATION_ICON_EXCLAMATION, true);
		return false;
	}
	
	return true;
}
