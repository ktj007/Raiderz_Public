#ifndef _MMONITORMONITOR
#define _MMONITORMONITOR


#include <crtdbg.h>
#include <mmsystem.h>

#include <vector>
#include <string>


using std::vector;
using std::string;

namespace mmonitor {

class MMonitorRecvXml;
class MMonitorCommunicator;
class MTaskManager;
class MTaskManagerGroup;
class MMonitorProtocolBuilder;
class MMonitorUDPXml;
class MMonitorUDPXmlManager;
class MMonitorCommand;
class MTask;
class MMonitorCommandElement;


class MMonitor
{
private :
	MMonitorCommunicator*			m_pCommunicator;
	MMonitorProtocolBuilder*		m_pProtocolBuilder;
	MMonitorUDPXmlManager*			m_pRecvUDPXmlManager;
	MMonitorUDPXmlManager*			m_pOnCommandUDPXmlManager;
	vector< MMonitorCommand* >		m_SendCommandList;

	DWORD							m_dwTotalSendCount;
	DWORD							m_dwTotalRecvCount;
	DWORD							m_dwTotalOnCmdCount;

protected :
	MTaskManagerGroup*				m_pTaskManagerGroup;

private :
	bool							Run_MonitorCommand();
	bool							Run_Task( const DWORD dwCurTime );
	void							Run_FlushSendUDPCommand();

	void							MoveRecvUDPXmlListToOnCommandUDPXmlList();

	bool							LocalTask( const DWORD dwCurTime, MTask* pLocalTask );
	MMonitorCommandElement*			NetworkTask( const DWORD dwCurTime, MTask* pNetworkTask );
	bool							LocalTaskManager( const DWORD dwCurTime, MTaskManager* pLocalTaskManager );
	bool							NetworkTaskManager( const DWORD dwCurTime, MTaskManager* pNetworkTaskManager );
	
	MMonitorCommandElement*			MonitorTask( const DWORD dwCurTime, MTask* pNetworkTask );

	bool							RequestMonitorCommand( const MMonitorCommand* pRequestCmd );
	bool							ResponseMonitorCommand( const MMonitorCommand* pResponseCmd );
	MMonitorCommandElement*			RequestMonitorCommandElement( const MMonitorCommandElement* pCmdElement );
	MMonitorCommandElement*			ResponseMonitorCommandElement( const MMonitorCommandElement* pCmdElement );

	MMonitorCommandElement*			RequestMonitorPingCommand( const MMonitorCommandElement* pCmdElement );
	MMonitorCommandElement*			ResponseMonitorPingCommand( const MMonitorCommandElement* pCmdElement );

private :
	////////////////////////////////////////// start Resource /////////////////////////////////////////////////////////////////

	bool							InitTaskManagerGroup();
	bool							InitUDPXmlManager();
	void							InitMonitorObjectMemPool();
	
	void							ReleaseMCommunicator();
	void							ReleaseTaskManagerGroup();
	void							ReleaseProtocolBuilder();
	void							ReleaseUDPXmlManager();
	void							ReleaseSendCommandList();
	void							ReleaseMonitorObjectMemPool();

	////////////////////////////////////////// end Resource /////////////////////////////////////////////////////////////////

private :
	/////////////////////////////////////////// start virtual function //////////////////////////////////////////////////////
	
	virtual bool					OnLocalTask( const DWORD /*dwCurTime*/, MTask* /*pLocalTask*/ )							{ return true; }
	virtual MMonitorCommandElement*	OnNetworkTask( const DWORD /*dwCurTime*/, MTask* /*pNetworkTask*/ )						{ return NULL; }

