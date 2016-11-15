#pragma once

#include "MSingleton.h"
#include "MTime.h"
#include "ZNetServer.h"
#include "ZServerInfo.h"
#include "ZServerSystems.h"
#include "SFrameCounter.h"

class ZCommandCenter;
class ZServerStatusUpdater;
class ZWorldStatusUpdater;
class ZServableChecker;
class SCommandProfileController;
class ZChannelBeholder;


/// 실제 서버 메인 클래스
class ZServer
{
protected:
	friend class ZNetServer;

	bool						m_bCreated;
	ZServerInfo					m_ServerInfo;					///< 서버 정보
	minet::MNetServer*			m_pNetServer;					///< 네트워크 관리
	ZCommandCenter*				m_pCommandCenter;
	SCommandProfileController*	m_pCommandProfileController;
	MRegulator*					m_pServerInfoViewRglt;
	ZServerStatusUpdater*		m_pServerStatusUpdater;
	ZWorldStatusUpdater*		m_pWorldStatusUpdater;
	MUIDRandomGenerator			m_ReserverKeyGenerator;
	ZServableChecker*			m_pServableChecker;
	ZChannelBeholder*			m_pChannelBeholder;

	SFrameCounter				m_FrameCounter;
private:
	void	WriteServerInfoLog();
	void	UpdateServerInfoView();
	void	DeInitInfo();
	bool	InitDependencyInfo();
	bool	LoadInfoFiles();
	bool	InitFixedParty();

protected:
	bool			InitRequisites();

	virtual bool	CreateNetwork();
	virtual void	DestroyNetwork();
	bool			InitDB();
	virtual void	ReleaseDB();

	virtual bool	InitInfo();

	virtual void	UpdateForDebug( float fDelta );

public:
	ZServer();
	virtual ~ZServer();

	virtual bool Create();
	virtual void Destroy();
	virtual void Update(float fDelta);

	void				StartCommandProfile();
	void				FinishCommandProfile();

	virtual MUID		NewUID()				{ return m_pNetServer->NewUID(); }
			MUID		GetUID()				{ return m_pNetServer->GetUID(); }
	bool				IsCreated()				{ return m_bCreated; }
	ZServerInfo&		GetServerInfo()			{ return m_ServerInfo; }
	ZServableChecker&	GetServableChecker()	{ return *m_pServableChecker; }

	virtual MUID		NewReserverKey(void);
};
