#pragma once

#define _SHADER30

// PHYSIQUE_QT를 이곳에서 디파인 한다. 코드 전반에 걸쳐 작용 해야 해서..
// 쿠터니언을 가지고 에니메이션 시키기 위해서는 Uniform Scale만 가능하다. 
// 하지만 현재 그러한 가정이 유효하지 않다.
//#define USING_RSC_PHYSIQUE_DQ
//#define USING_RSC_PHYSIQUE_QT


#include "RTimer.h"
#include "RConfiguration.h"
#include "RProfileInfo.h"
#include "RSystemInfo.h"
#include "RBackgroundWorker.h"
#include "RListener.h"

#include <set>
#include <string>

#define INIT_TCHECK( msg )	\
RTimeChecker100us tc; bool bCheckTime = false;		\
if (false == REngine::m_strDebugFlags.empty()){		\
	if (REngine::m_strDebugFlags.find(msg) != std::string::npos) bCheckTime = true;}

#define BEGIN_TCHECK	if(bCheckTime)	tc.Start();
#define END_TCHECK( msg )	\
if (bCheckTime){			\
	REngine::m_uintDebugInfo.push_back(tc.Check());	\
	REngine::m_strDebugInfo.push_back(msg);}

namespace rs3 {

class RDevice;
class RTimer;
class RSceneManager;
class RConfiguration;
class RResourceProfiler;
class REventBroadcaster;
class RSystemInfo;

class RS_API REngine
{
public:
	inline static RBackgroundWorker&	GetBackgroundWorker()	{ return m_BackgroundWorker; }
	inline static RTimer&				GetTimer()				{ return m_timer; }
	inline static RConfiguration&		GetConfig()				{ return m_config; }
	inline static RResourceProfiler&	GetResourceProfiler()	{ return m_ResourceProfiler; }
	inline static RSystemInfo&			GetSystemInfo()			{ return m_SystemInfo; }

	inline static RDevice& GetDevice()
	{
		_ASSERT(m_pDevice != NULL);
		return *m_pDevice;
	}

	inline static RDevice* GetDevicePtr()
	{
		return m_pDevice;
	}

	inline static RSceneManager& GetSceneManager()
	{
		_ASSERT(m_pSceneManager != NULL);
		return *m_pSceneManager;
	}

	inline static RSceneManager* GetSceneManagerPtr()
	{
		return m_pSceneManager;
	}

	static void RegisterDevice( RDevice* pDevice ){ m_pDevice = pDevice; }
	static void RegisterSceneManager( RSceneManager* pSceneManager ){ m_pSceneManager = pSceneManager; }

	static bool InitSharedResource();
	static void DestroySharedResource();

	// event
	inline static REventBroadcaster* GetEventBroadcaster(REVENT eEvent)
	{
		if(eEvent<RE_NONE || eEvent>=RE_END) return NULL;
		return &m_eventBroadcasters[eEvent];
	}

	inline static bool BroadcastEvent(REVENT eEvent)
	{
		REventBroadcaster* pEventBroadcaster = GetEventBroadcaster( eEvent );
		if(!pEventBroadcaster) return false;

		pEventBroadcaster->Notify();

		return true;
	}

	// 개발 버전 엔진인가?
	static bool IsDevelopVersion() { return false; }

	static bool						m_bUseSkinning;

	static std::string					m_strDebugFlags;
	static std::vector<unsigned int>	m_uintDebugInfo;
	static std::vector<std::string>		m_strDebugInfo;

private:
	REngine(){}
	~REngine(){}

	static RDevice*					m_pDevice;
	static RSceneManager*			m_pSceneManager;

	static RBackgroundWorker&		m_BackgroundWorker;
	static RTimer&					m_timer;
	static RConfiguration&			m_config;
	static RResourceProfiler&		m_ResourceProfiler;
	static RSystemInfo&				m_SystemInfo;
	static bool						m_bInited;
	static REventBroadcaster		m_eventBroadcasters[RE_END];	// event broadcaster
};

}
