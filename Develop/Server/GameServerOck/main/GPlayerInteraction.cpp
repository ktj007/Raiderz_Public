#include "StdAfx.h"
#include "GPlayerInteraction.h"
#include "GNPCMgr.h"
#include "GEntityPlayer.h"
#include "GPlayerDoing.h"
#include "GTradeSystem.h"
#include "GCraftSystem.h"
#include "GInnSystem.h"
#include "GQuestSystem.h"
#include "GGlobal.h"
#include "GCommand.h"
#include "GEntityNPC.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "GItemHolder.h"
#include "GItemUser.h"
#include "GQObjInteractor.h"
#include "GPlayerMailReserver.h"
#include "GClientMailRouter.h"


GPlayerInteraction::GPlayerInteraction(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
	m_ITargetUID.SetInvalid();
	m_ITriggerType = ITRT_NONE;
	m_ITargetType = ITAT_NONE;
	m_IUseItemUID.SetInvalid();
	
	m_nDilaogID = 0;
	m_pDialogSayInfo	= NULL;

	m_SelectedIElementInfo.m_nID = 0;
	m_SelectedIElementInfo.m_nType = IT_NONE;
	m_SelectedIElementInfo.m_pConditionsInfo = NULL;

	m_SleepRegulator.Stop();	
	m_InteractQObjectiveRegulator.Stop();
}

GPlayerInteraction::~GPlayerInteraction(void)
{	
}

void GPlayerInteraction::Update(float fDelta)
{
	PFC_THISFUNC;
	if (true == m_SleepRegulator.IsReady(fDelta))
	{
		m_SleepRegulator.Stop();

		gsys.pInnsystem->EndSleep(m_pOwner);
		gsys.pInteractionSystem->GetNPCInteractor().End(m_pOwner);
	}	

	if (true == m_InteractQObjectiveRegulator.IsReady(fDelta))
	{
		m_InteractQObjectiveRegulator.Stop();

		GNPCInteractionElementInfo& ieInfo = GetSelectedIElementInfo();
		if (1 >= ieInfo.m_vecAct.size()) return;

		int nQuestID = ieInfo.GetSinlgeAct();
		vector<int> vecQObjeciveID;		
		for (size_t i=1; i<ieInfo.m_vecAct.size(); i++)
		{
			int nQObjectiveID = ieInfo.m_vecAct[i];
			vecQObjeciveID.push_back(nQObjectiveID);
		}

		gsys.pQuestSystem->GetQObjInteractor().Act(m_pOwner, GetInteractingNPC(), nQuestID, vecQObjeciveID);
		gsys.pInteractionSystem->GetNPCInteractor().End(m_pOwner);
	}
}

void GPlayerInteraction::BeginInteraction(ITRIGGER_TYPE nITrigType, ITARGET_TYPE nITarType, MUID uidITarget, const vector<GNPCInteractionElementInfo*>& vecIElementInfo, MUID uidUseItem)
{
	m_pOwner->GetDoing().ChangeDoing(CD_INTERACTING);
	m_ITriggerType = nITrigType;
	m_ITargetType = nITarType;
	m_ITargetUID = uidITarget;
	m_IUseItemUID = uidUseItem;

	for each (GNPCInteractionElementInfo* pIElementInfo in vecIElementInfo)
	{
		m_mapSelectableElementInfo.insert(map<int, GNPCInteractionElementInfo*>::value_type(pIElementInfo->m_nID, pIElementInfo));
	}	
}

void GPlayerInteraction::SelectIElement(const GNPCInteractionElementInfo& IElementInfo)
{
	m_SelectedIElementInfo = IElementInfo;
}

