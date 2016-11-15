#include "stdafx.h"
#include "XMyActionAttune.h"
#include "XModuleMyControl.h"
#include "XModuleBuff.h"
#include "XMyPlayer.h"
#include "XItemHelper.h"
#include "XPost_Attune.h"
#include "XCommonData.h"
#include "XFeedbackInteractor.h"
#include "XModuleMotion.h"
#include "XStrings.h"

XMyActionAttune::XMyActionAttune( XModuleMyControl* pOwner )
: XMyActionState(pOwner)
{
	//m_nAttuneItemInvenSlotID = -1;
	m_Regulator.Stop();

	m_eAttuneState = AS_READY;
	m_bAttunable = false;
}

bool XMyActionAttune::CheckEnterable( void* pParam )
{
	ACTION_STATE nPrevState = m_pOwner->GetCurrentActionStateID();

	if (nPrevState != ACTION_STATE_IDLE &&
		nPrevState != ACTION_STATE_GESTURE)
		return false;

	//if(m_nAttuneItemInvenSlotID >= 0)
	//	return false;

	int nSlotID = (pParam) ? *(int*)pParam : 0;

	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem((SH_ITEM_SLOT)nSlotID);
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

//	global.script->GetGlueGameEvent().OnGameEvent( "Attune", "CHANGEDONE");

	return true;
}

void XMyActionAttune::Enter(void* pParam)
{
	XMyActionState::Enter(pParam);
	m_Regulator.Reset();

	int nSlotID = (pParam) ? *(int*)pParam : 0;
	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem((SH_ITEM_SLOT)nSlotID); //The user keeps the armor on, because we will force them into an animation. 
	if ( pItem != NULL)
	{
		if(pItem->m_nEXP == pItem->m_nNextAttuneXP && pItem->m_nAttuneLvl < 5) //We can attune our item TODO: We need to do this on the server
		{
			m_bAttunable = true;
			m_eAttuneState = AS_START;
		}
	}
}

void XMyActionAttune::Exit()
{
//	MCursorSystem::Set("defCursor");
	XMyActionState::Exit();
}

XEventResult XMyActionAttune::Event(XEvent& msg)
{
	switch (msg.m_nID)
	{
	case XEVTL_ATTUNE_ITEM_SELECT:
		{
			if(m_bAttunable)
			{
//				MCursorSystem::Set("defCursor");

				ATTUNE_DATA attune_data = *(ATTUNE_DATA*)(msg.m_pData);
				if(CheckAttuneAbleToDo(attune_data.nItemSlotID) == false)
					EndAttune(true);
				else
					XPostItem_AttunePrepare();

				global.script->GetGlueGameEvent().OnGameEvent( "ATTUNE", "BEGIN");

				return MR_TRUE;
			}
		}
		break;
	case XEVTL_ATTUNE_START:
		{
			if(m_bAttunable)
			{
				ATTUNE_DATA attune_data = *(ATTUNE_DATA*)(msg.m_pData);
				SendAttuneInfo(SLOTTYPE_EQUIP, attune_data.nItemSlotID);
			}
// 			if(m_nAttuneItemInvenSlotID >= 0)
// 			{
// 				m_AttuneData Attune_data = *(m_AttuneData*)(msg.m_pData);
// 
// 				if(CheckAttuneEnable(Attune_data.nItemSlotID))
// 				{
// 					if(Attune_data.bEquip)
// 						SendAttuneInfo(SLOTTYPE_EQUIP, Attune_data.nItemSlotID, m_nAttuneItemInvenSlotID, Attune_data.nAgentSlotID);
// 					else
// 						SendAttuneInfo(SLOTTYPE_INVENTORY, Attune_data.nItemSlotID, m_nAttuneItemInvenSlotID, Attune_data.nAgentSlotID);
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
			
				CancelAttune();

				global.script->GetGlueGameEvent().OnGameEvent( "ATTUNE", "END");

				break;
			}
		}
		break;
	case XEVTL_ATTUNE_CHECK_REQ:
		{
			/*if(m_nAttuneItemInvenSlotID >= 0)
			{
				m_AttuneData Attune_data = *(m_AttuneData*)(msg.m_pData);

				if(CheckAttuneEnable(Attune_data.nItemSlotID))
				{
					if(Attune_data.bEquip)
						XPostItem_AttuneCheck(SLOTTYPE_EQUIP, Attune_data.nItemSlotID, m_nAttuneItemInvenSlotID, Attune_data.nAgentSlotID);
					else
						XPostItem_AttuneCheck(SLOTTYPE_INVENTORY, Attune_data.nItemSlotID, m_nAttuneItemInvenSlotID, Attune_data.nAgentSlotID);
				}

				m_eAttuneState = AS_CHECK;

				//m_AttuneData.bEquipped = Attune_data.bEquip;
				m_AttuneData.nItemIndex = Attune_data.nItemSlotID;
				//m_AttuneData.nAgentInvenIndex = Attune_data.nAgentSlotID;


				// 정지*/
			if(m_bAttunable)
			{
				m_pOwner->StopRun();
				m_pOwner->InitAutoMove();

				return MR_TRUE;
			}

		}
		break;
	case XEVTL_ATTUNE_CHECK_SUCCESS:
		{
			m_Regulator.SetTickTime(3.f);
			m_Regulator.Start();

//			global.script->GetGlueGameEvent().OnGameEvent( "ACTIONPROGRESSBAR", "BEGIN", MAPT_INTERACTION, 3 * 1000.0f);

			XModuleMotion* pModuleMotion = m_pOwner->GetOwner()->GetModuleMotion();
			if (pModuleMotion)
			{
				pModuleMotion->ChangeMotion(L"PS_ia_beg2", MT_DEFAULT, true);
			}

			m_eAttuneState = AS_ATTUNING;
			return MR_TRUE;
		}
		break;
	case XEVTL_ATTUNE_RESULT:
		{
			// 다음 제스쳐에서 모션 처리하므로 데이터만 초기화
			Init();

			m_eAttuneState = AS_DONE;

			int nItemID = *(int*)(msg.m_pData);
			//int nAttuneItemID = *(int*)(msg.m_pResult);

			//Attuning can never fail. You are always guaranteed to Attune your item.
			global.ui->OnPresentationLower( XGetStr( L"ITEM_ATTUNESUCCESSFUL"), PRESENTATION_ICON_EXCLAMATION);
			global.script->GetGlueGameEvent().OnGameEvent( "ATTUNE", "RESULT", 0);

			XFeedbackInteractor feedback;
			feedback.DoAttuneSuccessAction(m_pOwner->GetOwner());

//			global.script->GetGlueGameEvent().OnGameEvent( "ACTIONPROGRESSBAR", "END");
			global.script->GetGlueGameEvent().OnGameEvent( "ATTUNE", "END");

			return MR_TRUE;
		}
		break;
	case XEVTD_TALENT_HIT:
		{
			CancelAttune();
		}
		break;
	case XEVTL_ATTUNE_CANCEL:
		{
			CancelAttune();
			return MR_TRUE;
		}
		break;
	}

	return XMyActionState::Event(msg);
}

