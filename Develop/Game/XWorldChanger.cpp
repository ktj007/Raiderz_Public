#include "stdafx.h"
#include "XWorldChanger.h"
#include "XGlobal.h"
#include "XGlobalVar.h"
#include "XController.h"
#include "XModuleMyControl.h"
#include "XMyPlayer.h"
#include "XCameraManager.h"
#include "XWorld.h"
#include "XResourceManager.h"
#include "XEffectManager.h"
#include "XSensorManager.h"
#include "XFieldInfoBuilder.h"

XWorldChanger::XWorldChanger() : m_pNextFieldInfo(NULL), m_nLastFieldZoneID(-1)
{

}

XWorldChanger::~XWorldChanger()
{
	SAFE_DELETE(m_pNextFieldInfo);
}

void XWorldChanger::ChangeField( XObjectManager* pObjectManager, XWorld* pWorld, XSensorManager* pSensorManager, int nNewFieldID, vec3 vMyPos, vec3 vMyDir )
{
	FinalGame_ForChangeField(pObjectManager, pWorld, nNewFieldID);
	InitGame_ForChangeField(pObjectManager, pWorld, pSensorManager, nNewFieldID, vMyPos, vMyDir);
}

void XWorldChanger::FinalGame_ForChangeField(XObjectManager* pObjectManager, XWorld* pWorld, int nNextFieldID)
{
	m_nLastFieldZoneID = pWorld->GetInfo()->m_nZoneID;
	SetupNextFieldInfo(nNextFieldID);

	CleanSound();

	// 필드를 바꾸기 전에 내 상태를 초기화하도록 한다
	CleanController();

	CleanMyPlayer(pObjectManager);

	// stop async work
	// CLEAR_QUEUE_DEBUG
	//REngine::GetBackgroundWorker().ClearQueues();

	pObjectManager->DestroyAllObject();

	if (IsSameZone())
	{
		pWorld->FinalForChange();
	}
	else
	{
		pWorld->Final();
	}

	CleanCamera();

	// scene manager clear
	global.emgr->Destroy();
	//global.smgr->Clear();

	if (!IsSameZone())
	{
		// 일단 리소스 캐쉬는 맵을 바꾸기전에 다 지워주는 정책
		if (global.res)
		{
			global.res->ClearCaching();
		}

		// D3D Managed 리소스 정리
		global.device->GetD3DDevice()->EvictManagedResources();
	}

	CleanHeaps();
}

void XWorldChanger::CleanHeaps()
{
	// 힙 메모리 정리
	{
		SIZE_T largestDecommitSize;
		PHANDLE phHeaps = new HANDLE[25];
		DWORD dwHeaps = GetProcessHeaps( 25, phHeaps );
		if( dwHeaps > 25 )
		{
			SAFE_DELETE_ARRAY( phHeaps );
			phHeaps = new HANDLE[dwHeaps];
			GetProcessHeaps( dwHeaps, phHeaps );
		}

		for( DWORD i = 0; i < dwHeaps; ++i )
		{
			largestDecommitSize = HeapCompact( phHeaps[i], 0 );
			//	mlog( "### HeapCompact %d, %d\n", i, largestDecommitSize );
		}
		SAFE_DELETE_ARRAY( phHeaps );
	}
}

void XWorldChanger::CleanController()
{
	if ( gg.controller) 
	{
		gg.controller->SetEnabled( false);
		gg.controller->ClearKeyBuffer();
	}
}

void XWorldChanger::CleanMyPlayer(XObjectManager* pObjectManager)
{
	XPlayer* pMyPlayer = static_cast<XPlayer*>( gg.omgr->Find( XGetMyUID()));
	if ( pMyPlayer)
	{
		XModuleMyControl* pMyControl = pMyPlayer->GetModuleMyControl();
		if ( pMyControl)
		{
			pMyControl->Final_ForChangeField();
		}
	}

	// clear world object
	if (gvar.Game.pMyPlayer)
	{
		pObjectManager->Erase(gvar.Game.pMyPlayer);
		gvar.Game.pMyPlayer->RemoveFromSceneManager();
	}
}

void XWorldChanger::CleanCamera()
{
	// clear scene object
	// 카메라는 해제하지 않는다.
	if (global.camera->GetCamera())
	{
		global.camera->GetCamera()->RemoveFromParent();
	}
}

void XWorldChanger::CleanSound()
{
	if ( global.sound)
		global.sound->StopAll();
}

//////////////////////////////////////////////////////////////////////////

void XWorldChanger::InitGame_ForChangeField( XObjectManager* pObjectManager, XWorld* pWorld, XSensorManager* pSensorManager, int nNextFieldID, vec3 vMyPos, vec3 vMyDir )
{
	pSensorManager->Clear();

	gvar.Game.Init_ForChangeField();

	InitRes(nNextFieldID);

	// init world
	XWorldInitParam worldInitParam;
	worldInitParam.SetField(nNextFieldID, true);

	if (IsSameZone())
	{
		pWorld->InitForChange(worldInitParam);
	}
	else
	{
		pWorld->Init(worldInitParam);
	}



	global.smgr->AddSceneNode(global.camera->GetCamera());

	InitMyPlayer(pObjectManager, vMyPos, vMyDir);
}


void XWorldChanger::InitMyPlayer( XObjectManager* pObjectManager, vec3 vMyPos, vec3 vMyDir )
{
	XMyPlayer* pMyPlayer = gvar.Game.pMyPlayer;

	if (pMyPlayer)
	{
		pMyPlayer->AddToSceneManager();
	}

	// create user and init
	pObjectManager->Add(pMyPlayer);

	if (pMyPlayer)
	{
		if (pMyPlayer->IsModuleEntity())
		{
			pMyPlayer->SetPosition(vMyPos);
			pMyPlayer->SetDirection(vMyDir);
			pMyPlayer->SetUpVector(vec3(0.0f, 0.0f, 1.0f));
		}

		//global.camera->GetCamera()->SetDirection(vMyDir);
		gg.controller->SetBackViewCameraDir(vMyDir);

		XModuleMyControl* pModuleMyControl = pMyPlayer->GetModuleMyControl();
		if (pModuleMyControl)
		{
			pModuleMyControl->Init_ForChangeField();
		}
	}
}

void XWorldChanger::InitRes( int nFieldID )
{
	wstring strNextZoneName = info.field->GetZoneNameFromFieldID(nFieldID);
	if (!strNextZoneName.empty())
	{
		wstring strZoneFileName( PATH_ZONE + strNextZoneName + L"/" + strNextZoneName + FILEEXT_ZONE_RES );
		global.res->GrabFieldResource(strZoneFileName);
	}
}

void XWorldChanger::SetupNextFieldInfo(int nNextFieldID)
{
	if (m_pNextFieldInfo) delete m_pNextFieldInfo;

	XFieldInfoBuilder fieldInfoBuilder;
	m_pNextFieldInfo = fieldInfoBuilder.NewFieldInfo(global.app->GetMode(), nNextFieldID, NULL);
}

bool XWorldChanger::IsSameZone()
{
	if (m_pNextFieldInfo && m_nLastFieldZoneID > -1)
	{
		return (m_nLastFieldZoneID == m_pNextFieldInfo->m_nZoneID);
	}
	return false;
}

