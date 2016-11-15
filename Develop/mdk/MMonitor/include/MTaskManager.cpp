#include "stdafx.h"
#include "MTask.h"
#include "MTaskManager.h"
#include "MMonitorCommandDefine.h"

#include <crtdbg.h>

namespace mmonitor {

void DeleteMonitorTaskManager( MTaskManager*& pTaskManager )
{
	if( NULL == pTaskManager )
		return;

	pTaskManager->Release();
	delete pTaskManager;
	pTaskManager = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


MTaskManager::MTaskManager()
{
	m_IsUse					= true;
	m_dwLastPingUpdateTime	= 0;
}


MTaskManager::MTaskManager( const string& strTaskType ) : m_strTaskType( strTaskType )
{
	m_IsUse = true;
}


MTaskManager::~MTaskManager()
{
	Release();
}

bool MTaskManager::AddTask( MTask* pTask )
{
	if( 0 == pTask ) 
		return false;

	if( NULL != GetTask(pTask->GetName()) )
		return false;

	m_TaskList.push_back( pTask );

	return true;
}


MTask* MTaskManager::GetTask( const DWORD dwIndex )
{
	if( GetTaskCount() > dwIndex )
		return m_TaskList[ dwIndex ];

	return NULL;
}


MTask* MTaskManager::GetTask( const string& strTaskName )
{
	if( 0 == strTaskName.length() )
		return NULL;

	MTask* pTask;
	const DWORD dwTaskCount = GetTaskCount();
	
	for( DWORD i = 0; i < dwTaskCount; ++i )
	{
		pTask = m_TaskList[ i ];
		if( strTaskName == pTask->GetName() )
			return pTask;
	}

    return NULL;
}


void MTaskManager::Release()
{
	OnRelease();

	MTask* pTask;
	for( DWORD i = 0; i < GetTaskCount(); ++i )
	{
		pTask = GetTask( i );
		if( 0 != pTask )
		{
			delete pTask;
			pTask = 0;
		}
	}

	m_TaskList.clear();
}


bool MTaskManager::MakePingTask( const DWORD dwIndex )
{
	_ASSERT( 0 != dwIndex );
	// Monitor ping은 TaskManager당 하나만 가지고 있을 수 있다.
	_ASSERT( NULL == GetTask(MONITOR_PING_TASK_NAME) );
	
	// local task는 핑을 보낼 필요가 없음.
	if( string(TASK_TYPE_NETWORK) != m_strTaskType )
		return true;

	MTask* pPingTask = new MTask;
	if( NULL == pPingTask )
		return false;

	char szPort[ 8 ] = {0,};

	sprintf( szPort, "%u", GetPort() );

	pPingTask->SetIndex( dwIndex );

	pPingTask->AddAttribute( TASK_NAME,		MONITOR_PING_TASK_NAME );
	pPingTask->AddAttribute( TASK_TARGET,	MONITOR_PING_TASK_NAME );
	pPingTask->AddAttribute( TASK_TYPE,		TASK_TYPE_NETWORK );
	pPingTask->AddAttribute( TASK_REQUEST,	MONITOR_PING_PROTOCOLID );
	pPingTask->AddAttribute( TASK_IP, GetIPStr() );
	pPingTask->AddAttribute( TASK_PORT, szPort );

	pPingTask->SetEnvirontment( MONITOR_PING_INTERVAL ); 

	return AddTask( pPingTask );
}

}