	// 네트웍으로부터 받은 커맨드를 처리하는 함수.
	//  처리된 작업에대한 응답이 필요 없으면 NULL을 반환하면 된다.
	//  만약 처리된 작업에 응답이 필요하면 MMonitorCommandElement를 생성해서 반환해 주면,
	//  커맨드를 보낸 클라이언트로 보낸다.
	virtual	MMonitorCommandElement* OnRequestMonitorCommandElement( const MMonitorCommandElement* /*pCmdElement*/ )		{ return NULL; }
	virtual	MMonitorCommandElement* OnResponseMonitorCommandElement( const MMonitorCommandElement* /*pCmdElement*/ )	{ return NULL; }


	// 프로토콜빌더를 초기화하는 클래스는 InitProtocolBuilder()함수를 아래와 같이 만들어 줘야 한다.
	// 1. m_pProtocolBuilder에 new로 메모리를 할당한다.
	// 2. m_pProtocolBuilder에서 XML을 파싱사용할때 사용할 MMonitorXmlDocument클래스를 제정의한 클래스의 포틴터를 넘겨준다.
	//  2-1. new로 MMonitorXmlDocument객체나 파생된 객체의 메모리를 할당한다.
	//  2-2 .생성된 객체의 포인터는 m_pProtocolBuilder->Init(...)함수의 파라미터로 넘겨진다. 삭제는 내부에서 관리를 해준다.
	virtual bool					InitProtocolBuilder()															= 0;
	virtual bool					OnInit()																		{ return true; }

	virtual void					OnPreRelease()																	{}
	virtual void					OnRelease()																		{}

	virtual bool					OnPreRun( const DWORD /*dwCurTime*/ )												{ return true; }
	virtual bool					OnRun( const DWORD /*dwCurTime*/ )													{ return true; }

	virtual void					OnPreFlushSendUDPCommand( const MMonitorCommand* /*pSendMonitorCommand*/ )			{}
	virtual void					OnFlushSendUDPCommand( const MMonitorCommand* /*pSendMonitorCommand*/ )				{}

	virtual MMonitorCommandElement* OnResponseMonitorPingCommand( const MMonitorCommandElement* /*pRecvCmdElement*/
																  , MTaskManager* /*pPingTaskManager*/ )				{ return NULL; }

	/////////////////////////////////////////// end virtual function //////////////////////////////////////////////////////

protected :
	void							PostMonitorCommand( MMonitorCommand* /*pMonitorCommand*/ );

	const DWORD						GetSendXmlListSize() { return static_cast<DWORD>(m_SendCommandList.size()); }
	const DWORD						GetRecvXmlListSize();
	const DWORD						GetOnCommandXmlListSize();

	const DWORD						GetTotalSendCount()		{ return m_dwTotalSendCount; }
	const DWORD						GetTotalRecvCount()		{ return m_dwTotalRecvCount; }
	const DWORD						GetTotalOnCmdCount()	{ return m_dwTotalOnCmdCount; }

#define MONITOR_MAX_DEBUG_PROCESS_COUNT (1000000000) // 10억.
	void							IncreaseSendCount()		{ if( MONITOR_MAX_DEBUG_PROCESS_COUNT < ++m_dwTotalSendCount ) m_dwTotalSendCount = 0; }
	void							IncreaseRecvCount()		{ if( MONITOR_MAX_DEBUG_PROCESS_COUNT < ++m_dwTotalRecvCount ) m_dwTotalSendCount = 0; }
	void							IncreaseOnCmdCount()	{ if( MONITOR_MAX_DEBUG_PROCESS_COUNT < ++m_dwTotalOnCmdCount ) m_dwTotalOnCmdCount = 0; }

public :
	MMonitor();
	virtual ~MMonitor();

	MMonitorCommunicator*			GetCommunicator();
	DWORD							GetTaskManagerCount();
	
	bool							Run( const DWORD dwCurTime = timeGetTime() );

	void							SetCommunicator( MMonitorCommunicator* pCommunicator );
	void							SetMonitorProtocolBuilder( MMonitorProtocolBuilder* pProtocolBuilder );
	void							SafePushRecvXml( MMonitorUDPXml* pRecvXml );

	bool							Init();
	void							Release();
};

}

#endif