void XMyActionAttune::CancelAttune()
{
	m_eAttuneState = AS_CANCEL;
	XPostItem_AttuneCancel();
	EndAttune(true);

	const wchar_t* szMsg = XGetStr(L"SMSG_ATTUNE_CANCEL");
	global.ui->OnPresentationLower(szMsg, PRESENTATION_ICON_EXCLAMATION);
}

void XMyActionAttune::SendAttuneInfo( SH_ITEM_SLOT_TYPE eSlotType, int nTargetSlotID)
{
	XPostItem_AttuneItem(eSlotType, nTargetSlotID);
}

bool XMyActionAttune::CheckAttuneEnable( int nSlotID )
{
	if(nSlotID == -1)
		return false;

	return true;
}

void XMyActionAttune::EndAttune(bool bUseIdleMotion)
{
	Init();

	global.script->GetGlueGameEvent().OnGameEvent( "ATTUNE", "END");
//	global.script->GetGlueGameEvent().OnGameEvent( "ACTIONPROGRESSBAR", "END");

	if(bUseIdleMotion)
		m_pOwner->DoActionIdle();
}

void XMyActionAttune::Update( float fDelta )
{
 	if(m_Regulator.IsReady(fDelta))
  	{
  		m_Regulator.Stop();
  
 		// 강화 시작
// 		global.script->GetGlueGameEvent().OnGameEvent( "Attune", "START");		
		//if(m_bAttunable)
			//SendAttuneInfo(SLOTTYPE_EQUIP, m_AttuneData.nItemIndex);
		
 	}
}

void XMyActionAttune::Init()
{
	m_Regulator.Stop();
}

bool XMyActionAttune::CheckExitable( int nNextStateID )
{
	// 다음 같은 경우는 나갈수 없다.
	if(m_eAttuneState < AS_DONE &&
		(nNextStateID == ACTION_STATE_USE_TALENT ||
		nNextStateID == ACTION_STATE_USE_ITEM ||
		nNextStateID == ACTION_STATE_WEAPON_CHANGE ||
		nNextStateID == ACTION_STATE_LOOTINGITEM))
		return false;

	if(nNextStateID == ACTION_STATE_IDLE && m_eAttuneState == AS_CHECK)
		return false;

	EndAttune(false);

	return true;
}

bool XMyActionAttune::CheckAttuneAbleToDo( int nTargetSlotID )
{
	XItem* pItem = NULL;
	pItem = gvar.MyInfo.EquipmentSlot.GetItem((SH_ITEM_SLOT)nTargetSlotID);
	if(!pItem)
		return false;
	else
		return true;
}
