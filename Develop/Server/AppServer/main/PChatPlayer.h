#pragma once

#include "STypes.h"

class PChatChannel;

class PChatPlayer
{
public:
	PChatPlayer(CID cid, int nGameServer);
	virtual ~PChatPlayer();

	void AddChannel(MUID uidChannel);
	void RemoveChannel(MUID uidChannel);
	
	void ChangedGameServerID(int nGameServerID);

	CID GetCID(void) const;	
	int GetGameServerID(void) const;
	bool IsEmptyChannel(void) const;
	void GetChannelList(vector<MUID>& vecChannel);
	int GetEnteredPrivateChannelCount();

private:
	CID	m_cid;
	int m_nGameServerID;
	set<MUID> m_setChannel;
};
