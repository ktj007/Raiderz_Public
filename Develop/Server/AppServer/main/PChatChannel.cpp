#include "StdAfx.h"
#include "PChatChannel.h"
#include "PChatModuleBase.h"
#include "PCmdRouter_Chat.h"
#include "PChatPlayer.h"
#include "PGlobal.h"
#include "PChatChannelManager.h"
#include "PChatPlayer.h"

PChatChannel::PChatChannel(const MUID& uid, CHATTING_CHANNEL_TYPE eType)
: m_UID(uid)
, m_eType(eType)
, m_bIsReserveDelete(false)
, m_cidOwner(0)
, m_uidParty(MUID::ZERO)
, m_nFieldID(INVALID_ID)
{
	VALID(m_UID.IsValid());
}

PChatChannel::~PChatChannel()
{
	m_setPlayer.clear();
	m_setBanedPlayerName.clear();
	m_listSequencedPlayer.clear();

	m_mapChatModules.Clear();
}

bool PChatChannel::Init(int nOwnerCID, const std::wstring& strChannelName)
{
	VALID_RET(CCT_PRIVATE == m_eType, false);
	VALID_RET(0 != nOwnerCID, false);
	VALID_RET(0 != strChannelName.size(), false);

	m_cidOwner = nOwnerCID;
	m_strChannelName = strChannelName;
	return true;
}
bool PChatChannel::Init(const MUID& uidParty)
{
	VALID_RET(CCT_PARTY == m_eType, false);
	VALID_RET(uidParty.IsValid(), false);

	m_uidParty = uidParty;
	return true;
}

bool PChatChannel::Init_Field(int nFieldID)
{
	if (nFieldID == INVALID_ID)	return false;
	if (nFieldID <= 0)	return false;
	
	m_nFieldID = nFieldID;
	return true;
}

void PChatChannel::AddPlayer(PChatPlayer* pChatPlayer)
{
	VALID(pChatPlayer);

	InsertPlayer(pChatPlayer->GetCID());
	m_GameServerIDCounterMgr.AddGameServerID(pChatPlayer->GetGameServerID());
	
	PCmdRouter_Chat router;
	router.RouteAddedChatPlayerInfo(GetUID(), pChatPlayer->GetCID());
}

void PChatChannel::RemovePlayer(PChatPlayer* pChatPlayer)
{
	VALID(pChatPlayer);

	PCmdRouter_Chat router;
	router.RouteDeletedChatPlayerInfo(GetUID(), pChatPlayer->GetCID());

	ErasePlayer(pChatPlayer->GetCID());
	m_GameServerIDCounterMgr.DelGameServerID(pChatPlayer->GetGameServerID());
}

void PChatChannel::ChangedGameServerID(PChatPlayer* pChatPlayer, int nNewGameServerID)
{
	VALID(pChatPlayer != NULL);

	// 채널 멤버 확인
	CID cidPlayer = pChatPlayer->GetCID();
	if (!IsExistPlayer(cidPlayer))	return;

	// 게임서버 변경
	int nOldGameServerID = pChatPlayer->GetGameServerID();
	m_GameServerIDCounterMgr.DelGameServerID(nOldGameServerID);
	m_GameServerIDCounterMgr.AddGameServerID(nNewGameServerID);
}

void PChatChannel::RunChatModule(CHATTING_MODULE_TYPE eModule, minet::MCommand* pCmd)
{
	VALID(pCmd);
	if (NULL == m_mapChatModules[eModule]) return;

	m_mapChatModules[eModule]->Run(pCmd);
}

PChatModuleBase* PChatChannel::FindChatModules(CHATTING_MODULE_TYPE eModule)
{
	PChatModuleMap::iterator it = m_mapChatModules.find(eModule);
	if (it == m_mapChatModules.end())	return NULL;

	return it->second;
}

bool PChatChannel::IsExistPlayer(CID cidPlayer)
{
	PChatPlayerCIDSet::iterator it = m_setPlayer.find(cidPlayer);
	return it != m_setPlayer.end();
}

bool PChatChannel::IsBanedPlayer(wstring& strPlayerName)
{
	PChatPlayerNameSet::iterator it = m_setBanedPlayerName.find(strPlayerName);
	return it != m_setBanedPlayerName.end();
}

void PChatChannel::AddBanPlayer(wstring strPlayerName)
{
	m_setBanedPlayerName.insert(strPlayerName);
}
void PChatChannel::RemoveBanPlayer(wstring strPlayerName)
{
	m_setBanedPlayerName.erase(strPlayerName);
}

void PChatChannel::GetPlayerGameServerIDVec(std::vector<int>& refVecGameServerID)
{
	PGameServerIDCounterMgr::PIDCounterMap::iterator it = m_GameServerIDCounterMgr.m_mapIDCounter.begin();
	for (; it != m_GameServerIDCounterMgr.m_mapIDCounter.end(); ++it)
	{
		refVecGameServerID.push_back(it->first);	///< key 는 GameServer의 ID 입니다.
	}
}

CID PChatChannel::GetFirstPlayerCID()
{
	if (0 == m_listSequencedPlayer.size()) return 0;

	PChatPlayerCIDList::iterator it = m_listSequencedPlayer.begin();
	return (*it);
}

void PChatChannel::InsertPlayer(CID cidPlayer)
{
	m_setPlayer.insert(cidPlayer);
	m_listSequencedPlayer.push_back(cidPlayer);
}

void PChatChannel::ErasePlayer(CID cidPlayer)
{
	m_setPlayer.erase(cidPlayer);

	PChatPlayerCIDList::iterator itr = m_listSequencedPlayer.begin();
	for (; itr != m_listSequencedPlayer.end(); ++ itr)
	{
		if ((*itr) == cidPlayer)
		{
			m_listSequencedPlayer.erase(itr);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PChatChannel::PGameServerIDCounterMgr::AddGameServerID(int nGameServerID)
{
	PIDCounterMap::iterator it =  m_mapIDCounter.find(nGameServerID);
	if (it == m_mapIDCounter.end())
	{
		int nFirstCount = 1;
		m_mapIDCounter.insert(PIDCounterMap::value_type(nGameServerID, nFirstCount));
		return;
	}

	int& refCounter = it->second;
	refCounter++;
}
void PChatChannel::PGameServerIDCounterMgr::DelGameServerID(int nGameServerID)
{
	PIDCounterMap::iterator it =  m_mapIDCounter.find(nGameServerID);
	if (it == m_mapIDCounter.end())
		return;

	int& refCounter = it->second;
	refCounter--;

	if (refCounter <= 0)
	{
		m_mapIDCounter.erase(it);
	}
}
