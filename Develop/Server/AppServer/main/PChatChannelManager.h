#pragma once

#include "STypes.h"

class PChatChannel;
class PChatPlayer;

class PChatChannelManager
{
public:
	typedef std::map<MUID, PChatChannel* >		PChatChannelMapByUID;
	typedef std::map<int, PChatChannel* >		PChatChannelMapByID;
	typedef std::map<wstring, PChatChannel* >	PChatChannelMapByStr;
	typedef std::map<CID, PChatPlayer* >		PChatPlayerMapByCID;

	typedef std::vector<PChatChannel* >			PChatChannelVec;

public:
	PChatChannelManager();
	~PChatChannelManager();

	void					Update(float fDelta);

	bool					AddChannel(PChatChannel* pChannel);
	void					DeleteChannel(PChatChannel* pChannel);
	void					LazilyDeleteChannel(PChatChannel* pChannel);
	
	void					ChangedGameServerID(CID cidPlayer, int nNewGameServerID);
	bool					EnterChannel(PChatChannel* pChannel, CID cidPlayer, int nGameServerID);
	void					LeaveChannel(PChatChannel* pChannel, CID cidPlayer);
	void					LeaveAllChannel(CID cidPlayer);

	PChatChannel*			FindChannel(const std::wstring& strChannelName);
	PChatChannel*			FindChannel(const MUID& uidChannel);
	PChatChannel*			FindChannel_Party(const MUID& uidParty);
	PChatChannel*			FindChannel_Field(const int nFieldID);
	PChatChannel*			FindChannel_Auction(void);
	
	PChatPlayer*			FindPlayer(CID cid);

	int						GetChannelCount()			{ return (int)m_mapChatChannel.size(); }
	int						GetChannelCount_Party()		{ return (int)m_mapChatChannel_Party.size(); }

	PChatChannelMapByUID&	GetChannelMap()				{ return m_mapChatChannel; }
	PChatPlayerMapByCID&	GetChatPlayerMap()			{ return m_mapChatPlayer; }

private:
	PChatChannelMapByUID	m_mapChatChannel;
	PChatChannelMapByUID	m_mapChatChannel_Party;
	PChatChannelMapByID		m_mapChatChannel_Field;
	PChatChannelMapByStr	m_mapChatChannel_Private;
	PChatChannel*			m_pChatChannel_Auction;
	
	PChatPlayerMapByCID		m_mapChatPlayer;

	PChatChannelVec			m_vecChatChannel_ToDelete;

	void					Clear();
	void					DeleteAllLazilyDeleteChannel();
	bool					CheckNewChannelAttribute(PChatChannel* pChannel);

	void					DeleteChannelReferences(PChatChannel* pChannel);
};
