#include "stdafx.h"
#include "XResourceCache.h"
#include "RMeshMgr.h"
#include "REffectInstance.h"
#include "REffectManager.h"
#include "RSceneManager.h"

#include "MProfiler.h"
#include "RTimer.h"
#include "MLocale.h"

class XEffectBGWorkUnit : public RBackgroundWorkUnit
{
public:
	XEffectBGWorkUnit(const std::tstring& _strEffectName) : m_bBGWorkCompleted(false)
	{
		m_strEffectName = _strEffectName;
		m_pEffectSceneNode = NULL;
	}

	virtual ~XEffectBGWorkUnit()
	{
		if (false == m_bBGWorkCompleted)
			InvalidateWork();

		SAFE_DELETE(m_pEffectSceneNode);
		REngine::GetSceneManager().GetEffectManager()->RemoveRefEffectSource(MLocale::ConvTCHARToAnsi(m_strEffectName.c_str()));
	}

	virtual void OnBackgroundWork()
	{
		m_pEffectSceneNode = REngine::GetSceneManager().GetEffectManager()->CreateEffectSource(MLocale::ConvTCHARToAnsi(m_strEffectName.c_str()).c_str(), true, false);
	}

	bool CheckDone()
	{
		if (NULL == m_pEffectSceneNode)
			return true;

		if (m_pEffectSceneNode->IsLoadingCompleted())
		{
			if (1 != REngine::GetSceneManager().GetEffectManager()->AddRefEffectSource(MLocale::ConvTCHARToAnsi(m_strEffectName.c_str()), m_pEffectSceneNode))
			{
				delete m_pEffectSceneNode;
			}
			m_pEffectSceneNode = NULL;
			return true;
		}

		return false;
	}

	bool IsBGWorkCompleted(){ return m_bBGWorkCompleted; }

protected:
	virtual void OnUpdateCompleteWork()
	{
		m_bBGWorkCompleted = true;
		FinishWork();
	}
	virtual int			CalcPriority(){ return RBGWK_HIGHEST; }
	bool				m_bBGWorkCompleted;
	std::tstring		m_strEffectName;
	REffectSceneNode*	m_pEffectSceneNode;
};

void XResourceCache::Init(rs3::RSceneManager* pSceneManager, const TCHAR* szPreLoad)
{
	// Cache
	_InitMeshCache(pSceneManager);
	_InitEffectCache();

	// PreLoading
	_InitResourcePreLoad(szPreLoad);
}

void XResourceCache::_InitMeshCache(rs3::RSceneManager* pSceneManager)
{
	RMeshMgr* pMeshMgr = pSceneManager->GetResourceMgr< RMeshMgr >();
	pMeshMgr->SetCacheEnable( true );

	// cache 정책 설정
	// 리소스는 pResource->SetCachePriority()를 통해 우선순위를 가집니다.
	// 그리고 캐쉬될때 이 우선순위를 바탕으로 각각의 캐쉬큐로 들어갑니다.
	// 다음처럼 캐쉬큐의 Sweep을 하는시간 - 캐시 지속시간 - 을 설정합니다.
	pMeshMgr->SetCacheDuration(RResource::RRCP_HIGH, 600.f);	//high priority cache queue   - 300초(5분) 마다 sweep
	pMeshMgr->SetCacheDuration(RResource::RRCP_MIDDLE, 180.f);	//middle priority cache queue - 180초(3분) 마다 sweep
	pMeshMgr->SetCacheDuration(RResource::RRCP_LOW, 10.f);		//low priority cache queue    - 10초마다 sweep
	m_arrayCachableResMgr[CACHE_MESH] = pMeshMgr;
}

void XResourceCache::_InitEffectCache()
{
	// 	REffectResourceMgr* pEffectResMgr = global.smgr->GetResourceMgr<REffectResourceMgr>();
	// 	pEffectResMgr->SetCacheEnable(true);
	// 
	// 	m_arrayCachableResMgr[CACHE_EFFECT] = pEffectResMgr;
}

