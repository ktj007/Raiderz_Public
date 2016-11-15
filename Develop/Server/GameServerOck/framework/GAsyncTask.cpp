#include "stdafx.h"
#include "GAsyncTask.h"

GAsyncTask::GAsyncTask( MUID uid, uint32 nNowTime ) : m_UID(uid), m_bDone(false), m_nStartTime(nNowTime)
{

}

bool GAsyncTask::CheckTimeOut( uint32 nNowTime )
{
	if (GetTimeOut() == 0) return false;
	if (nNowTime - m_nStartTime < GetTimeOut()) return false;

	return true;
}

void GAsyncTask::Update()
{
	if (IsDone()) return;

	OnUpdate();
}

//////////////////////////////////////////////////////////////////////////

GAsyncTaskManager::GAsyncTaskManager()
{

}
GAsyncTaskManager::~GAsyncTaskManager()
{
	Clear();
}


void GAsyncTaskManager::Add( GAsyncTask* pAsyncTask )
{
	m_TaskMap.insert(GAsyncTaskMap::value_type(pAsyncTask->GetUID(), pAsyncTask));
}

void GAsyncTaskManager::Delete( MUID uidTask )
{
	GAsyncTaskMap::iterator itor = m_TaskMap.find(uidTask);
	if (itor != m_TaskMap.end())
	{
		GAsyncTask* pTask = (*itor).second;
		pTask->Drop();
		m_TaskMap.erase(itor);
	}
}

GAsyncTask* GAsyncTaskManager::FindTask( MUID uidTask )
{
	GAsyncTaskMap::iterator itor = m_TaskMap.find(uidTask);
	if (itor != m_TaskMap.end())
	{
		return (*itor).second;
	}
	return NULL;
}

void GAsyncTaskManager::Clear()
{
	for (GAsyncTaskMap::iterator itor = m_TaskMap.begin(); itor != m_TaskMap.end(); ++itor)
	{
		GAsyncTask* pTask = (*itor).second;
		pTask->Drop();
	}
	m_TaskMap.clear();
}


void GAsyncTaskManager::Update(uint32 nNowTime)
{
	for (GAsyncTaskMap::iterator itor = m_TaskMap.begin(); itor != m_TaskMap.end(); )
	{
		GAsyncTask* pTask = (*itor).second;
		pTask->Update();
		
		if (pTask->IsDone() || pTask->CheckTimeOut(nNowTime))
		{
			pTask->Drop();
			itor = m_TaskMap.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

MUID GAsyncTaskManager::NewUID()
{
	return m_UIDGenerator.Generate();
}

void GAsyncTaskManager::OnCommand( minet::MCommand* pCommand )
{
	for (GAsyncTaskMap::iterator itor = m_TaskMap.begin(); itor != m_TaskMap.end(); ++itor)
	{
		GAsyncTask* pTask = (*itor).second;
		pTask->OnCommand(pCommand);
	}
}

