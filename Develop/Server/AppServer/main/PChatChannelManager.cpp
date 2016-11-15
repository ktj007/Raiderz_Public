#include "StdAfx.h"
#include "PChatChannelManager.h"
#include "PChatChannel.h"
#include "PCmdRouter_Chat.h"
#include "PChatPlayer.h"
#include "PGlobal.h"
#include "PCommandCenter.h"
#include "CCommandTable_App.h"

PChatChannelManager::PChatChannelManager()
: m_pChatChannel_Auction(NULL)
{
}

PChatChannelManager::~PChatChannelManager()
{
	Clear();
}

void PChatChannelManager::Update(float fDelta)
{
	// 제거 목록에 있는 채널 삭제
	DeleteAllLazilyDeleteChannel();
}

bool PChatChannelManager::AddChannel(PChatChannel* pChannel)
{
	VALID_RET(pChannel, false);
	if (!CheckNewChannelAttribute(pChannel)) return false;

	if (CCT_PARTY == pChannel->GetType())
	{
		MUID uidKey = pChannel->GetPartyUID();
		VALID_RET(uidKey.IsValid(), false);
		m_mapChatChannel_Party.insert(PChatChannelMapByUID::value_type(uidKey, pChannel));
	}
	else if (CCT_FIELD == pChannel->GetType())
	{
		int nKey = pChannel->GetFieldID();
		VALID_RET(INVALID_ID != nKey, false);
		m_mapChatChannel_Field.insert(PChatChannelMapByID::value_type(nKey, pChannel));
	}
	else if (CCT_AUCTION == pChannel->GetType())
	{
		m_pChatChannel_Auction = pChannel;
	}
	else if (CCT_PRIVATE == pChannel->GetType())
	{
		wstring strKey = pChannel->GetName();
		VALID_RET(strKey.size() != 0, false);
		m_mapChatChannel_Private.insert(PChatChannelMapByStr::value_type(strKey, pChannel));
	}

	m_mapChatChannel.insert(PChatChannelMapByUID::value_type(pChannel->GetUID(), pChannel));

	/// 채팅 채널 동기화 정보 라우팅
	PCmdRouter_Chat router;
	router.RouteAddedChannelInfo(pChannel);

	return true;
}

void PChatChannelManager::DeleteChannel(PChatChannel* pChannel)
{
	VALID(pChannel);

	/// 채팅 채널 동기화 정보 라우팅
	PCmdRouter_Chat router;
	router.RouteDeletedChannelInfo(pChannel->GetUID());

	/// 채널 제거
	DeleteChannelReferences(pChannel);
	SAFE_DELETE(pChannel);
}
void PChatChannelManager::LazilyDeleteChannel(PChatChannel* pChannel)
{
	VALID(pChannel);

	/// 지연 제거 등록
	pChannel->SetReserveDelete();
	m_vecChatChannel_ToDelete.push_back(pChannel);

	/// 컨테이너에서 채널 첨조 제거
	DeleteChannelReferences(pChannel);
}
void PChatChannelManager::DeleteChannelReferences(PChatChannel* pChannel)
{
	VALID(pChannel);

	if (CCT_PARTY == pChannel->GetType())
	{
		m_mapChatChannel_Party.erase(pChannel->GetPartyUID());
	}
	else if (CCT_FIELD == pChannel->GetType())
	{
		m_mapChatChannel_Field.erase(pChannel->GetFieldID());
	}
	else if (CCT_AUCTION == pChannel->GetType())
	{
		m_pChatChannel_Auction = NULL;
	}
	else if (CCT_PRIVATE == pChannel->GetType())
	{
		m_mapChatChannel_Private.erase(pChannel->GetName());
	}

	m_mapChatChannel.erase(pChannel->GetUID());
}

void PChatChannelManager::ChangedGameServerID(CID cidPlayer, int nNewGameServerID)
{
	PChatPlayerMapByCID::iterator it = m_mapChatPlayer.find(cidPlayer);
	if (it == m_mapChatPlayer.end())	return;

	PChatPlayer* pPlayer = it->second;
	pPlayer->ChangedGameServerID(nNewGameServerID);

	/// 채팅 채널 플레이어 게임서버 동기화 정보 라우팅
	PCmdRouter_Chat router;
	router.RouteChatPlayerGameServerInfo(cidPlayer, nNewGameServerID);
}

bool PChatChannelManager::EnterChannel(PChatChannel* pChannel, CID cidPlayer, int nGameServerID)
{
	VALID_RET(pChannel != NULL, false);

	PChatPlayer* pPlayer = NULL;
	PChatPlayerMapByCID::iterator it = m_mapChatPlayer.find(cidPlayer);

	if (it == m_mapChatPlayer.end())
	{
		pPlayer = new PChatPlayer(cidPlayer, nGameServerID);
		m_mapChatPlayer.insert(make_pair(cidPlayer, pPlayer));
	}
	else
	{
		pPlayer = it->second;
	}

	MUID uidChannel = pChannel->GetUID();
	pPlayer->AddChannel(uidChannel);
	pChannel->AddPlayer(pPlayer);
		
	return true;
}

