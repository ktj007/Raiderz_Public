#include "stdafx.h"
#include "XNPCIConEffect.h"
#include "XNonPlayer.h"
#include "XEffectManager.h"
#include "XCharacter.h"
#include "XModuleEffect.h"
#include "XModuleUI.h"
#include "XHeadIcon.h"

XNPCIConEffect::XNPCIConEffect()
{
	m_uidEffect = MUID::ZERO;
}

XNPCIConEffect::~XNPCIConEffect()
{
	Destroy();
}

void XNPCIConEffect::Destroy(XObject * pObject)
{
	if(global.emgr)
		global.emgr->DeleteXEffect(m_uidEffect);

	m_uidEffect = MUID::ZERO;

	if (pObject)
	{
		XModuleUI* pModuleUI = pObject->GetModuleUI();
		if (pModuleUI)
		{
			XHeadIconPlacer* pHeadIconPlacer = pModuleUI->GetHeadIconPlacer();
			if (pHeadIconPlacer)
			{
				pHeadIconPlacer->SetNPCIConEffectUID(MUID::ZERO);
			}
		}
	}
}

void XNPCIConEffect::SetNPCICon(XObject * pObject, NPC_ICON_STATE NIS)
{
	switch (NIS)
	{
	case NIS_NONE:
	case NIS_ONLY_INTERACTION:	Destroy(pObject);	return;
	case NIS_QUEST_INTERACT:
	case NIS_LOOT:
		{
			Destroy(pObject);
			pObject->GetModuleEffect()->OnEffectEvent(XEFTEVT_EFFECT_LOOTABLE);
		}
		return;
	}

	CreateNPCIConEffect(pObject, NIS);
}

void XNPCIConEffect::CreateNPCIConEffect( XObject * pObject, NPC_ICON_STATE NIS )
{
	wstring EffectName;
	bool	bLaseframe = false;
	bool	bOnlyOneShow = false;

	switch (NIS)
	{
	case NIS_COMBAT:		
		{
			XNonPlayer* pNpc = AsNPC(pObject);
			if(pNpc && pNpc->IsShowAggro() == false)	return;	// 적 발견시 이펙트 안나오게 하는거(http://dev/redmine/issues/15762)

			EffectName = ICON_EFFECT_COMBAT;	
			bOnlyOneShow = true;
		}
		break;
	case NIS_QUEST_INTERACTION:
		{
			EffectName = ICON_EFFECT_QUEST_INTERACTION;
			bLaseframe = true;
		}
		break;
	case NIS_QUEST_ACCEPT:
		{
			EffectName = ICON_EFFECT_QUEST_ACCEPT; 
			bLaseframe = true;
		}
		break;
	case NIS_QUEST_NOT_BEGIN_ONLYICON:
	case NIS_QUEST_NOT_BEGIN:	EffectName = ICON_EFFECT_QUEST_NOT_BEGIN;	break;
	case NIS_QUEST_BEGIN:		EffectName = ICON_EFFECT_QUEST_BEGIN;		break;
	case NIS_QUEST_CONTINUE_ONLYICON:
	case NIS_QUEST_CONTINUE:	EffectName = ICON_EFFECT_QUEST_CONTINUE;	break;
	case NIS_QUEST_END:			EffectName = ICON_EFFECT_QUEST_END;			break;
	case NIS_CRAFT:				EffectName = ICON_EFFECT_CRAFT;				break;
	case NIS_SHOP:				EffectName = ICON_EFFECT_SHOP;				break;
	case NIS_GUILD_CREATE:		EffectName = ICON_EFFECT_GUILD_CREATE;				break;
	case NIS_TALK:				EffectName = ICON_EFFECT_TALK;				break;
	case NIS_ITEM_UNUSABLE:		EffectName = ICON_EFFECT_ITEM_UNUSABLE;		break;
	case NIS_ITEM_USABLE:		EffectName = ICON_EFFECT_ITEM_USABLE;		break;
	case NIS_HELPER:			EffectName = ICON_EFFECT_HELP;				break;
	case NIS_QUEST_REWARD:		EffectName = ICON_EFFECT_QUEST_REWARD;		break;
	case NIS_TRIGGER:			EffectName = ICON_EFFECT_TRIGGER;			break;
	default:					Destroy(pObject);							return;
	}

	Destroy(pObject);

	m_uidEffect = CreateEffect(pObject, EffectName, bLaseframe);

	// 한번 보여주면 끝인 아이콘... 말풍선이 끝나고 나서도 보여주면 안되는 경우
	if(bOnlyOneShow)
	{
		XNonPlayer* pTargetNPC	= AsNPC(pObject);
		if(pTargetNPC)
			pTargetNPC->SetNPCIConState(NIS_NONE);
	}
}

MUID XNPCIConEffect::CreateEffect( XObject* pObject, wstring& strEffectName, bool bLaseframe )
{
	XNonPlayer* pTargetNPC	= AsNPC(pObject);
	XNPCInfo* pNPCInfo		= pTargetNPC->GetInfo(true);
	MUID uidEffect(MUID::ZERO);

	XModuleUI* pModuleUI = pObject->GetModuleUI();
	if (pModuleUI)
	{
		XHeadIconPlacer* pHeadIconPlacer = pModuleUI->GetHeadIconPlacer();

		if (pHeadIconPlacer)
		{
			RMatrix matLocal;
			pHeadIconPlacer->MakeNPCIconLocalMatrix(matLocal, pNPCInfo->fTitleHeight, pTargetNPC->GetDirection());

			RActorNode* pActorNode = pHeadIconPlacer->FindTitleBone(pObject);

			uidEffect = global.emgr->AddXEffect(strEffectName.c_str(), pObject->GetActor(), pActorNode, matLocal, bLaseframe, true);
			pHeadIconPlacer->SetNPCIConEffectUID(uidEffect);
		}
	}

	return uidEffect;
}