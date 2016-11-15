#include "stdafx.h"

#include "RS3UnitTestUtility.h"
#include "REngine.h"
#include "RSceneManager.h"

#include "RResource.h"
#include "RResourceManager.h"

using namespace rs3;

class RTestResource : public RGeneralResource
{
public:
	RTestResource(const char* _pSzResourceName, RSceneManager* pSceneManager) : RGeneralResource(_pSzResourceName,pSceneManager)
	{
		p = NULL;
	}

	virtual ~RTestResource()
	{
		delete p;
		p = NULL;
	}

	virtual bool Fill()
	{
		p = new int;
		Sleep(200);
		*p = 10;
		Sleep(200);
		return true;
	}

	int* p;
};

TEST( TestBackgroundLoading, Deletion_Resource_While_BackgroundLoading )
{
	NullDeviceEngineCreater nullEngine;

	RSceneManager* pSceneMgr = REngine::GetSceneManagerPtr();
	EXPECT_TRUE( NULL != pSceneMgr );
	if (REngine::GetBackgroundWorker().IsInited() == false)
	{
		REngine::GetBackgroundWorker().Init();
	}

	RTestResource* pTestResource = pSceneMgr->CreateResource<RTestResource>("_null_");			EXPECT_TRUE( NULL != pTestResource );
	RGeneralResourceManager* pResMgr = pSceneMgr->GetResourceMgr<RGeneralResourceManager>();	EXPECT_TRUE( NULL != pResMgr );
	
	pTestResource->BuildAll(true, NULL);
	Sleep(100);

	pSceneMgr->ReleaseResource(pTestResource);
	REngine::GetBackgroundWorker().Update();
	REngine::GetBackgroundWorker().Destroy();
}
