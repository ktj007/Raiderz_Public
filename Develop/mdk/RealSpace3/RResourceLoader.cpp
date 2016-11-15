#include "StdAfx.h"
#include "RResourceLoader.h"
#include "RResourceID.h"
#include "MTime.h"

namespace rs3 {

RResourceLoader::RResourceLoader(RResource* pResource, bool bBackground ) : m_pResource(pResource), m_bBackgroundCreate(bBackground)
{
}

const char* RResourceLoader::GetName()
{
	if (m_pResource)
	{
		m_strWorkUnitName = "RResourceLoader, " + m_pResource->GetName();
		return m_strWorkUnitName.c_str();
	}
	else
	{
		return "RResourceLoader";
	}
}

int RResourceLoader::CalcPriority()
{
	if( m_pResource->m_loadingControllers.empty() )
		return m_pResource->m_nLoadingPriority;

	RResource::LOADINGCONTROLLERLIST::iterator itr = m_pResource->m_loadingControllers.begin();
	int nPriority = (*itr)->CalcPriority();
	++itr;

	for( ; itr!= m_pResource->m_loadingControllers.end(); ++itr)
	{
		nPriority = min ( nPriority, (*itr)->CalcPriority() );
	}

	return nPriority;
}

void RResourceLoader::OnBackgroundWork()
{

	if(m_pResource->GetState() == RRS_FILLED ||
		m_pResource->GetState() == RRS_LOADED) return;

	if ( m_pResource->GetState() != RRS_EMPTY )
	{
		m_pResource->m_state = RRS_WRONG;

		return;
	}

	_ASSERT( m_pResource->GetState() == RRS_EMPTY );
	bool bRet = doFill(m_pResource);

	if( !bRet )
	{
		m_pResource->m_state = RRS_WRONG;
		return;
	}

	m_pResource->m_state = RRS_FILLED;
}

void RResourceLoader::OnUpdateCompleteWork()
{
	if ( m_pResource->GetState() != RRS_FILLED && 
		m_pResource->GetState() != RRS_LOADED)	// 로딩 실패
	{
		FinishWork();
		m_pResource->SendNotifyAll();
		return;
	}

	if(m_pResource->GetState() == RRS_FILLED)
	{
		RResource::LOAD_RESULT lr = doLoad(m_pResource);

		if(lr == RResource::WRONG)
		{
			FinishWork();
			m_pResource->m_state = RRS_WRONG;
		}else
		if(lr == RResource::WAITING)
		{
			// filled 인 상태가 유지되면 계속 대기
			if (m_pBackgroundWorker)
				m_pBackgroundWorker->AddToReadyQueue(this);
		}else
		{
			_ASSERT( lr == RResource::LOADED );
			m_pResource->m_state = RRS_LOADED;
		}
	}

	if(m_pResource->GetState() == RRS_LOADED)	// 로딩 완료
	{
		if(!m_pResource->SendNotify1())
		{
			FinishWork();
		}else
		{
			if (m_pBackgroundWorker)
				m_pBackgroundWorker->AddToReadyQueue(this);
		}
	}
}

bool RResourceLoader::Load(bool bBackground)
{
	INIT_TCHECK("/check_resource_loadtime");

	if( m_pResource->IsLoadingCompleted() )	// 이미 로딩됨. 로딩할 필요 없음
	{
		BEGIN_TCHECK;

		m_pResource->SendNotifyAll();

		END_TCHECK("RResourceLoader::Load, SendNotifyAll / " + m_pResource->GetName());

		return true;
	}

	RBackgroundWorker& bg_worker = REngine::GetBackgroundWorker();

	// 백그라운드 로딩요청
	if(bBackground && bg_worker.IsInited())
	{
		// 해당 리소스가 이미 백그라운드 로딩중인경우
		if( m_pResource->IsBackgroundCreated() )
		{
			return true;
		}

		_ASSERT( m_pResource->m_pLoader == NULL );

		m_pResource->m_pLoader = new RResourceLoader(m_pResource,true);

		bg_worker.AddToReadyQueue(m_pResource->m_pLoader);
		bg_worker.FlushReadyQueueAndRun();

		return true;
	}else
	// foreground 로딩 요청인경우
	{
		BEGIN_TCHECK;

		// 해당 리소스가 이미 백그라운드 로딩중인경우
		if( m_pResource->IsBackgroundCreated() )
		{
			// 백그라운드 로딩 할 리소스가 빌드를 미루고 있다면 다시 포그라운드 요청이 들어와선 안된다.
			_ASSERT( !m_pResource->IsBuildLater() );

			// 이미 로딩중이면 완료될때 까지 기다린다
			while( !m_pResource->IsLoadingCompleted() && bg_worker.GetRemainedWorkableTicketSize() != 0) 
			{
				//Sleep(10);
				m_pResource->SetLoadingPriority(RBGWK_HIGHEST - RBGWK_HIGH_OFFSET);
				m_pResource->m_pLoader->SetPriority(RBGWK_HIGHEST - RBGWK_HIGH_OFFSET);
				bg_worker.ReOrderRunningQueue();
				bg_worker.Update();
			};

			// 로딩이 끝났으므로 알림
			m_pResource->SendNotifyAll();

		}else
		{
			// 로딩중이 아니면 로딩
			_ASSERT(m_pResource->GetState() == RRS_EMPTY );
			WorkInTheForeground();
		}

		if(m_pResource->GetState() != RRS_LOADED)
			return false;

		END_TCHECK("RResourceLoader, Foreground / " + m_pResource->GetName());
	}

	// 후에 buildall 과 통합
	return true;
}

RResource::LOAD_RESULT RResourceLoader::doLoad( RResource* pResource )
{
	const bool bLog = REngine::GetConfig().m_bLogResource;

	MTimeChecker tc;
	if (bLog)
	{
		tc.Start();
	}

	RResource::LOAD_RESULT ret = m_pResource->Load();

	return ret;
}


//extern set<string>		g_setResources;

bool RResourceLoader::doFill( RResource* pResource )
{
	const bool bLog = REngine::GetConfig().m_bLogResource;

	MTimeChecker tc;
	if (bLog)
	{
		tc.Start();
	}

	bool bRet = pResource->Fill();

	//string str = pResource->GetDebugName();
	//if (str != "")
	//{
	//	if (g_setResources.find(str) != g_setResources.end())
	//	{
	//		const char* szResID = RRESOURCETYPE::GetResoruceIDString(pResource->GetResID());

	//		mlog("두번 로딩: %s %s\n", szResID, str.c_str());
	//	}
	//}

	return bRet;
}


}