#pragma once

class LPlayerAcceptor_FromGameServer
{
public:
	LPlayerAcceptor_FromGameServer(void);
	~LPlayerAcceptor_FromGameServer(void);

	void			ReserveAccept(MUID uidConnectionKey, AID nAID, const wstring& strUserID, MUID uidPlayerFromGameServer);
	void			OnTimeout();

	MUID			GetConnectionKey() const	{ return m_uidConnectionKey; }
	AID				GetAID() const				{ return m_nAID; }
	const wstring&	GetName()					{ return m_strUserID; }
	MUID			GetUID() const				{ return m_uidPlayerFromGameServer; }

	uint32			GetReserveTime()			{ return m_nReserveTime; }

private:
	MUID	m_uidConnectionKey;
	AID		m_nAID;
	wstring	m_strUserID;
	MUID	m_uidPlayerFromGameServer;

	uint32	m_nReserveTime;
};
