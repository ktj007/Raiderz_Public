#include "stdafx.h"
#include "XGameState.h"
#include "XController.h"
#include "XEffectManager.h"
#include "XEngine.h"
#include "XNaming.h"
#include "XNetwork.h"
#include "XSensorManager.h"

XGameState::XGameState( XGameStateMgr * pStateMgr )
{
	m_pWorld			= NULL;
	m_pController		= NULL;
	m_pObjectManager	= NULL;
	
	m_pSensorManager	= new XSensorManager();

	m_bRealGameNotTest	= false;

	// 게임 스테이트가 생성 전에 오브젝트 메니저가 글로벌에 등록이 되어 있어야 합니다.
	m_pObjectManager = gg.omgr;

	m_pOwnerStateMgr = pStateMgr;
}

XGameState::~XGameState()
{
	SAFE_DELETE(m_pSensorManager);
	if(global.net &&
		global.net->GetNetClient())
	{
		global.net->GetNetClient()->SetControlNetCmdState(NULL);
	}
}

bool XGameState::Create()
{
	SetControlNetCmd();

	return true;
}

bool XGameState::Ready( XGameState* pPreState, int nParam /*= 0*/ )
{
	if(global.net &&
		global.net->GetNetClient())
	{
		global.net->GetNetClient()->SetControlNetCmdState(&m_ControlNetCmdState);
	}

	return true;
}

void XGameState::Destroy()
{
	if(m_pWorld)
		m_pWorld->Final();

	SAFE_DELETE(m_pWorld);

	if (m_pController)
		m_pController->Destroy();

	SAFE_DELETE(m_pController);
}

void XGameState::Update( float fDelta )
{
	PFI_B(201, "XGame::Update : m_pWorld->Update" );
	PFC_B("XGame::Update : m_pWorld->Update");
	if(m_pWorld)
		m_pWorld->Update(fDelta);
	PFC_E;
	PFI_E(201);
}

void XGameState::OnWorldLoadingComplete()
{
	if(m_pWorld)
	{
		m_pWorld->OnWorldLoadingComplete();
	}
}

void XGameState::Exit()
{
	//////////////////////////////////////////////////////////////////////////
	// 삭제
	// 이펙트 삭제
	global.emgr->Destroy();

	// 사운드 삭제
	//global.sound->Destroy();

	//m_pWorld->Destroy();
	//SAFE_DELETE(m_pWorld);

	// 오브젝트 삭제
	gg.omgr->Destroy();
}

bool XGameState::CheckModelLoadingAsync( const wstring& strResourceName )
{
	RMesh* pMesh = global.engine->CreateMesh(strResourceName.c_str());

	bool bAsync = true;
	if(pMesh->GetState() == RRS_LOADED)
	{
		bAsync = false;
	}

	global.engine->ReleaseMesh(pMesh);

	return bAsync;
}

bool XGameState::CheckModelLoadingAsync( SEX nSex )
{
	return CheckModelLoadingAsync(XNaming::GetPlayerModelName(nSex));
}

void XGameState::ChangeTimeWeather( GAME_TIME_TYPE nGameTime, GAME_WEATHER_TYPE nWeather )
{
	if(gg.currentgamestate->GetWorld())
	{
		if((gvar.Game.nLocalWeather == WEATHER_INVALID) && (gvar.Game.nLocalTime == TIME_INVALID))
		{
			gg.currentgamestate->GetWorld()->OnCmdSetEnv(nGameTime, nWeather, true, GAME_ENVIRONMENT_FADING_TIME);
		}
		else if(gvar.Game.nLocalWeather == WEATHER_INVALID)
		{
			gg.currentgamestate->GetWorld()->OnCmdSetWeather(nWeather, true, GAME_ENVIRONMENT_FADING_TIME);
		}
		else if(gvar.Game.nLocalTime == TIME_INVALID)
		{
			gg.currentgamestate->GetWorld()->OnCmdSetTime(nGameTime, true, GAME_ENVIRONMENT_FADING_TIME);
		}
	}
}