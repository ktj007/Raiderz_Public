#include "StdAfx.h"
#include "GNPCRewardOwner.h"
#include "GEntityNPC.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GParty.h"
#include "GPartySystem.h"
#include "GHateTable.h"

GNPCRewardOwner::GNPCRewardOwner(GEntityNPC* pOwner)
: m_pOwner(pOwner)
{
	m_pOwner->AttachObserver(this);
}

GNPCRewardOwner::~GNPCRewardOwner(void)
{
	m_pOwner->DetachObserver(this);
}

void GNPCRewardOwner::OnAddHateEntry(GEntityActor* pEnemy)
{
	if (NULL == pEnemy) return;
	if (false == pEnemy->IsPlayer()) return;

	GEntityPlayer* pPlayer = ToEntityPlayer(pEnemy);

	Insert(pPlayer->GetCID(), pPlayer->GetPartyUID());
}

void GNPCRewardOwner::OnAddHateEntryMember(GEntityActor* pEnemyMember)
{
	if (NULL == pEnemyMember) return;
	if (false == pEnemyMember->IsPlayer()) return;

	GEntityPlayer* pMemberPlayer = ToEntityPlayer(pEnemyMember);

	Insert(pMemberPlayer->GetCID(), pMemberPlayer->GetPartyUID());
}

void GNPCRewardOwner::OnLostHateEntry(MUID nEnemy)
{
	GEntityPlayer* pEnemyPlayer = gmgr.pPlayerObjectManager->GetEntity(nEnemy);
	if (NULL == pEnemyPlayer) return;

	vector<int> vecDeletableCandidateCID = SelectDeletableCandidateCID(pEnemyPlayer);

	Delete(vecDeletableCandidateCID);
}

void GNPCRewardOwner::OnEmptyHateTable()
{
	Clear();
}

vector<int> GNPCRewardOwner::SelectDeletableCandidateCID(GEntityPlayer* pPlayer)
{
	MUID nPartyUID = pPlayer->GetPartyUID();
	GParty* pParty = gsys.pPartySystem->FindParty(nPartyUID);

	vector<int> vecDeletableCandidataCID;

	// 헤이트 테이블에 등재된 파티원이 하나도 없으면 그때 제거된다.
	if (NULL == pParty)
	{
		vecDeletableCandidataCID.push_back(pPlayer->GetCID());
	}
	else
	{
		bool bExistMemberInHateTable = false;

		for (partymember_iterator iter = pParty->GetMemberBegin(); iter != pParty->GetMemberEnd(); iter++)
		{
			MUID uidMember = iter->first;

			if (true == m_pOwner->GetHateTable().IsExist(uidMember))
			{
				bExistMemberInHateTable = true;
				break;
			}
		}

		if (false == bExistMemberInHateTable)
		{
			vecDeletableCandidataCID = pParty->CollectMemberCID();
		}
	}

	return vecDeletableCandidataCID;
}