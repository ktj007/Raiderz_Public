#pragma once

#include "RResourceManager.h"
#include "MTstring.h"

using namespace rs3;

namespace rs3 {
	class REffectInstanceSceneNode;
}

class XEffectBGWorkUnit;
class XResourceCache
{
private:
	enum _CACHABLE_RES_MGR
	{
		CACHE_MESH = 0,
		//		CACHE_EFFECT,

		CACHE_MGR_NUM
	};

	RResourceManager*			m_arrayCachableResMgr[CACHE_MGR_NUM]; // 캐싱가능한 리소스 관리자
	std::vector<RResource*>		m_vecPreLoadResource;

	bool				m_bUsingEffectCache;
	bool				m_bUsingEffectLog;

	std::map<std::tstring,REffectInstanceSceneNode*>	m_mapPreLoadEffects;
	std::map< int, std::vector<std::tstring> >			m_npcEffectPreLoad;

	std::set<std::tstring>								m_setRequestedBGEffect;
	std::map<std::tstring, XEffectBGWorkUnit*>			m_mapBackgroundWorkUnits;
	std::vector<XEffectBGWorkUnit*>						m_BackgroundWorkUnitsChecked;

	// cacher init
	void _InitMeshCache(rs3::RSceneManager* pSceneManager);
	void _InitEffectCache();

	// PreLoading resource init
	void _InitResourcePreLoad(const TCHAR* szPreLoad);

public:
	XResourceCache() 
	{
		for (int i = 0; i < CACHE_MGR_NUM; i++)
		{
			m_arrayCachableResMgr[i] = NULL;
		}

		m_bUsingEffectCache = true;
		m_bUsingEffectLog = false;
	}
	~XResourceCache() {}
	void Init(rs3::RSceneManager* pSceneManager, const TCHAR* szPreLoad);
	void Fini();
	void Update(float fDelta);
	void Clear();

	std::vector< std::tstring >* GetNPCEffectPreLoadList(int _nNPCId);
	bool IsUsingEffectCache(){return m_bUsingEffectCache; }
	bool IsUsingEffectCacheLog(){ return m_bUsingEffectLog; }
	void LogEffectCache( const std::tstring& _rEffectName, bool bBgCache );
	REffectInstanceSceneNode* CreateEffectWithBGCache(const tstring& str);
};
