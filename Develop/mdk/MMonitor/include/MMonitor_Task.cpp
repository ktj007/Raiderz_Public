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
#include "MMonitorCommandDefine.h"

#include <crtdbg.h>

namespace mmonitor {

bool MMonitor::Run_Task( const DWORD dwCurTime )
{
	MTaskManager* pTaskManager;
	for( DWORD i = 0; i < GetTaskManagerCount(); ++i )
	{
		pTaskManager = m_pTaskManagerGroup->GetTaskManager( i );
		if( 0 == pTaskManager )
		{
			_ASSERT( 0 );
			continue;
		}

		if( string(TASK_NETWORK_TYPE) == pTaskManager->GetTaskType() )
		{
			if( !NetworkTaskManager(dwCurTime, pTaskManager) )
			{
				_ASSERT( 0 );
				return false;
			}
		}
		else if( string(TASK_LOCAL_TYPE) == pTaskManager->GetTaskType() )
		{
			if( !LocalTaskManager(dwCurTime, pTaskManager) )
			{
				_ASSERT( 0 );
				return false;
			}
		}
#ifdef _DEBUG
		else
		{
			_ASSERT( 0 );
		}
#endif
	}
		
	return true;
}


bool MMonitor::LocalTask( const DWORD dwCurTime, MTask* pLocalTask )
{
	return OnLocalTask( dwCurTime, pLocalTask );
}


MMonitorCommandElement* MMonitor::NetworkTask( const DWORD dwCurTime, MTask* pNetworkTask )
{
	if( string(MONITOR_PING_TASK_NAME) == pNetworkTask->GetName() )
	{
		return MonitorTask( dwCurTime, pNetworkTask );
	}
	
	return OnNetworkTask( dwCurTime, pNetworkTask );
}


bool MMonitor::NetworkTaskManager( const DWORD dwCurTime, MTaskManager* pNetworkTaskManager )
{
	if( NULL == pNetworkTaskManager )
		return false;

	MMonitorCommand* pCmd = NULL;

	const DWORD dwTaskCount = pNetworkTaskManager->GetTaskCount();
	for( DWORD i = 0; i < dwTaskCount; ++i )
	{
		MTask* pTask = pNetworkTaskManager->GetTask( i );
		if( NULL == pTask )
			continue;

		// 이 부분은 상속 받는 쪽에서 처리르 하도록 한다.
		//if( !pTask->IsUse() )
		//	continue;

		if( pTask->GetInterval() < (dwCurTime - pTask->GetLastUpdateTime()) )
		{
			MMonitorCommandElement* pCmdElement = NetworkTask( dwCurTime, pTask );
			if( NULL == pCmdElement )
				continue;

			if( NULL == pCmd )
			{
				pCmd = new MMonitorCommand;
				if( NULL == pCmd )
					return false;

				pCmd->SetIP( pNetworkTaskManager->GetIP() );
				pCmd->SetPort( pNetworkTaskManager->GetPort() );
			}

			pCmd->AddCommandElement( pCmdElement );
		}
	}

	if( NULL != pCmd )
        PostMonitorCommand( pCmd );

	return true;
}


bool MMonitor::LocalTaskManager( const DWORD dwCurTime, MTaskManager* pLocalTaskManager )
{
	if( NULL == pLocalTaskManager )
		return false;

	const DWORD dwTaskCount = pLocalTaskManager->GetTaskCount();
	for( DWORD i = 0; i < dwTaskCount; ++i )
	{
		MTask* pTask = pLocalTaskManager->GetTask( i );
		if( NULL == pTask )
		{
			_ASSERT( 0 );
			continue;
		}

		if( pTask->GetInterval() < (dwCurTime - pTask->GetLastUpdateTime()) )
		{
			if( !LocalTask(dwCurTime, pTask) )
			{
				continue;
			}
		}
	}

	return true;
}





DWORD MMonitor::GetTaskManagerCount()
{
	if( NULL == m_pTaskManagerGroup )
		return 0;

	return m_pTaskManagerGroup->GetTaskManagerCount();
}


MMonitorCommandElement* MMonitor::MonitorTask( const DWORD dwCurTime, MTask* pNetworkTask )
{
	MMonitorCommandElement* pCmdElement = new MMonitorCommandElement;
	if( NULL == pCmdElement )
		return NULL;

	pCmdElement->SetProtocolID( MONITOR_PING_PROTOCOLID );
	pCmdElement->SetTaskName( pNetworkTask->GetName() );
	pCmdElement->SetTarget( pNetworkTask->GetTarget() );
	pCmdElement->AddParam( MMCXML_TRANSMIT_IP, pNetworkTask->GetIPStr() );
	pCmdElement->AddParam( MMCXML_TRANSMIT_PORT, pNetworkTask->GetPortStr() );

	pCmdElement->ThisIsRequest();

	pNetworkTask->SetLastUpdatedTime( dwCurTime );

	return pCmdElement;
}

}