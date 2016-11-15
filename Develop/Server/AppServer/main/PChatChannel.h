#pragma once

#include "PDef.h"
#include "PDef_Chatting.h"
#include "PChatModuleBase.h"
#include "STypes.h"


class PChatModuleMap : public std::map<CHATTING_MODULE_TYPE, PChatModuleBase* >
{
public:
	void AddModule(CHATTING_MODULE_TYPE eModule, PChatModuleBase* pChatModule)
	{
		this->insert(value_type(eModule, pChatModule));
	}
	void Clear()
	{
		PChatModuleMap::iterator itor = this->begin();
		for ( ; itor != this->end(); ++itor)
		{
			SAFE_DELETE((*itor).second);
		}
		this->clear();
	}
};

//////////////////////////////////////////////////////////////////////////

typedef std::set<CID>		PChatPlayerCIDSet;
typedef std::set<wstring>	PChatPlayerNameSet;
typedef std::list<CID>		PChatPlayerCIDList;

class PChatPlayer;
class PChatChannel
{
public:
	PChatChannel(const MUID& uid, CHATTING_CHANNEL_TYPE eType);
	virtual ~PChatChannel();

	bool					Init(int nOwnerCID, const std::wstring& strChannelName);
	bool					Init(const MUID& uidParty);	
	bool					Init_Field(int nFieldID);

	void					AddPlayer(PChatPlayer* pChatPlayer);
	void					RemovePlayer(PChatPlayer* pChatPlayer);
	void					ChangedGameServerID(PChatPlayer* pChatPlayer, int nNewGameServerID);

	void					KickPlayer(CID cidPlayer);	

	void					AddBanPlayer(wstring strPlayerName);
	void					RemoveBanPlayer(wstring strPlayerName);

	void					RunChatModule(CHATTING_MODULE_TYPE eModule, minet::MCommand* pCmd);		
	PChatModuleBase*		FindChatModules(CHATTING_MODULE_TYPE eModule);

	PChatModuleMap&			GetChatModules()	{ return m_mapChatModules; }
	MUID					GetUID()			{ return m_UID; }
	CHATTING_CHANNEL_TYPE	GetType()			{ return m_eType; }
	std::wstring&			GetName()			{ return m_strChannelName; }
	CID						GetOwnerCID()		{ return m_cidOwner; }
	MUID					GetPartyUID()		{ return m_uidParty; }
	int						GetFieldID()		{ return m_nFieldID;	}
	size_t					GetPlayerCount()	{ return m_setPlayer.size(); }
	bool					IsEmptyPlayer()		{ return m_setPlayer.empty(); }
	bool					IsExistPlayer(CID cidPlayer);
	bool					IsBanedPlayer(wstring& strPlayerName);
	bool					IsReserveDelete()	{ return m_bIsReserveDelete; }

	void					GetPlayerGameServerIDVec(std::vector<int>& refVecGameServerID);
	CID						GetFirstPlayerCID();

	void					SetOwnerCID(CID cidPlayer)	{ m_cidOwner = cidPlayer; }
	void					SetReserveDelete() { m_bIsReserveDelete = true; }

private:
	MUID					m_UID;
	CHATTING_CHANNEL_TYPE	m_eType;
	bool					m_bIsReserveDelete;

	/// 사설 채팅 채널일 경우만 쓰입니다.
	std::wstring				m_strChannelName;
	int						m_cidOwner;

	/// 파티 채팅 채널일 경우만 쓰입니다.
	MUID					m_uidParty;

	/// 필드 채팅 채널일 경우만 쓰입니다.
	int						m_nFieldID;

	PChatModuleMap			m_mapChatModules;
	PChatPlayerCIDSet		m_setPlayer;
	PChatPlayerNameSet		m_setBanedPlayerName;
	PChatPlayerCIDList		m_listSequencedPlayer;

	void					InsertPlayer(CID cidPlayer);
	void					ErasePlayer(CID cidPlayer);

private:
	/// Utility
	class PGameServerIDCounterMgr
	{
		friend PChatChannel;
		typedef std::map<int, int>	PIDCounterMap;	/// 키 : GameServerID, 값 : 카운터
	public:
		void AddGameServerID(int nGameServerID);
		void DelGameServerID(int nGameServerID);
	private:
		PIDCounterMap	m_mapIDCounter;

	};

	PGameServerIDCounterMgr	m_GameServerIDCounterMgr;

};
