#include "stdafx.h"

#include <winsock2.h>
#include <windows.h>

#include "MTask.h"
#include "MTaskManager.h"
#include "MTaskManagerGroup.h"

#include <crtdbg.h>
#include <utility>

using std::pair;


namespace mmonitor {

MTaskManagerGroup::MTaskManagerGroup()
{
	m_nIndexCache = 0;
	m_IsUsableNameIndex = true;
}


MTaskManagerGroup::~MTaskManagerGroup()
{
	Release();
}


MTaskManager* MTaskManagerGroup::GetTaskManager( const DWORD dwIndex )
{
	if( dwIndex < m_TaskManagerList.size() )
		return m_TaskManagerList[ dwIndex ];

	return NULL;
}


MTaskManager* MTaskManagerGroup::GetTaskManager( const string& strTaskManagerName )
{
	if( 0 == strTaskManagerName.length() )
		return NULL;

	for( DWORD i = 0; i < GetTaskManagerCount(); ++i )
	{
		if( strTaskManagerName == m_TaskManagerList[i]->GetName() )
			return m_TaskManagerList[ i ];
	}

	return NULL;
}


MTaskManager* MTaskManagerGroup::GetTaskManager( const string& strType, const string& strIP, const unsigned short nPort )
{
	MTaskManager* pTaskManager;

	for( DWORD i = 0; i < GetTaskManagerCount(); ++i )
	{
		pTaskManager = m_TaskManagerList[ i ];

		if( string(TASK_NETWORK_TYPE) == strType )
		{
			if( (nPort == pTaskManager->GetPort()) &&
				(strType == pTaskManager->GetTaskType()) &&
				(strIP == pTaskManager->GetIPStr()) )
			{
				return m_TaskManagerList[ i ];
			}
		}
		else
		{
			if( strType == pTaskManager->GetTaskType() )
				return m_TaskManagerList[ i ];
		}
	}

	return NULL;
}


bool MTaskManagerGroup::AddTask( const string& strType, const string& strIP, const unsigned short nPort, MTask* pTask )
{
	// Task를 등록한다.
	// 만약 적당한 TaskManager가 없다면 새로 생성을 한다.
	// Task는 TaskManager에 등록되기 전에 북마크를 해둬야 한다.
	MTaskManager* pTaskManager = GetTaskManager( strType, strIP, nPort );
	if( NULL == pTaskManager )
	{
		pTaskManager = new MTaskManager( strType );
		if( 0 == pTaskManager )
			return false;

		pTaskManager->SetIPStr( strIP );
		pTaskManager->SetPort( nPort );

		if( string(TASK_TYPE_NETWORK) == strType )
		{
			if( !pTaskManager->MakePingTask(MakeIndex()) )
			{
				DeleteMonitorTaskManager( pTaskManager );
				return false;
			}
		}

		m_TaskManagerList.push_back( pTaskManager );
	}

	// 인텍스 생성.
	pTask->SetIndex( MakeIndex() );

	// Task북마크.
	if( !AddTaskToMap(pTask) )
		return false;

	// 북마크가 완료된 Task는 TaskManager에 등록된다.
	if( !pTaskManager->AddTask(pTask) )
	{
		DeleteTaskFromMap( pTask );
		return false;
	}

	return true;
}


bool MTaskManagerGroup::AddTaskManager( MTaskManager* pTaskManager )
{
	if( NULL != GetTaskManager(pTaskManager->GetTaskType()
		, pTaskManager->GetIPStr()
		, pTaskManager->GetPort()) )
	{
		return false;
	}

	if( NULL == pTaskManager->GetTask(MONITOR_PING_TASK_NAME) )
	{
		if( !pTaskManager->MakePingTask(MakeIndex()) )
		{
			_ASSERT( 0 );
			return false;
		}
	}

	MTask* pTask;
	const DWORD dwTaskCount = pTaskManager->GetTaskCount();
	for( DWORD i = 0; i < dwTaskCount; ++i )
	{
		pTask = pTaskManager->GetTask( i );
		if( 0 == pTask )
			return false;

		pTask->SetIndex( MakeIndex() );

		// 모든 Task를 북마크 한다.
		if( !AddTaskToMap(pTask) )
			return false;
	}

	// 북마크가 완료되면 TaskManager를 등록한다.
	m_TaskManagerList.push_back( pTaskManager );

	return true;
}


bool MTaskManagerGroup::AddTaskManager( const string& strType, const string& strIP, const unsigned short nPort )
{
	// 비어있는 TaskManager를 생성.

	if( NULL != GetTaskManager(strType, strIP, nPort) )
		return false;

	MTaskManager* pTaskManager = new MTaskManager( strType );
	if( 0 == pTaskManager )
		return false;

	pTaskManager->SetIPStr( strIP );
	pTaskManager->SetPort( nPort );

	if( !pTaskManager->MakePingTask(MakeIndex()) )
	{
		DeleteMonitorTaskManager( pTaskManager );
		return false;
	}

	m_TaskManagerList.push_back( pTaskManager );

	return true;
}


void MTaskManagerGroup::Release()
{
	MTaskManager* pTaskManager;
	for( DWORD i = 0; i < GetTaskManagerCount(); ++i )
	{
		pTaskManager = GetTaskManager( i );
		if( 0 == pTaskManager )
		{
			_ASSERT( 0 );
			continue;
		}

		DeleteMonitorTaskManager( pTaskManager );
	}

	m_TaskManagerList.clear();
	m_TaskMap.clear();
	m_TaskIndexMap.clear();
}


bool MTaskManagerGroup::AddTaskToMap( MTask* pTask )
{
	// Task를 북마크 한다.

	if( 0 == pTask )
		return false;

	// 같은 이름의 Task가 있는지 검사.
	if( m_IsUsableNameIndex && (NULL == GetTask(pTask->GetName())) )
	{
		m_TaskMap.insert( pair<string, MTask*>(pTask->GetName(), pTask) );
	}
	else
	{
		// 만약 같은 이름이 있으면, 이름으로 인텍싱한건 사용할 수 없게 한다.
		if( m_IsUsableNameIndex )
		{
			_ASSERT( 0 && "duplicated task name." );
			m_TaskMap.clear();
		}

		m_IsUsableNameIndex = false;
	}
	
	// id는 항상 고유하므로 그냥 추가를 한다.
	m_TaskIndexMap.insert( pair<DWORD, MTask*>(pTask->GetIndex(), pTask) );

	return true;
}


MTask* MTaskManagerGroup::GetTask( const string& strTaskName )
{
	if( !m_IsUsableNameIndex )
		return NULL;

	map< string, MTask* >::iterator itFind = m_TaskMap.find( strTaskName );
	if( m_TaskMap.end() == itFind )
		return NULL;

	return itFind->second;
}


DWORD MTaskManagerGroup::MakeIndex()
{
	if( MMONITOR_TASK_MAX_INDEX < (++m_nIndexCache) )
		m_nIndexCache = 1;

	return m_nIndexCache;
}


void MTaskManagerGroup::DeleteTaskFromMap( MTask* pTask )
{
	map< string, MTask* >::iterator itFind = m_TaskMap.find( pTask->GetName() );
	if( m_TaskMap.end() != itFind )
		m_TaskMap.erase( itFind );

	map< DWORD, MTask* >::iterator itFindIndex = m_TaskIndexMap.find( pTask->GetIndex() );
	if( m_TaskIndexMap.end() != itFindIndex )
		m_TaskIndexMap.erase( itFindIndex );
}


MTask* MTaskManagerGroup::GetTask( const DWORD dwIndex )
{
	map< DWORD, MTask* >::iterator itFind = m_TaskIndexMap.find( dwIndex );
	if( m_TaskIndexMap.end() == itFind )
		return NULL;

	return itFind->second;	
}

}