#ifndef _GSERVER_H
#define _GSERVER_H

#include "MSingleton.h"
#include "MTime.h"
#include "GNetServer.h"
#include "GserverInfo.h"
#include "GServerSystems.h"

class GCmdHandler_Log;
class GCommandCenter;

/// 실제 서버 메인 클래스
class GServer
{
protected:
	friend class GNetServer;

	bool						m_bCreated;
	GServerInfo					m_ServerInfo;					///< 서버 정보
	GNetServer*					m_pNetServer;					///< 네트워크 관리
	GCommandCenter*				m_pCommandCenter;
	MRegulator*					m_pServerInfoViewRglt;

private:
	void	WriteServerInfoLog();
	void	UpdateServerInfoView();

protected:
	bool	InitDB();		
	virtual void	UpdateForDebug( float fDelta );
	void	UpdateLog();

public:
	GServer();
	virtual ~GServer();

	virtual bool Create();
	virtual void Destroy();
	virtual void Update(float fDelta);

	void Disconnect(MUID& uidGameServer);

	virtual MUID	NewUID()			{ return m_pNetServer->NewUID(); }
			MUID	GetUID()			{ return m_pNetServer->GetUID(); }
	bool			IsCreated()			{ return m_bCreated; }
	GServerInfo&	GetServerInfo()		{ return m_ServerInfo; }
};


MCommand* MakeNewCommand(int nCmdID, MUID TargetUID, int nParamCount, MCommandParameter* pCmdParam, ... );
MCommand* MakeNewCommand(int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... );


#endif