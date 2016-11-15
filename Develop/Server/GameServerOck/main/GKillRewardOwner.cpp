#include "StdAfx.h"
#include "GKillRewardOwner.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GParty.h"
#include "GPartySystem.h"


GKillRewardOwner::GKillRewardOwner()
: m_nCanditateRank(0)
{
}

GKillRewardOwner::~GKillRewardOwner(void)
{
}

bool GKillRewardOwner::IsCandidate(int nCID)
{
	return (m_mapCandidateCID.end() != m_mapCandidateCID.find(nCID));
}

bool GKillRewardOwner::IsEmpty()
{
	return m_mapCandidateCID.empty();
}

void GKillRewardOwner::GetBeneficiaryAndParty(vector<int>& outvecBeneficiaryCID, MUID& outnPartyUID)
{	
	outnPartyUID = MUID::ZERO;

	int nRewardOwnerCID = GetRewardOwnerCID();
	if (0 == nRewardOwnerCID) return;

	// 현재 소유권자의 파티를 알수 있으면 현재파티로
	// 알수 없으면 소유권자로 등록될때의 파티로 보상자 산출
	GEntityPlayer* pRewardOwner = gmgr.pPlayerObjectManager->GetEntity(nRewardOwnerCID);
	if (NULL != pRewardOwner)
	{
		outnPartyUID = pRewardOwner->GetPartyUID();
	}
	else
	{
		outnPartyUID = GetRewardOwnerPastPartyUID();
	}

	GParty* pParty = gsys.pPartySystem->FindParty(outnPartyUID);
	if (NULL == pParty)
	{
		outvecBeneficiaryCID.push_back(nRewardOwnerCID);
	}
	else
	{
		vector<int> vecMemberCID = pParty->CollectMemberCID();
		for each (int nMemberCID in vecMemberCID)
		{
			if (false == IsCandidate(nMemberCID)) continue;

			outvecBeneficiaryCID.push_back(nMemberCID);
		}
	}
}

void GKillRewardOwner::InsertBeneficiaryAndParty( const vector<int>& vecBeneficiaryCID, const MUID& nPartyUID )
{
	if (true ==	vecBeneficiaryCID.empty()) return;

	int nSize = static_cast<int>(vecBeneficiaryCID.size());

	Insert(vecBeneficiaryCID.front(), nPartyUID);
	for (int i=1; i<nSize; i++)
	{
		Insert(vecBeneficiaryCID[i], MUID::ZERO);
	}
}


int GKillRewardOwner::GetRewardOwnerCID()
{
	if (true == m_mapCandidateRank.empty()) return 0;

	return m_mapCandidateRank.begin()->second.m_nCID;
}

MUID GKillRewardOwner::GetRewardOwnerPastPartyUID()
{
	if (true == m_mapCandidateRank.empty()) return 0;

	return m_mapCandidateRank.begin()->second.m_nPartyUID;
}

void GKillRewardOwner::Insert(int nCID, MUID nPartyUID)
{
	if (m_mapCandidateCID.end() != m_mapCandidateCID.find(nCID)) return;

	int nCandidateRank = NewCandidateRank();

	m_mapCandidateRank.insert(MAP_CANDIDATE_RANK::value_type(nCandidateRank, PARTYUID_CID(nCID, nPartyUID)));
	m_mapCandidateCID.insert(MAP_CANDIDATE_CID::value_type(nCID, nCandidateRank));
}

void GKillRewardOwner::Delete(int nCID)
{
	MAP_CANDIDATE_CID::iterator itor = m_mapCandidateCID.find(nCID);
	if (m_mapCandidateCID.end() == itor) return;

	int nCandidateRank = itor->second;

	m_mapCandidateRank.erase(nCandidateRank);
	m_mapCandidateCID.erase(nCID);
}

void GKillRewardOwner::Delete(const vector<int>& vecCID)
{
	for each (int nCID in vecCID)
	{
		Delete(nCID);
	}
}

void GKillRewardOwner::Clear()
{
	m_mapCandidateRank.clear();
	m_mapCandidateCID.clear();
	m_nCanditateRank = 0;
}

int GKillRewardOwner::NewCandidateRank()
{
	return (m_nCanditateRank++);
}