#include "stdafx.h"
#include "ZFixedPartyLogic.h"
#include "ZFixedPartyInfoManager.h"
#include "ZPartyManager.h"
#include "ZPlayer.h"
#include "ZPlayerManager.h"
#include "ZParty.h"
#include "ZPlayerCommandRouter.h"
#include "ZPartyMember.h"
#include "ZPartyCommandRouter.h"

ZFixedPartyLogic::ZFixedPartyLogic()
{
	m_pRouter = new ZPartyCommandRouter();
}

ZFixedPartyLogic::~ZFixedPartyLogic()
{
	SAFE_DELETE(m_pRouter);
}

int ZFixedPartyLogic::GetFixedPartyID(wstring strName)
{
	int nFixedPartyID = gmgr.pFixedPartyInfoManager->FindFixedPartyID(strName);
	return nFixedPartyID;
}

ZParty* ZFixedPartyLogic::FindParty(int nFixedPartyID)
{
	waiting_map::iterator it = m_mapWaiting.find(nFixedPartyID);

	if (it == m_mapWaiting.end())
	{
		return NULL;
	}

	FixedPartyNode& node = it->second;	
	ZParty* pParty = gmgr.pPartyManager->Find(node.uidParty);
	
	return pParty;
}

void ZFixedPartyLogic::AddWaiting(int nFixedPartyID, MUID uidMember)
{
	waiting_map::iterator it = m_mapWaiting.find(nFixedPartyID);
	
	if (it == m_mapWaiting.end())
	{
		FixedPartyNode node;
		node.setMemberUID.insert(uidMember);

		m_mapWaiting.insert(make_pair(nFixedPartyID, node));
	}
	else
	{
		FixedPartyNode& node = it->second;
		node.setMemberUID.insert(uidMember);
	}
}

void ZFixedPartyLogic::CreateFixedParty(int nFixedPartyID)
{
	// 등록확인
	waiting_map::iterator it = m_mapWaiting.find(nFixedPartyID);

	if (it == m_mapWaiting.end())
	{
		return;
	}


	// 파티가 이미 있으면 안된다.
	FixedPartyNode& node = it->second;
	
	ZParty* pParty = gmgr.pPartyManager->Find(node.uidParty);

	if (pParty != NULL)
	{
		return;
	}


	// 대기멤버 상태확인
	vector<MUID> vecInvalidPlayer;
	
	for each(const MUID& uidMember in node.setMemberUID)
	{
		ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidMember);

		if (pPlayer == NULL || pPlayer->GetPartyUID().IsValid())
		{
			vecInvalidPlayer.push_back(uidMember);
		}
	}

	for each(const MUID& uidMember in vecInvalidPlayer)
	{
		node.setMemberUID.erase(uidMember);
	}

	
	// 최소 파티원 확인
	if (node.setMemberUID.size() < 2)
	{
		return;
	}


	// 파티 생성
	for each(const MUID& uidMember in node.setMemberUID)
	{
		ZPlayer* pPlayer = gmgr.pPlayerManager->FindByUID(uidMember);

		int nGameServerID = pPlayer->GetGameServerID();

		if (pParty == NULL)
		{
			pParty = gmgr.pPartyManager->AddParty(uidMember, pPlayer->GetPlayerName(), nGameServerID, pPlayer->GetCID());
			node.uidParty = pParty->GetUID();
		}
		else
		{
			pParty->AddMember(uidMember, pPlayer->GetPlayerName(), nGameServerID, pPlayer->GetCID());
		}

		pPlayer->SetPartyUID(node.uidParty);

		ZPlayerCommandRouter::RouteProxyPlayerUpdate(pPlayer);
	}


	// 파티 정보 동기화	
	for each(const MUID& uidMember in node.setMemberUID)
	{		
		ZPartyMember* pMember = pParty->GetMember(uidMember);

		if (MAX_PARTY_MEMBER_COUNT <= pParty->GetCount())
		{
			return;
		}

		if (pMember->GetUID() == pParty->GetLeader())
		{
			m_pRouter->PartyAdd(node.uidParty, pMember->GetUID(), pMember->GetName(), pMember->GetCID());		
		}
		else
		{
			m_pRouter->AddMember(node.uidParty, pMember->GetUID(), pMember->GetName(), pMember->GetCID());
		}
	}


	// 이름 설정
	wstring strPartyName = gmgr.pFixedPartyInfoManager->FindFixedPartyName(nFixedPartyID);	
	pParty->SetName(strPartyName);
	m_pRouter->ChangeNameRes(node.uidParty, strPartyName);
}

int ZFixedPartyLogic::GetWaitingPartyCount(void)
{
	int nSize = m_mapWaiting.size();
	return nSize;
}

int ZFixedPartyLogic::GetWaitingMemberCount(int nFixedPartyID)
{
	waiting_map::iterator it = m_mapWaiting.find(nFixedPartyID);
	
	if (it == m_mapWaiting.end())
	{
		return 0;
	}

	FixedPartyNode& node = it->second;	
	int nSize = node.setMemberUID.size();	

	return nSize;
}
