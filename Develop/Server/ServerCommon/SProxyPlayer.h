#pragma once

#include "ServerCommonLib.h"
#include "SDef.h"
#include "MTstring.h"


struct TD_PROXY_PLAYER_INFO;

class SCOMMON_API SProxyPlayer
{
public:
	SProxyPlayer(CID cid);
	virtual ~SProxyPlayer();

	bool Update(const TD_PROXY_PLAYER_INFO& info);

	AID GetAID(void) const;
	CID GetCID(void) const;
	MUID GetUID(void) const;
	wstring GetName(void) const;
	int GetGameServerID(void) const;
	MUID GetFieldUID(void) const;
	MUID GetPartyUID(void) const;
	int GetFieldID(void) const;
	int GetChannelID(void) const;
	MUID GetFieldGroupUID(void) const;

	void SetUID(MUID uid);
	void SetName(wstring strName);
	void SetGameServerID(int nGameServerID);
	void SetFieldUID(MUID uidField);
	void SetPartyUID(MUID uidParty);
	void SetFieldID(int nFieldID);
	void SetChannelID(int nChannelID);
	void SetFieldGroupUID(MUID uidFieldGroup);

private:
	AID				m_nAID;
	CID				m_CID;
	MUID			m_UID;
	wstring			m_strName;
	int				m_nGameServerID;
	MUID			m_uidField;
	MUID			m_uidParty;
	int				m_nGuildID;
	int				m_nFieldID;
	int				m_nChannelID;
	MUID			m_uidFieldGroup;
};
