#include "StdAfx.h"
#include "GUTHelper_Party.h"
#include "GPartySystem.h"
#include "GGlobal.h"
#include "SUnitTestUtil.h"
#include "GMasterServerFacade.h"
#include "GPartyManager.h"
#include "GPlayerObjectManager.h"
#include "GPlayerQuests.h"
#include "GProxyFieldManager.h"
#include "GProxyDynamicFieldMaster.h"
#include "GEntityPlayer.h"

GParty* GUTHelper_Party::CreateParty(GEntityPlayer* pLeader)
{
	MUID uidParty = SUnitTestUtil::NewUID();	
	gsys.pMasterServerFacade->AddProxyParty(uidParty, pLeader->GetUID(), pLeader->GetName(), pLeader->GetCID());
	
	GPartyManager* pPartyManager = gsys.pPartySystem->GetPartyManager();
	GParty* pParty = pPartyManager->NewParty(uidParty);

	pParty->AddMember(pLeader->GetUID(), pLeader->GetName(), pLeader->GetCID());
	pParty->SetLeader(pLeader->GetUID());
	pPartyManager->AddParty(pParty);

	pLeader->SetParty(uidParty);
	pLeader->SetPartyLeader(true);
	
	AddQuest(pParty, pLeader);
	AddField(pParty, pLeader);
	SyncQuestRunner(pParty, pLeader);

	return pParty;
}

GParty* GUTHelper_Party::CreateParty(GEntityPlayer* pLeader, GEntityPlayer* pMember)
{
	GParty* pParty = CreateParty(pLeader);
	AddMember(pParty, pMember);

	return pParty;
}

void GUTHelper_Party::AddMember(GParty* pParty, GEntityPlayer* pMember)
{
	_ASSERT(pParty != NULL);
	_ASSERT(pMember != NULL);

	MUID uidParty = pParty->GetUID();
	gsys.pMasterServerFacade->AddProxyPartyMember(uidParty, pMember->GetUID(), pMember->GetName(), pMember->GetCID());
	
	pParty->AddMember(pMember->GetUID(), pMember->GetName(), pMember->GetCID());
	pMember->SetParty(uidParty);

	AddQuest(pParty, pMember);
	AddField(pParty, pMember);
	SyncQuestRunner(pParty, pMember);
}

void GUTHelper_Party::LeaveMember(GParty* pParty, GEntityPlayer* pMember)
{
	_ASSERT(pParty != NULL);
	_ASSERT(pMember != NULL);

	MUID uidParty = pParty->GetUID();

	if (pMember->GetUID() == pParty->GetAloneMember())
	{
		DeleteParty(pParty);
		pMember->SetParty(MUID::Invalid());
		return;
	}


	if (pParty->GetLeader() == pMember->GetUID())
	{
		MUID uidNewLeader = pParty->FindCandidateForLeader();
		if (uidNewLeader.IsValid())
		{
			gsys.pMasterServerFacade->ChangeProxyPartyLeader(uidParty, uidNewLeader);
			pParty->SetLeader(uidNewLeader);
			pMember->SetPartyLeader(false);
			
			GEntityPlayer* pNewLeader = gmgr.pPlayerObjectManager->GetEntity(uidNewLeader);
			if (pNewLeader != NULL)
			{
				pNewLeader->SetPartyLeader(true);
			}
		}
	}


	gsys.pMasterServerFacade->RemoveProxyParty(uidParty);
	pParty->RemoveMember(pMember->GetUID());
	pMember->SetParty(MUID::Invalid());
	SyncEraseQuestRunner(pParty, pMember);
	RemoveField(pParty, pMember);	
	RemoveQuest(pParty, pMember);	

	if (1 == pParty->GetMemberCount())
	{
		MUID uidAloneMember = pParty->GetAloneMember();
		DeleteParty(pParty);

		GEntityPlayer* pAloneMember = gmgr.pPlayerObjectManager->GetEntity(uidAloneMember);
		if (pAloneMember != NULL)
		{
			pAloneMember->SetParty(MUID::Invalid());
			pAloneMember->SetPartyLeader(false);
		}
	}
}

