#pragma once

#include "GPlayerAcceptor.h"

class GPlayerAcceptor_FromLoginServer : public GPlayerAcceptor, public MTestMemPool<GPlayerAcceptor_FromLoginServer>
{
protected:
	int32		m_nCID;
	MUID		m_uidPlayer_OnLoginServer;
public:
	GPlayerAcceptor_FromLoginServer() : GPlayerAcceptor(), m_nCID(0), m_uidPlayer_OnLoginServer(MUID::ZERO) {}
	virtual ~GPlayerAcceptor_FromLoginServer() {}
	virtual AcceptorType GetType() { return FROM_LOGIN_SERVER; }

	// reserve
	void ReserveLogin(TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo, MUID uidConnectionKey, MUID uidPlayerOnLoginServer=MUID::ZERO);

	// fake login
	void MakeFakeReserveLogin( MUID uidConnectionKey, TD_LOGIN_ACCOUNT_INFO* pLoginAccountInfo );
	void PostResponseSelMyChar(int nRet, MUID uidPlayer);

	int32	GetCID()				{ return m_nCID; }
	MUID	GetPlayerUID()			{ return m_uidPlayer_OnLoginServer; }

	void OnTimeout() override;
};
