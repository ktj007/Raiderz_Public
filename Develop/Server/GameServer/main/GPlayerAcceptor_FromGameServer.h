#pragma once

#include "GPlayerAcceptor.h"
#include "GPlayerGameData.h"

class GPlayerAcceptor_FromGameServer : public GPlayerAcceptor, public MTestMemPool<GPlayerAcceptor_FromGameServer>
{
protected:
	MUID				m_uidPlayerUID;
	GPlayerGameData		m_PlayerGameData;

public:
	GPlayerAcceptor_FromGameServer() : GPlayerAcceptor(), m_uidPlayerUID(MUID::ZERO) {}
	virtual ~GPlayerAcceptor_FromGameServer() {}

	void ReserveLogin(MCommand* pCommand);
	bool SerializePlayerGameData(GEntityPlayer* pEntityPlayer);

	MUID	GetPlayerUID()			{ return m_uidPlayerUID; }
	virtual AcceptorType GetType()	{ return FROM_GAME_SERVER; }

	void OnTimeout() override;
};
