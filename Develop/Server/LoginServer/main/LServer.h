#pragma once

#include "MSingleton.h"
#include "MTime.h"
#include "LNetServer.h"
#include "LServerInfo.h"
#include "LServerSystems.h"
#include "SFrameCounter.h"

class LCmdHandler_Master;
class LCommandCenter;
class LMasterNetClient;
class LServerStatusUpdater;
class LTimeoutManager;
class LPlayerLoginGameServerProcManager;
class LPlayerLoginGameServerRetryer;
class LMasterClient;
class LServableChecker;
class SCommandProfileController;
class LWorldLocatorServer;

/// 실제 서버 메인 클래스
class LServer
{
protected:
	friend class LNetServer;

	bool						m_bCreated;
	bool						m_bExiting;						///< 서버가 종료되는 중인지 여부
	LServerInfo					m_ServerInfo;					///< 서버 정보
	minet::MNetServer*			m_pNetServer;					///< 네트워크 관리
	LWorldLocatorServer*		m_pWorldLocatorServer;			///< 월드선택 UDP 서버
	LCommandCenter*				m_pCommandCenter;
	SCommandProfileController*	m_pCommandProfileController;
	MRegulator*					m_pServerInfoViewRglt;
	LMasterClient*				m_pMasterClient;				///< 마스터 서버 클라이언트
	SFrameCounter				m_FrameCounter;
	LServerStatusUpdater*		m_pServerStatusUpdater;
	LServableChecker*			m_pServableChecker;
	
	LPlayerLoginGameServerProcManager*	m_pPlayerLoginGameServerProcManager;
	LPlayerLoginGameServerRetryer*		m_pPlayerLoginGameServerRetryer;

private:
	void	WriteServerInfoLog();
	void	UpdateServerInfoView();
	void	DeInitInfo();
protected:
	virtual bool CreateNetwork();
	virtual void DestroyNetwork();

	virtual void	SetDsnFactory();
	bool			InitDB();	
	virtual void	ReleaseDB();
	virtual bool	InitInfo();

	virtual void	UpdateForDebug( float fDelta );
public:
	LServer();
	virtual ~LServer();

	virtual bool Create();
	virtual void Destroy();
	virtual bool Update(float fDelta);
	
	virtual void DisconnectPlayer(MUID uidPlayer);
	void HardDisconnectPlayer(MUID uidPlayer);

	static void OnServerStartCallback();

	void Exit(); // 서버 종료 처리

	void StartCommandProfile();
	void FinishCommandProfile();

	virtual MUID			NewUID()				{ return m_pNetServer->NewUID(); }
			MUID			GetUID()				{ return m_pNetServer->GetUID(); }
	bool					IsCreated()				{ return m_bCreated; }
	LServerInfo&			GetServerInfo()			{ return m_ServerInfo; }
	LServableChecker&		GetServableChecker()	{ return *m_pServableChecker; }

	LMasterClient*			GetMasterClient()		{ return m_pMasterClient; }
	LNetServer*				GetNetServer()			{ return dynamic_cast<LNetServer*>(m_pNetServer); }
	LWorldLocatorServer*	GetWorldLocatorServer()	{ return m_pWorldLocatorServer;	}

	LPlayerLoginGameServerProcManager*	GetPlayerLoginGameServerProcManager() { return m_pPlayerLoginGameServerProcManager; }
};
