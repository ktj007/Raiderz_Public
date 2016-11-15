#pragma once

#include "MSingleton.h"
#include "MTime.h"
#include "GNetServer.h"
#include "GserverInfo.h"
#include "GScriptManager.h"
#include "GEnvManager.h"
#include "GServerSystems.h"
#include "SFrameCounter.h"

class GCmdHandler_Comm;
class GCmdHandler_Game;
class GFullMgrAndSys;
class GCommandCenter;
class GWorld;
class GMasterNetClient;
class GMasterServerFacade;
class GAppServerFacade;
class GAsyncTaskManager;
class GServerStatusUpdater;
class GFieldStatusUpdater;
class SCommandProfileController;
class GServableChecker;
class GDBCacheRegularFlush;
class GServerCloser;


/// 실제 서버 메인 클래스
class GServer
{
protected:
	friend class GNetServer;
	friend class GCmdHandler_Comm;

	// 시스템 ------------------------------------
	//
	bool						m_bCreated;						///< 생성되었는지 확인
	GServerInfo					m_ServerInfo;					///< 서버 정보
	MRegulator*					m_pServerInfoViewRglt;
	GServerStatusUpdater*		m_pServerStatusUpdater;
	GFieldStatusUpdater*		m_pFieldStatusUpdater;
	SFrameCounter*				m_pFrameCounter;
	GServableChecker*			m_pServableChecker;
	GServerCloser*				m_pServerCloser;


	// 네트워크 ----------------------------------
	//
	MNetServer*					m_pNetServer;					///< 서버
	GCommandCenter*				m_pCommandCenter;
	SCommandProfileController*	m_pCommandProfileController;
	SCommandProfileController*	m_pCommandProfileController_ForMasterServer;
	SCommandProfileController*	m_pCommandProfileController_ForAppServer;
	
	GMasterServerFacade*		m_pMasterServerFacade;
	GAppServerFacade*			m_pAppServerFacade;
	

	// 로직 --------------------------------------
	//
	GWorld*						m_pWorld;
	GAsyncTaskManager*			m_pAsyncTaskManager;
	GDBCacheRegularFlush*		m_pDBCacheRegularFlush;

private:
	void			WriteServerInfoLog();
	void			UpdateServerInfoView();

	void			DeInitInfo();
	virtual bool	LoadInfoFiles();
protected:
	bool			InitRequisites(bool bConnectDB=true);
	void			Init_UpdateDB_Infos();

	virtual bool	CreateNetwork();
	virtual void	DestroyNetwork();
	virtual void	SetDsnFactory();
	virtual bool	InitDB();
	virtual void	ReleaseDB();

	virtual bool	InitDependencyInfo();
	virtual bool	InitInfo();

	virtual void	UpdateForDebug( float fDelta );

public:
	GServer();
	virtual ~GServer();

	virtual bool		Create(const std::wstring& strServerVersion, bool bConnectDB=true);

	virtual void		Destroy();
	virtual bool		Update(float fDelta);
	
	void				CloseServer(void);		// 서버 종료

	bool				Reload();
	void				Disconnect(const MUID& uidPlayer);
	void				HardDisconnect(const MUID& uidPlayer);
	void				StartCommandProfile();
	void				DumpCommandProfile();
	void				FinishCommandProfile();
	void				CreateCommandProfileForAppServer(MCommandCommunicator* pCC);
	void				Dump(wstring strID);

	MNetServer*			GetNetServer()			{ return m_pNetServer; }
	virtual MUID		NewUID()				{ return m_pNetServer->NewUID(); }
			MUID		GetUID()				{ return m_pNetServer->GetUID(); }
	bool				ReAllocLinkUID(MUID uidOrigin, MUID uidReAlloc);

	bool				IsCreated()				{ return m_bCreated; }	
	GServerInfo&		GetServerInfo()			{ return m_ServerInfo; }
	GServableChecker&	GetServableChecker()	{ return *m_pServableChecker; }
	void				FrameCounterClear();

	GAsyncTaskManager*	GetAsyncTaskManager()	{ return m_pAsyncTaskManager; }

	bool				IsServerClosing();
};