void XResourceCache::_InitResourcePreLoad(const TCHAR* szPreLoad)
{
	MXml xml;
	if (xml.LoadFile(MLocale::ConvTCHARToAnsi(szPreLoad).c_str()) == false) 
		return;

	// Root Element Handle
	MXmlHandle docHandle = xml.DocHandle().FirstChild("maiet");

	// config element
	MXmlElement* pConfigElement = docHandle.FirstChild("CONFIG").Element();
	if (pConfigElement)
	{
		MXmlElement* pEffectConfigElement = pConfigElement->FirstChildElement("EFFECT");
		if (pEffectConfigElement)
		{
			_Attribute(m_bUsingEffectCache, pEffectConfigElement, "using");
			_Attribute(m_bUsingEffectLog, pEffectConfigElement, "log");
		}
	}

	RTimeChecker tc;
	if (m_bUsingEffectLog)
	{
		tc.Start();
		mlog("Effect PreLoad Elapsed checking ...\n");
	}

	// Effect
	if (m_bUsingEffectCache)
	{
		std::tstring strResourceName;
		bool bLazy = false;
		MXmlElement* pChildElement = docHandle.FirstChild("EFFECT_LIST").FirstChildElement().Element();
		for( pChildElement; pChildElement != NULL; pChildElement=pChildElement->NextSiblingElement() )
		{
			_Attribute(strResourceName, pChildElement, "name" );
			_Attribute(bLazy, pChildElement, "lazy", false);

			if(m_mapPreLoadEffects.find(strResourceName) == m_mapPreLoadEffects.end())
			{
				if (bLazy)
				{
					m_setRequestedBGEffect.insert(strResourceName);
				}
				else
				{
					REffectInstanceSceneNode* pNewInstance = new REffectInstanceSceneNode;
					pNewInstance->Create( MLocale::ConvTCHARToAnsi(strResourceName.c_str()).c_str() );
					m_mapPreLoadEffects.insert(std::map<std::tstring,REffectInstanceSceneNode*>::value_type(strResourceName, pNewInstance));
				}
			}
		}

		pChildElement = docHandle.FirstChild("EFFECT_NPC_LIST").FirstChildElement().Element();

		char szNpcId[1024] = "";
		std::vector<std::tstring> vecEffects;
		for( pChildElement; pChildElement != NULL; pChildElement=pChildElement->NextSiblingElement() )
		{
			vecEffects.clear();
			_Attribute(szNpcId, pChildElement, "id" );

			MXmlElement* pEffectElement = pChildElement->FirstChildElement("EFFECT");
			for (pEffectElement; pEffectElement != NULL; pEffectElement = pEffectElement->NextSiblingElement())
			{
				_Attribute(strResourceName, pEffectElement, "name");
				vecEffects.push_back(strResourceName);

				_Attribute(bLazy, pEffectElement, "lazy", false);
				if (bLazy)
				{
					m_setRequestedBGEffect.insert(strResourceName);
				}
				else
				{
					std::set<std::tstring>::iterator itr_lazy = m_setRequestedBGEffect.find(strResourceName);
					if (itr_lazy != m_setRequestedBGEffect.end())
						m_setRequestedBGEffect.erase(itr_lazy);
				}
			}

			char* pContext;
			char* token = strtok_s(szNpcId, " ,;", &pContext);
			while (token != NULL)
			{
				int npc_id = atoi(token);
				std::vector<std::tstring>& effects = m_npcEffectPreLoad[npc_id];
				effects.insert(effects.end(), vecEffects.begin(), vecEffects.end());
				token = strtok_s(NULL, " ,;", &pContext);
			}
		}
	}

	if (m_bUsingEffectLog)
	{
		float time = float(tc.Check()) * 0.001f;
		mlog("Effect PreLoad Elapsed time : %f sec\n", time);
	}
}

void XResourceCache::Fini()
{
	for(std::map<std::tstring,REffectInstanceSceneNode*>::iterator it = m_mapPreLoadEffects.begin();
		it != m_mapPreLoadEffects.end(); it++)
	{
		REffectInstanceSceneNode* pEffectInstance = it->second;
		delete pEffectInstance;
	}
	m_mapPreLoadEffects.clear();


	// PreLoaded Resource Fini
	for (std::vector<RResource*>::iterator itr = m_vecPreLoadResource.begin(); itr != m_vecPreLoadResource.end(); ++itr)
		REngine::GetSceneManager().ReleaseResource(*itr);

	m_vecPreLoadResource.clear();

	// 지우기 전에 리소스 캐쉬 지워주기
	for( int i = 0; i < CACHE_MGR_NUM; ++i )
	{
		if (m_arrayCachableResMgr[i])
			m_arrayCachableResMgr[i]->ClearCache();
	}

	for(std::map<std::tstring, XEffectBGWorkUnit*>::iterator itr = m_mapBackgroundWorkUnits.begin();
		itr != m_mapBackgroundWorkUnits.end(); ++itr)
	{
		delete itr->second;
	}
	m_mapBackgroundWorkUnits.clear();

	for(std::vector<XEffectBGWorkUnit*>::iterator itr = m_BackgroundWorkUnitsChecked.begin();
		itr != m_BackgroundWorkUnitsChecked.end(); ++itr)
	{
		delete *itr;
	}
	m_BackgroundWorkUnitsChecked.clear();
}

