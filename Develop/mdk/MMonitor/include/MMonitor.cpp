#include "stdafx.h"
#include "MMonitor.h"
#include "MMonitorCommunicator.h"
#include "MTask.h"
#include "MTaskManager.h"
#include "MTaskManagerGroup.h"
#include "MMonitorProtocolBuilder.h"
#include "MMonitorUDPXml.h"
#include "MMonitorCommand.h"
#include "MMonitorMemPool.h"

#include <crtdbg.h>


namespace mmonitor {

MMonitor::MMonitor() 
{
	m_pCommunicator				= NULL;
	m_pTaskManagerGroup			= NULL;
	m_pProtocolBuilder			= NULL;
	m_pRecvUDPXmlManager		= NULL;
	m_pOnCommandUDPXmlManager	= NULL;

	m_dwTotalSendCount			= 0;
	m_dwTotalRecvCount			= 0;
	m_dwTotalOnCmdCount			= 0;
}


MMonitor::~MMonitor()
{
	Release();
}


bool MMonitor::Run( const DWORD dwCurTime )
{
 	if( !OnPreRun(dwCurTime) )
		return false;

	if( !Run_MonitorCommand() )
		return false;

	if( !Run_Task(dwCurTime) )
		return false;

	Run_FlushSendUDPCommand();

	return OnRun( dwCurTime );
}

}