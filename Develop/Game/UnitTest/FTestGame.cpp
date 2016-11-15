#include "stdafx.h"
#include "FTestGame.h"
#include "TestModule.h"
#include "XNetwork.h"
#include "MockSystem.h"


FTestGame::FTestGame()
{
	m_pNetAgent = new minet::MTestNetAgent(global.net->GetClient());
	m_pTestContext = new TestModuleContext(&m_TestGame, m_pNetAgent);

	m_pGame = m_TestGame.GetGame();

	m_Helper.SetAniDataSet_NPCSample1(&m_NPCAniDataSet);
	m_Helper.SetAniDataSet_Player(&m_PCAniDataSet);

	XExpectNowTime(0);
}

FTestGame::~FTestGame()
{
	SAFE_DELETE(m_pTestContext);
	SAFE_DELETE(m_pNetAgent);
}

XNetPlayer* FTestGame::NewNetPlayer( vec3 vPos/*=vec3::ZERO*/, vec3 vDir/*=vec3(0.0f, 1.0f, 0.0f)*/, SEX nSex/*=SEX_FEMALE*/ )
{
	return m_pTestContext->CreateNetPlayerAndEnterToGmae(vPos, vDir, nSex);
}

XNonPlayer* FTestGame::NewNonPlayer( int nNPCID, vec3 vPos/*=vec3::ZERO*/, vec3 vDir/*=vec3(0.0f, 1.0f, 0.0f)*/ )
{
	return m_pTestContext->CreateNPCAndEnterToGame(nNPCID, vPos, vDir);
}

XMyPlayer* FTestGame::GetMyPlayer()
{
	return m_TestGame.GetMyPlayer();
}

void FTestGame::Update( float fDelta )
{
	m_TestGame.Update(fDelta);

	uint32 nDeltaMS = fDelta * 1000;

	XExpectNowTime(XGetNowTime() + nDeltaMS);
}

XNPCInfo* FTestGame::NewNPCInfo( int nNPCID, const wchar_t* szName, int nGrade, const wchar_t* szMeshName, const wchar_t* szMeshPath )
{
	return XUnitTestHelper::NewNPCInfo(nNPCID, szName, nGrade, szMeshName, szMeshPath);
}