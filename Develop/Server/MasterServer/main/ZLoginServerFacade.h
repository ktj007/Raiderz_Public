#pragma once

class ZLoginServerInfo;
class ZLoginServerObject;

class ZLoginServerFacade
{
public:
	ZLoginServerFacade();
	~ZLoginServerFacade();

	bool				IsConnected() { return m_UID.IsValid(); }

	bool				Connected(MUID uid, ZLoginServerInfo* pLoginServerInfo);
	void				Disconnect();

	MUID				GetUID();
	int					GetID();

private:
	MUID				m_UID;
	ZLoginServerInfo*	m_pLoginServerInfo;
};
