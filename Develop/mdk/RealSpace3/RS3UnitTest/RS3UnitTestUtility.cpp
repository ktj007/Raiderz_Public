#include "stdafx.h"
#include "RS3UnitTestUtility.h"
#include "REngine.h"
#include "RSceneManager.h"
#include "RDeviceNull.h"

using namespace rs3;

NullDeviceEngineCreater::NullDeviceEngineCreater()
{
	// 디바이스 생성
	RDeviceNull* pDevice = new RDeviceNull;
	bool bRet = pDevice->Create( NULL );
	_ASSERT( bRet == true );
	REngine::RegisterDevice( pDevice );

	// 씬 매니저 생성
	RSceneManager* pSceneManager = new RSceneManager;
	REngine::RegisterSceneManager( pSceneManager );
}

NullDeviceEngineCreater::~NullDeviceEngineCreater()
{
	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
	_ASSERT( pSceneManager != NULL );

	// 씬매니저
	pSceneManager->Clear();
	pSceneManager->Destroy();
	delete pSceneManager;
	REngine::RegisterSceneManager( NULL );

	// 디바이스
	RDevice* pDevice = REngine::GetDevicePtr();
	_ASSERT( pDevice != NULL );
	pDevice->Destroy();
	delete pDevice;
	REngine::RegisterDevice(NULL);
}

void NullDeviceFixture::UpdateSceneManager( DWORD dwElapsed /*= 1*/ )
{
	REngine::GetSceneManager().UpdateTimerCustom( dwElapsed );
	REngine::GetSceneManager().Update();
}
