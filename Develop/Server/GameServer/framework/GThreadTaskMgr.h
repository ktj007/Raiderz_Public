#pragma once

#include "GThreadTask.h"

#pragma push_macro ("new")

#ifdef new 
#undef new
#endif


class GThreadTaskMgr
{
protected:
	class GTBBTask : public tbb::task
	{
	protected:
		GBaseThreadTask*	m_pTask;
	public:
		GTBBTask(GBaseThreadTask* pThreadTask) : m_pTask(pThreadTask)
		{

		}
		tbb::task* execute()
		{
			set_ref_count(0);
			m_pTask->Run();

			delete m_pTask;
			m_pTask = NULL;

			return NULL;
		}
	};

	//list<GBaseThreadTask*>			m_ReadyQueue;
	//typedef list<GBaseThreadTask*>	GThreadTaskQueue;

	void Clear()
	{
		//for (GThreadTaskQueue::iterator itor = m_ReadyQueue.begin(); itor != m_ReadyQueue.end(); ++itor)
		//{
		//	GBaseThreadTask* pTask = (*itor);
		//	delete pTask;
		//}
		//m_ReadyQueue.clear();
	}
public:
	GThreadTaskMgr() 
	{
	}
	virtual ~GThreadTaskMgr() 
	{
		Clear();
	}

	void SpawnTask(GBaseThreadTask* pThreadTask)
	{
		GTBBTask& a = *new(tbb::task::allocate_root()) GTBBTask(pThreadTask);
		a.spawn(a);
	}

	template<typename TResultData>
	GFutureResult<TResultData>* Request(GThreadTask<TResultData>* pTask)
	{
		GFutureResult<TResultData>* pFutureResult = new GFutureResult<TResultData>();
		pTask->SetFutureResult(pFutureResult);

		SpawnTask(pTask);

		//m_ReadyQueue.push_back(pTask);

		return pFutureResult;
	}
};

#pragma pop_macro("new")


