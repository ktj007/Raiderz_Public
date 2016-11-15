#pragma once

#include "GPlayerAcceptor.h"

class GFakeLoginPlayerAcceptor_FromGameServerToLoginServer : public GPlayerAcceptor, public MTestMemPool<GFakeLoginPlayerAcceptor_FromGameServerToLoginServer>
{
public:
	GFakeLoginPlayerAcceptor_FromGameServerToLoginServer(void);
	~GFakeLoginPlayerAcceptor_FromGameServerToLoginServer(void);

	void ReserveLogin(MUID uidConnectionKey, AID nAID, wstring strUserID);

	void OnTimeout() override;
	AcceptorType GetType() override	{ return FAKE_LOGIN_FROM_GAME_SERVER_TO_LOGIN_SERVER; }
};
