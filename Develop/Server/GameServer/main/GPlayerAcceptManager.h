#pragma once

#include "GPlayerAcceptor.h"

class GPlayerAcceptor_FromLoginServer;
class GPlayerAcceptor_FromGameServer;
class GFakeLoginPlayerAcceptor_FromGameServerToLoginServer;

class GPlayerAcceptManager : public MTestMemPool<GPlayerAcceptManager>
{
private:
	class GPlayerAcceptorMap : public map<MUID, GPlayerAcceptor*>
	{
	public:
		GPlayerAcceptorMap() {}
		virtual ~GPlayerAcceptorMap() {}
		void Update(uint32 nNowTime);
		void Clear();
		bool Add(GPlayerAcceptor* pAcceptor);
		void Del(MUID uidConnectionKey);
		GPlayerAcceptor* FindPlayerAcceptor(MUID uidConnectionKey);
	};
private:
	GPlayerAcceptorMap			m_LoginServerAcceptorMap;
	GPlayerAcceptorMap			m_GameServerAcceptorMap;
public:
	GPlayerAcceptManager();
	virtual ~GPlayerAcceptManager();
	void Update();

	bool Add(GPlayerAcceptor* pAcceptor, GPlayerAcceptor::AcceptorType nAcceptorType);
	void Del(MUID uidConnectionKey, GPlayerAcceptor::AcceptorType nAcceptorType);
	GPlayerAcceptor_FromLoginServer* FindPlayerAcceptor_FromLoginServer(MUID uidConnectionKey);
	GPlayerAcceptor_FromGameServer* FindPlayerAcceptor_FromGameServer(MUID uidConnectionKey);

	GFakeLoginPlayerAcceptor_FromGameServerToLoginServer* FindFakeLoginPlayerAcceptor_FromGameServerToLoginServer(MUID uidConnectionKey);

	size_t GetSize(GPlayerAcceptor::AcceptorType nAcceptorType);
};