void PChatChannelManager::LeaveChannel(PChatChannel* pChannel, CID cidPlayer)
{
	VALID(pChannel != NULL);

	PChatPlayerMapByCID::iterator it = m_mapChatPlayer.find(cidPlayer);
	if (it == m_mapChatPlayer.end())	return;

	
	PChatPlayer* pPlayer = it->second;
	MUID uidChannel = pChannel->GetUID();
	
	pPlayer->RemoveChannel(uidChannel);
	pChannel->RemovePlayer(pPlayer);

	if (pPlayer->IsEmptyChannel())
	{
		SAFE_DELETE(it->second);
		m_mapChatPlayer.erase(it);
	}
}

void PChatChannelManager::LeaveAllChannel(CID cidPlayer)
{
	PChatPlayerMapByCID::iterator it = m_mapChatPlayer.find(cidPlayer);
	if (it == m_mapChatPlayer.end())	return;

	PChatPlayer* pPlayer = it->second;
	vector<MUID> vecChannel;
	pPlayer->GetChannelList(vecChannel);
	
	auto_ptr<MCommand> pCmd(gsys.pCommandCenter->MakeNewCommand(MPC_MSG_LEAVE_CHANNEL_REQ,
																2,
																NEW_INT(cidPlayer),
																NEW_UID()
																));

	for (vector<MUID>::iterator it = vecChannel.begin(); it != vecChannel.end(); it++)
	{
		PChatChannel* pChannel = FindChannel(*it);
		if (pChannel != NULL)
		{
			pCmd->GetParameter(1)->SetData((int8*)&*it, sizeof(MUID));
			pChannel->RunChatModule(CMT_LEAVE_CHANNEL, pCmd.get());
		}
	}
}

PChatChannel* PChatChannelManager::FindChannel(const std::wstring& strChannelName)
{
	PChatChannelMapByStr::iterator iter = m_mapChatChannel_Private.find(strChannelName);
	if (iter != m_mapChatChannel_Private.end())
	{
		return (*iter).second;
	}
	return NULL;
}
PChatChannel* PChatChannelManager::FindChannel(const MUID& uidChannel)
{
	PChatChannelMapByUID::iterator iter = m_mapChatChannel.find(uidChannel);
	if (iter != m_mapChatChannel.end())
	{
		return (*iter).second;
	}
	return NULL;
}
PChatChannel* PChatChannelManager::FindChannel_Party(const MUID& uidParty)
{
	PChatChannelMapByUID::iterator iter = m_mapChatChannel_Party.find(uidParty);
	if (iter != m_mapChatChannel_Party.end())
	{
		return (*iter).second;
	}
	return NULL;
}

PChatChannel* PChatChannelManager::FindChannel_Field(const int nFieldID)
{
	PChatChannelMapByID::iterator it = m_mapChatChannel_Field.find(nFieldID);
	if (it == m_mapChatChannel_Field.end())	return NULL;

	return it->second;
}

PChatChannel* PChatChannelManager::FindChannel_Auction(void)
{
	return m_pChatChannel_Auction;
}

PChatPlayer* PChatChannelManager::FindPlayer(CID cid)
{
	PChatPlayerMapByCID::iterator it = m_mapChatPlayer.find(cid);
	if (it == m_mapChatPlayer.end())	return NULL;

	return it->second;
}

void PChatChannelManager::Clear()
{
	m_mapChatChannel_Party.clear();
	m_mapChatChannel_Field.clear();
	m_pChatChannel_Auction = NULL;
	
	PChatChannelMapByUID::iterator iter = m_mapChatChannel.begin();
	for (; iter != m_mapChatChannel.end(); ++iter)
	{
		PChatChannel* pChannel = (*iter).second;
		SAFE_DELETE(pChannel);
	}
	m_mapChatChannel.clear();

	for(PChatPlayerMapByCID::iterator it = m_mapChatPlayer.begin(); it != m_mapChatPlayer.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	m_mapChatPlayer.clear();

	for (size_t i = 0; i < m_vecChatChannel_ToDelete.size(); ++i)
	{
		SAFE_DELETE(m_vecChatChannel_ToDelete[i]);
	}
	m_vecChatChannel_ToDelete.clear();
}
void PChatChannelManager::DeleteAllLazilyDeleteChannel()
{
	for (size_t i = 0; i < m_vecChatChannel_ToDelete.size(); ++i)
	{
		DeleteChannel(m_vecChatChannel_ToDelete[i]);
	}
	m_vecChatChannel_ToDelete.clear();
}

bool PChatChannelManager::CheckNewChannelAttribute(PChatChannel* pChannel)
{
	VALID_RET(pChannel, false);

	switch(pChannel->GetType())
	{
	case CCT_PARTY:
		if (pChannel->GetPartyUID() == MUID::ZERO) return false;
		break;
	case CCT_PRIVATE:
		if (pChannel->GetOwnerCID() == 0 || pChannel->GetName().size() == 0) return false;
		break;
	case CCT_FIELD:
		if (pChannel->GetFieldID() <= 0)	return false;
		break;
	case CCT_AUCTION:
		// do nothing
		break;
	}
	return true;
}
