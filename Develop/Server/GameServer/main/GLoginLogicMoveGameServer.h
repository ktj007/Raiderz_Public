#pragma once

class GPlayerAcceptManager;
class GPlayerAcceptor_FromGameServer;
class GPlayerAcceptor;
class GPlayerObject;

class GLoginLogicMoveGameServer
{
public:
	GLoginLogicMoveGameServer();
	~GLoginLogicMoveGameServer();

	bool			Init(MUID uidPlayerSession, MUID uidConnectionKey);
	bool			Check();
	bool			Apply(bool bOnPmang=false, wstring strPmangStatIndex=L"");

	MUID			GetRealPlayerUID() { return m_uidRealPlayerUID; }

	static bool		IsExistPastPlayerObject(MUID uidPlayer);

private:
	GPlayerAcceptManager*			m_pGlobalPlayerAcceptManager;
	GPlayerAcceptor_FromGameServer* m_pAcceptorRef;
	MUID			m_uidPlayerSession;
	MUID			m_uidConnectionKey;
	MUID			m_uidRealPlayerUID;

	void			DelAcceptor();
	void			NewLazyLoginMoveGameServerTask(bool bOnPmang=false, wstring strPmangStatIndex=L"");
	GPlayerObject*	NewPlayerObjectAndInitAccount(MUID uidPlayer, GPlayerAcceptor* pAcceptor);

};
