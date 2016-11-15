#pragma once

#include "MSingleton.h"
#include "MTime.h"
#include "PNetServer.h"
#include "PServerInfo.h"
#include "PServerSystems.h"
#include "SFrameCounter.h"
#include "PProxyPlayerListener.h"

class PCmdHandler_Master;
class PCommandCenter;
class PServerStatusUpdater;
class SProxyPlayerManager;
class PServableChecker;
class PMasterServerFacade;
class SCommandProfileController;

/// 실제 서버 메인 클래스
class PServer
{
protected:
	friend class PNetServer;

	bool						m_bCreated;
	bool						m_bExiting;						///< 서버가 종료되는 중인지 여부
	PServerInfo					m_ServerInfo;					///< 서버 정보
	minet::MNetServer*			m_pNetServer;					///< 네트워크 관리
	PCommandCenter*				m_pCommandCenter;
	SCommandProfileController*	m_pCommandProfileController;
	MRegulator*					m_pServerInfoViewRglt;
	PServerStatusUpdater*		m_pServerStatusUpdater;
	PServableChecker*			m_pServableChecker;

	PMasterServerFacade*		m_pMasterServerFacade;
	SProxyPlayerManager*		m_pProxyPlayerManager;
	PProxyPlayerListener		m_proxyPlayerListener;


	SFrameCounter				m_FrameCounter;
private:
	void			WriteServerInfoLog();
	void			UpdateServerInfoView();
	void			DeInitInfo();

protected:
	bool			InitRequisites();

	virtual bool 	CreateNetwork();
	virtual void 	DestroyNetwork();
	virtual bool	InitDB();
	virtual bool	InitInfo();
	virtual bool	LoadInfoFiles(void);

	virtual void	UpdateForDebug( float fDelta );
	void			UpdateLog();
public:
	PServer();
	virtual ~PServer();

	virtual bool Create();
	virtual void Destroy();
	virtual bool Update(float fDelta);

	void				Exit(); // 서버 종료 처리

	void					StartCommandProfile();
	void					FinishCommandProfile();

	virtual MUID			NewUID()				{ return m_pNetServer->NewUID(); }
			MUID			GetUID()				{ return m_pNetServer->GetUID(); }
	bool					IsCreated()				{ return m_bCreated; }
	PServerInfo&			GetServerInfo()			{ return m_ServerInfo; }
	PServableChecker&		GetServableChecker()	{ return *m_pServableChecker; }

	SProxyPlayerManager*	GetPlayerManager()		{ return m_pProxyPlayerManager;	}
};