void XResourceCache::Update( float fDelta )
{
	// BG Effect
	for(std::map<std::tstring, XEffectBGWorkUnit*>::iterator itr = m_mapBackgroundWorkUnits.begin();
		itr != m_mapBackgroundWorkUnits.end();)
	{
		XEffectBGWorkUnit* pWorkUnit = itr->second;
		if (pWorkUnit->IsBGWorkCompleted() && pWorkUnit->CheckDone())
		{
			m_mapBackgroundWorkUnits.erase( itr++ );
			m_BackgroundWorkUnitsChecked.push_back(pWorkUnit);
			//mlog("XModuleBackgroundWork::OnUpdate\n");
		}
		else
		{
			++itr;
		}
	}

	//////////////////////////////////////////////////
	// 리소스 캐싱
	//
	for( int i = 0; i < CACHE_MGR_NUM; ++i )
	{
		if (m_arrayCachableResMgr[i])
			m_arrayCachableResMgr[i]->UpdateCache(fDelta);

		// 아래와 같이 자원 매니저를 통해 사용량을 얻어올수 있습니다.
		// 자원사용량은 리소스 갯수와 비례하는 선형탐색을 하므로 매 프레임당 호출은 좋지 않습니다.
		// pResourceManager->DeleteCache( 지우길 원하는 메모리량(바이트) ) 를 통해 캐쉬를 지울수 있습니다.
		// 캐시를 지울시점은 클라이언트가 적절히 판단하여 지웁니다.
		// 판단을 도울수 있는 도구로는 RDevice에 있는 GetVideoMemory 사이즈(전체 비디오 용량을 리턴),
		// RApplication에 있는 GetSystemMemoryInfo등이 있습니다.
#if 0
		mlog("%d byte 시스템 메모리 사용중, %d byte 비디오 메모리 사용중\n",
			m_arrayCachableResMgr[i]->GetUsingSystemMemory(),
			m_arrayCachableResMgr[i]->GetUsingVideoMemory() );

		mlog("Total Video Memory : %d byte\n", global.device->GetVideoMemorySize());
		DWORD dwTotal, dwAvailable;
		global.app->GetSystemMemoryInfo(dwTotal, dwAvailable);
		mlog("Total System Memory : %d byte, Available System Memory : %d byte\n", dwTotal, dwAvailable);
#endif
	}

	//float fCurrentTime = GetProfileManager().Get_Time_Since_CurrentReset();
	//if (fCurrentTime > 0.1f)
	//	mlog("%f\n", fCurrentTime);
}

void XResourceCache::Clear()
{
	for( int i = 0; i < CACHE_MGR_NUM; ++i )
	{
		if (m_arrayCachableResMgr[i])
			m_arrayCachableResMgr[i]->ClearCache();
	}
}

std::vector< std::tstring >* XResourceCache::GetNPCEffectPreLoadList( int _nNPCId )
{
	std::map< int, std::vector<std::tstring> >::iterator itr = m_npcEffectPreLoad.find(_nNPCId);
	if (itr != m_npcEffectPreLoad.end())
	{
		return &itr->second;
	}

	return NULL;
}

