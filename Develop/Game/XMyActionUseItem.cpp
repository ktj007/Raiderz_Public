#include "stdafx.h"
#include "XMyActionUseItem.h"
#include "XModulePost.h"
#include "XEventID.h"
#include "XModuleBuff.h"
#include "XMyPlayer.h"
#include "XTalentInfoMgr.h"
#include "XPost_Item.h"
#include "XInteractionInfo.h"
#include "XItemHelper.h"
#include "XNonPlayer.h"
#include "XTalentHelper.h"
#include "XStrings.h"

// Use Talent //////////////////////////////////////////////////////////
XMyActionUseItem::XMyActionUseItem(XModuleMyControl* pOwner)
: XMyActionState(pOwner)
{
	m_fElipsed = 0.0f;
}

XMyActionUseItem::~XMyActionUseItem()
{
}

bool XMyActionUseItem::CheckEnterable(void* pParam)
{
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
		if (false == XItemHelper::IsUsableItemAtDead(pItemData->m_nID)) return false;	
	}
	else
	{
		if (true == XItemHelper::IsUsableItemAtDead(pItemData->m_nID) ||
			ACTION_STATE_IDLE != m_pOwner->GetCurrentActionStateID()) return false;
	}

	m_nInvenSlotID = nSlotID;

	// 쿨타임 체크
	if ( gvar.MyInfo.Talent.CoolTime.IsCoolDown(pItemData->m_vecUsableParam.front()))
	{
		wstring strMsg = CSFormatString::Format( XGetStr( L"ITEM_NOTUSEITEMYET"), FSParam(pItemData->GetName()));
		
		global.ui->OnPresentationLower(strMsg, PRESENTATION_ICON_EXCLAMATION, true);
		return false;
	}

	if (USABLE_TALENT_USE == pItemData->m_nUsableType)
	{
		XTalentInfo* pTalentInfo = info.talent->Get(pItemData->m_vecUsableParam.front());
		if (NULL == pTalentInfo) return false;

		if (false == XTalentHelper::CheckPortalTalentUsable(pTalentInfo))
		{
			wstring strMsg = CSFormatString::Format( XGetStr( L"ITEM_NOTUSEITEMINREGION"), FSParam(pItemData->GetName()));
			
			global.ui->OnPresentationLower(strMsg, PRESENTATION_ICON_EXCLAMATION, true);
			return false;
		}
	} 	


	XGetInteractionInfo().InteractionTargetUID.SetZero();

	if (pItemData->m_nUsableType == USABLE_INTERACTION ||
		pItemData->m_nUsableType == USABLE_INTERACTION_DEAD)
	{
		MUID uidNPC = CheckInteractionItem(pItemData);
		if (uidNPC == MUID::ZERO) return false;

		XNonPlayer* pNPC = gg.omgr->FindNPC(uidNPC);
		if (pNPC == NULL) return false;

		if(false == IsExist(pItemData->m_vecUsableParam, pNPC->GetInfo()->nID))
		{
			return false;			
		}

		//npc 인터랙션 가능으로
		XGetInteractionInfo().InteractionTargetUID = uidNPC;			
	}

	return true;
}

MUID XMyActionUseItem::CheckInteractionItem(XItemData* pItemData)
{
	INTERACTIONTYPE FindType = FIND_NPC;

	if(pItemData->m_nUsableType == USABLE_INTERACTION)
	{
		FindType = FIND_NPC;
	}
	else if(pItemData->m_nUsableType == USABLE_INTERACTION_DEAD)
	{
		FindType = FIND_DEADNPC;
	}

	return  GetInteractionObject(FindType);
}

MUID XMyActionUseItem::GetInteractionObject(INTERACTIONTYPE FindType)
{
	XModuleCollision* pModuleCollision = m_pOwner->GetOwner()->GetModuleCollision();
	if( pModuleCollision == NULL) return false;

	vec3 vPos = vec3(0.f, 0.f, 100.f);
	vec3 vDir = m_pOwner->GetOwner()->GetDirection();

	float fCheckX = 100.f;
	float fBottomZ = -CHECK_INTERACTION_COL_Z*0.5f;
	RBoundingBox CollisionBox;
	CollisionBox.vmax.Set( fCheckX, fCheckX, CHECK_INTERACTION_COL_Z);
	CollisionBox.vmin.Set(-fCheckX, -CHECK_INTERACTION_DISTANCE, fBottomZ);

	float fDist		= FLT_MAX;
	XObject* pTarObject = pModuleCollision->GetInteractionObjectColBox(FindType, vPos, vDir, CollisionBox, fDist);

	if (pTarObject)
	{
		float fValidInteractionDistance = pTarObject->GetValidInteractionDistance();

		if (fDist < fValidInteractionDistance)
		{
			return pTarObject->GetUID();
		}
	}


	return MUID::ZERO;
}


//탈렌트 사용.
void XMyActionUseItem::Enter(void* pParam)
{
	XMyActionState::Enter(pParam);

	if (XGetInteractionInfo().InteractionTargetUID != MUID::ZERO)
		XPostItem_Interact(m_nInvenSlotID, XGetInteractionInfo().InteractionTargetUID);
	else
	{
		int nSlotID = (pParam) ? *(int*)pParam : 0;
		XItem* pItem = gvar.MyInfo.Inventory.GetItem( nSlotID);
		if ( pItem != NULL)
		{
			XItemData* pItemData = pItem->m_pItemData;
			if ( pItemData->m_ItemType == ITEMTYPE_USABLE  &&  pItemData->m_nUsableType == USABLE_TALENT_USE)
			{
				int nBindTalentID = pItemData->m_vecUsableParam.front();
				XTalentInfo* pTalentInfo = info.talent->Get( nBindTalentID);
				if ( pTalentInfo != NULL  &&  pTalentInfo->m_fCastingTime > 0.0f)
				{
					if (global.script)
						global.script->GetGlueGameEvent().OnGameEvent( "ACTIONPROGRESSBAR", "BEGIN", MAPT_USE_ITEM, (int)( pTalentInfo->m_fCastingTime * 1000.0f));
				}
			}

			XPostItem_Use(m_nInvenSlotID);
		}
	}

	m_fElipsed = 0.0f;
}

XEventResult XMyActionUseItem::Event(XEvent& msg)
{
	switch (msg.m_nID)
	{
	case XEVTL_MYACTION_USE_TALENT:
		{
			int nTalentID = *(int*)(msg.m_pData);
			m_pOwner->DoAction(ACTION_STATE_USE_TALENT, nTalentID);
			return MR_TRUE;
		}
		break;
	}

	return XMyActionState::Event(msg);
}

void XMyActionUseItem::Update(float fDelta)
{
	m_fElipsed += fDelta;

	// 임시로 일정 시간 후에는 강제로 IDLE로 돌아갈 수 있도록 한다. 나중에 수정
	if ( m_fElipsed > 2.0f)
		m_pOwner->DoAction( ACTION_STATE_IDLE);
}