void GPlayerInteraction::EndInteraction()
{
	// 인터렉션 종료전 처리
	if (m_SelectedIElementInfo.m_nType == IT_MAILBOX)
	{
		GPlayerMailReserver& reserver = m_pOwner->GetMailReserver();
		reserver.Clear();

		GClientMailRouter routerMail;
		routerMail.MailboxIsClosed(m_pOwner);
	}


	// 인터랙션 종료
	if (m_pOwner->GetDoing().IsNowInteracting())
		m_pOwner->GetDoing().ChangeDoing(CD_NORMAL);

	// 인터랙션으로 퀘스트목적을 변경하는 경우에는, 인터랙션 아이템의 소모가 인터랙션 종료시 일어나지 않는다.
	if (IT_QUEST_INTERACT_MORTAL != m_SelectedIElementInfo.m_nType &&
		IT_QUEST_INTERACT_IMMORTAL != m_SelectedIElementInfo.m_nType)
	{
		ConsumeIUseItem();
	}

	m_ITargetUID.SetInvalid();
	m_ITriggerType = ITRT_NONE;
	m_ITargetType = ITAT_NONE;	
	m_IUseItemUID.SetInvalid();

	m_nDilaogID = 0;
	m_pDialogSayInfo	= NULL;

	m_SelectedIElementInfo.m_nID = 0;
	m_SelectedIElementInfo.m_vecAct.clear();
	m_SelectedIElementInfo.m_nType = IT_NONE;
	m_SelectedIElementInfo.m_pConditionsInfo = NULL;

	m_SleepRegulator.Stop();
	m_mapSelectableElementInfo.clear();
}

GNPCInteractionElementInfo* GPlayerInteraction::GetSelectableIElementInfo(int nElementID)
{
	map<int, GNPCInteractionElementInfo*>::iterator itor = m_mapSelectableElementInfo.find(nElementID);
	if (m_mapSelectableElementInfo.end() == itor) return NULL;

	return (*itor).second;
}

GEntityNPC* GPlayerInteraction::GetInteractingNPC()
{
	if (ITAT_NPC != m_ITargetType) return NULL;

	return m_pOwner->FindNPC(m_ITargetUID);
}

GItem* GPlayerInteraction::GetInteractingItem()
{
	if (ITAT_ITEM != m_ITargetType) return NULL;

	GItemHolder* pItemHolder = m_pOwner->GetItemHolder();
	return pItemHolder->GetItem(SLOTTYPE_INVENTORY, m_ITargetUID);
}

void GPlayerInteraction::BeginDialog(int nDialogID, GDialogSayInfo* pDialogSayInfo)
{
	m_nDilaogID = nDialogID;
	m_pDialogSayInfo = pDialogSayInfo;
}

void GPlayerInteraction::ContinueDialog( GDialogSayInfo* pDialogSayInfo )
{
	m_pDialogSayInfo = pDialogSayInfo;
}

void GPlayerInteraction::EndDialog()
{
	m_pDialogSayInfo = NULL;
	m_nDilaogID = 0;
}

bool GPlayerInteraction::IsDialogExit()
{
	if (NULL != m_pDialogSayInfo) return false;

	return true;
}

void GPlayerInteraction::BeginSleep(float fTime)
{
	m_SleepRegulator.SetTickTime(fTime);
	m_SleepRegulator.Start();
}

void GPlayerInteraction::BeginInteractQObjectvie( float fTime )
{
	m_InteractQObjectiveRegulator.SetTickTime(fTime);
	m_InteractQObjectiveRegulator.Start();
}

void GPlayerInteraction::CancelInteractQObjectvie()
{
	m_InteractQObjectiveRegulator.Stop();
}

bool GPlayerInteraction::IsInteractQObjectiveRegulatorActive()
{
	return m_InteractQObjectiveRegulator.IsActive();
}

MUID GPlayerInteraction::GetInteractingNPCUID()
{
	return m_ITargetUID;
}

int GPlayerInteraction::GetInteractingNPCID()
{
	GEntityNPC* pNPC = GetInteractingNPC();
	if (NULL == pNPC) return 0;

	return pNPC->GetID();
}

void GPlayerInteraction::ConsumeIUseItem()
{
	gsys.pItemSystem->GetUser().ConsumeUsableItem(m_pOwner, m_IUseItemUID);
}

bool GPlayerInteraction::IsGuildStorageInteraction()
{
	return IT_GUILD_STORAGE == GetSelectedIElementInfo().m_nType;
}