void GUTHelper_Party::DeleteParty(GParty* pParty)
{
	_ASSERT(pParty != NULL);

	for(partymember_iterator it = pParty->GetMemberBegin(); it != pParty->GetMemberEnd(); it++)
	{
		MUID uidMember = it->first;

		GEntityPlayer* pMember = gmgr.pPlayerObjectManager->GetEntity(uidMember);
		if (pMember != NULL)
		{
			pMember->SetParty(MUID::Invalid());
		}
	}

	MUID uidParty = pParty->GetUID();
	gsys.pMasterServerFacade->RemoveProxyParty(uidParty);
	gsys.pPartySystem->RemoveParty(uidParty);
}

void GUTHelper_Party::DoOfflineMember(GParty* pParty, GEntityPlayer* pMember)
{
	_ASSERT(pParty != NULL);
	_ASSERT(pMember != NULL);


	if (pMember->GetUID() == pParty->GetLeader())
	{
		MUID uidNewLeader = pParty->FindCandidateForLeader();
		if (uidNewLeader.IsValid())
		{
			gsys.pMasterServerFacade->ChangeProxyPartyLeader(pParty->GetUID(), uidNewLeader);
			pParty->SetLeader(uidNewLeader);
			pMember->SetPartyLeader(false);
		}
	}

	pParty->AddOfflineMember(pMember->GetUID());
	SyncEraseQuestRunner(pParty, pMember);
	RemoveField(pParty, pMember);
	RemoveQuest(pParty, pMember);	
}

void GUTHelper_Party::DoOnlineMember(GParty* pParty, GEntityPlayer* pMember)
{
	_ASSERT(pParty != NULL);
	_ASSERT(pMember != NULL);

	pParty->RemoveOfflineMember(pMember->GetUID());

	vector<int> vecQuestID;
	pMember->GetQuests().MakeActiveQuestID(&vecQuestID);
	for each(int id in vecQuestID)
	{	
		pParty->AddQuest(id);
	}
}

void GUTHelper_Party::AddQuest(GParty* pParty, GEntityPlayer* pMember)
{
	vector<int> vecQuestID;
	pMember->GetQuests().MakeActiveQuestID(&vecQuestID);
	for each(int id in vecQuestID)
	{	
		pParty->AddQuest(id);
	}
}

void GUTHelper_Party::RemoveQuest(GParty* pParty, GEntityPlayer* pMember)
{
	vector<int> vecQuestID;
	pMember->GetQuests().MakeActiveQuestID(&vecQuestID);
	for each(int id in vecQuestID)
	{	
		pParty->RemoveQuest(id);
	}
}

void GUTHelper_Party::AddField(GParty* pParty, GEntityPlayer* pMember)
{
	MUID uidFieldGroup = pMember->GetPlayerField().GetDynamicFieldGroupUID();
	if (uidFieldGroup.IsInvalid())		return;

	GProxyFieldManager* pFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
	_ASSERT(pFieldManager != NULL);
	GProxyDynamicFieldMaster* pFieldGroup = pFieldManager->FindFieldGroup(uidFieldGroup);
	if (pFieldGroup == NULL)			return;

	int nFieldGroupID = pFieldGroup->GetInfo()->nID;
	pParty->CreateField(nFieldGroupID, uidFieldGroup);
}

void GUTHelper_Party::RemoveField(GParty* pParty, GEntityPlayer* pMember)
{
	MUID uidFieldGroup = pMember->GetPlayerField().GetDynamicFieldGroupUID();
	if (uidFieldGroup.IsInvalid())		return;

	GProxyFieldManager* pFieldManager = gsys.pMasterServerFacade->GetProxyFieldManager();
	_ASSERT(pFieldManager != NULL);
	GProxyDynamicFieldMaster* pFieldGroup = pFieldManager->FindFieldGroup(uidFieldGroup);
	if (pFieldGroup == NULL)			return;

	int nFieldGroupID = pFieldGroup->GetInfo()->nID;
	pParty->RemoveField(nFieldGroupID);
}

void GUTHelper_Party::SyncQuestRunner(GParty* pParty, GEntityPlayer* pMember)
{
	vector<int> vecQuestID;
	pMember->GetQuests().MakeActiveQuestID(&vecQuestID);
	gsys.pPartySystem->SyncQuestRunner(pParty->GetUID(), pMember->GetUID(), pMember->GetFieldUID(), vecQuestID);
}

void GUTHelper_Party::SyncEraseQuestRunner(GParty* pParty, GEntityPlayer* pMember)
{
	gsys.pPartySystem->SyncEraseQuestRunner(pParty->GetUID(), pMember->GetUID());
}
