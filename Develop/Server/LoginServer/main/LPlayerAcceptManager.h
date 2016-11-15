#pragma once

class LPlayerAcceptor_FromGameServer;

class LPlayerAcceptManager
{
	typedef map<MUID, LPlayerAcceptor_FromGameServer* > LPlayerAcceptorMap;
public:
	LPlayerAcceptManager();
	virtual ~LPlayerAcceptManager();

	void Update();

	bool Add(LPlayerAcceptor_FromGameServer* pAcceptor);
	void Del(MUID uidConnectionKey);
	LPlayerAcceptor_FromGameServer* FindAcceptor(MUID uidConnectionKey);

	size_t GetSize();

private:
	LPlayerAcceptorMap	m_AcceptorMap;

};