void XResourceCache::LogEffectCache( const std::tstring& _rEffectName, bool bBgCache )
{
	static std::map< std::pair<std::tstring, bool>, int > effectCacheLogInfo;
	static std::set< std::tstring > effectBgCachLogInfo;
	if (_rEffectName.empty())
	{
		FILE* pFile;
		errno_t err = fopen_s(&pFile, "effect_report.txt", "w+");

		if( pFile == NULL || err != 0)
			return;

		std::vector< std::tstring > cached_effect_name, none_cached_effect_name;
		std::vector< int > cached_effect_cnt, none_cached_effect_cnt;
		for (std::map< std::pair<std::tstring, bool>, int >::iterator itr = effectCacheLogInfo.begin();
			itr != effectCacheLogInfo.end(); ++itr)
		{
			if (itr->first.second == true)
			{
				cached_effect_name.push_back(itr->first.first);
				cached_effect_cnt.push_back(itr->second);
			}
			else
			{
				none_cached_effect_name.push_back(itr->first.first);
				none_cached_effect_cnt.push_back(itr->second);
			}
		}

		fprintf(pFile, "==== START Report ====\n");
		fprintf(pFile, "Cached Effect\n");
		int nCnt = (int)cached_effect_cnt.size();
		for (int i = 0; i < nCnt; ++i)
		{
			fprintf(pFile, "	%s : %d\n", MLocale::ConvTCHARToAnsi(cached_effect_name[i].c_str()).c_str(), cached_effect_cnt[i]);
		}

		fprintf(pFile, "None Cached Effect\n");
		nCnt = (int)none_cached_effect_cnt.size();
		for (int i = 0; i < nCnt; ++i)
		{
			fprintf(pFile, "	%s : %d\n", MLocale::ConvTCHARToAnsi(none_cached_effect_name[i].c_str()).c_str(), none_cached_effect_cnt[i]);
		}

		fprintf(pFile, "BG Cache Requested Effect\n");
		for (std::set< std::tstring >::iterator itr_bg = effectBgCachLogInfo.begin(); itr_bg != effectBgCachLogInfo.end(); ++itr_bg)
		{
			fprintf(pFile, "	%s\n", MLocale::ConvTCHARToAnsi(itr_bg->c_str()).c_str());
		}

		fprintf(pFile, "==== END Report ====\n");
		fclose(pFile);
	}
	else
	{
		if (bBgCache)
		{
			effectBgCachLogInfo.insert(_rEffectName);
		}
		else
		{
			bool bCache = false;
			if (NULL != REngine::GetSceneManager().GetEffectManager()->GetEffect(MLocale::ConvTCHARToAnsi(_rEffectName.c_str()).c_str()))
				bCache = true;

			std::pair<std::tstring, bool> key(_rEffectName, bCache);
			if (effectCacheLogInfo.find(key) != effectCacheLogInfo.end())
			{
				effectCacheLogInfo[key] += 1;
			}
			else
			{
				effectCacheLogInfo[key] = 1;
			}
		}
	}
}

REffectInstanceSceneNode* XResourceCache::CreateEffectWithBGCache( const tstring& str )
{
	bool bLazyLoading = false;
	std::set<std::tstring>::iterator itr_lazy = m_setRequestedBGEffect.find(str);
	if (itr_lazy != m_setRequestedBGEffect.end())
	{
		bLazyLoading = true;
		if (NULL != REngine::GetSceneManager().GetEffectManager()->GetEffect(MLocale::ConvTCHARToAnsi(str.c_str()).c_str()))
		{
			// 캐싱이 되어있다면 lazy loding할 필요가 없다.
			bLazyLoading = false;
			m_setRequestedBGEffect.erase(itr_lazy);
		}
	}

	// lazy loading이 아닌경우
	if (false == bLazyLoading)
	{
		REffectInstanceSceneNode* pEffectInstance = static_cast<REffectInstanceSceneNode*>(REngine::GetSceneManagerPtr()->CreateSceneNode(RSID_EFFECTINSTANCE));
		if (IsUsingEffectCacheLog())
			LogEffectCache(str, false);

		if( !pEffectInstance->Create( MLocale::ConvTCHARToAnsi(str.c_str()).c_str() ) )
		{
			#ifdef _DEBUG
			mlog("CreateEffect 실패(%s)\n", MLocale::ConvTCHARToAnsi(str.c_str()).c_str());
			#endif
			SAFE_DELETE(pEffectInstance);
			return NULL;
		}

		return pEffectInstance;
	}

	// lazy loading인 경우
	std::map<std::tstring, XEffectBGWorkUnit*>::iterator itr = m_mapBackgroundWorkUnits.find(str);
	if (itr == m_mapBackgroundWorkUnits.end())
	{
		// 처음요청

		XEffectBGWorkUnit* pBGUnit = new XEffectBGWorkUnit(str);
		REngine::GetBackgroundWorker().AddToReadyQueue(pBGUnit);
		REngine::GetBackgroundWorker().ReOrderReadyQueue();
		REngine::GetBackgroundWorker().FlushReadyQueueAndRun();

		m_mapBackgroundWorkUnits.insert( std::pair<std::tstring, XEffectBGWorkUnit*>(str, pBGUnit) );
		return NULL;
	}
	else
	{
		// 요청중이라면
		return NULL;
	}

	_ASSERT(0);
	return NULL;
}
