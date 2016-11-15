#include "StdAfx.h"
#include "REngine.h"

#include "RFont.h"
#include "RStaticMeshSceneNode.h"
#include "RSpotLightSceneNode.h"
#include "RTerrain.h"
#include "RBackgroundWorker.h"
#include "RMesh.h"
#include "RActor.h"


namespace rs3 {

RBackgroundWorker backgroundWorker;
RBackgroundWorker& REngine::m_BackgroundWorker = backgroundWorker;



RTimer timer;
RTimer& REngine::m_timer = timer;

RConfiguration config;
RConfiguration& REngine::m_config = config;

RResourceProfiler profileInfo;
RResourceProfiler& REngine::m_ResourceProfiler = profileInfo;

RSystemInfo systemInfo;
RSystemInfo& REngine::m_SystemInfo = systemInfo;

RDevice* REngine::m_pDevice = NULL;
RSceneManager* REngine::m_pSceneManager = NULL;

REventBroadcaster REngine::m_eventBroadcasters[RE_END];

bool REngine::m_bInited = false;

bool REngine::m_bUseSkinning = true;

string REngine::m_strDebugFlags;
std::vector<unsigned int> REngine::m_uintDebugInfo;
std::vector<std::string> REngine::m_strDebugInfo;

bool REngine::InitSharedResource()
{
	_ASSERT( m_bInited == false);

	if(!RFont::Initialize(REngine::GetDevicePtr()))
	{
		mlog_setlasterror("Failed to initialize the font system.\n");
		return false;
	}

	if(!RMesh::InitSharedResources(REngine::GetDevicePtr()))
	{
		return false;
	}

	_ASSERT( REngine::GetSceneManagerPtr() );
	if(!REngine::GetSceneManagerPtr()->InitSharedResources())
	{
		mlog_setlasterror("Failed to initialize the shader system.\n");
		return false;
	}

	m_bInited = true;

	return true;
}

void REngine::DestroySharedResource()
{
//	_ASSERT( m_bInited == true );

	// static object ÇØÁ¦
	RFont::Finalize();
	RStaticMeshSceneNode::Destroy();
	RMesh::DestroySharedResources();

	_ASSERT( REngine::GetSceneManagerPtr() );
	REngine::GetSceneManagerPtr()->DestroySharedResources();

	m_bInited = false;
}

}