#pragma once

#include "TestGame.h"
#include "TestCommonFixture.h"

#include "XGame.h"
#include "XSystem.h"

#include "XMyPlayer.h"
#include "XNetPlayer.h"
#include "XNonPlayer.h"

#include "MockNetClient.h"
#include "MockController.h"
#include "MockTalentInfo.h"
#include "MockMeshInfo.h"

#include "XModuleCollision.h"
#include "XModuleEntity.h"
#include "XModuleUI.h"

#include "MWidget.h"

class TestModuleContext
{
private:

	TestGame*						m_pTestGame;
	minet::MTestNetAgent*			m_pNetAgent;
	MockTalentInfoMgr*				m_pTalentInfoMgr;
	void							_Init(TestGame* pTestGame, minet::MTestNetAgent* pNetAgent);

	template< typename T > struct ModuleToType {};
	template<> struct ModuleToType<XModuleNetControl>	{ enum {Result = XMID_PLAYER_CONTROL}; };
	template<> struct ModuleToType<XModuleMyControl>	{ enum {Result = XMID_PLAYER_CONTROL}; };
	template<> struct ModuleToType<XModuleMovable>		{ enum {Result = XMID_MOVABLE}; };
	template<> struct ModuleToType<XModuleTalent>		{ enum {Result = XMID_TALENT}; };
	template<> struct ModuleToType<XModuleMotion>		{ enum {Result = XMID_MOTION}; };
	template<> struct ModuleToType<XModuleCollision>	{ enum {Result = XMID_COLLISION}; };
	template<> struct ModuleToType<XModuleEntity>		{ enum {Result = XMID_ENTITY}; };
	template<> struct ModuleToType<XModuleUI>			{ enum {Result = XMID_UI}; };


public:

	TestModuleContext(TestGame* pTestGame, minet::MTestNetAgent* pNetAgent);
	TestModuleContext(SimpleTestFixture* pSimpleTestFixture);
	~TestModuleContext();


	//////////////////////////////////////////////////////////////////////////
	// Game Context 업데이트
	//
	void							UpdateGame(float fSec);


	//////////////////////////////////////////////////////////////////////////
	// 게임 상에서 사용하는 Talent
	//
	bool							CreateTalent( const std::wstring& _strXmlData );
	XTalentInfo*					GetTalent(int nID);
	void							TalentClear();


	//////////////////////////////////////////////////////////////////////////
	// 게임 액터 관련 : MyPlayer, NetPlayer, NPC
	//
	XMyPlayer*						GetMyPlayer();
	XNetPlayer*						CreateNetPlayerAndEnterToGmae(const vec3& vPos = vec3::ZERO, const vec3& vDir = vec3::AXISX, SEX eSex = SEX_FEMALE);
	XNonPlayer*						CreateNPCAndEnterToGame(int nNPCID, const vec3& vPos = vec3::ZERO, const vec3& vDir = vec3::AXISX, MCapsule* pColCapsule=NULL, const wchar_t* szMeshName=NULL);

	template<typename T> inline T*	GetModule( XActor* pOwnerActor);	// 게임 액터의 모듈


	//////////////////////////////////////////////////////////////////////////
	// Context 만들기
	//
	bool							MakeContextFieldLoadingCompleteIgnore();
	bool							MakeContextRecvNetMoveCmd(XNetPlayer* pMoved, const vec3* pMovedPosIfChanged = NULL, const vec3* pMovedDirIfChanged = NULL, MOVEMENT_FLAG _eMovementFlag = MOVEMENTFLAG_FORWARD);

};
//----------------------------------------------------------------------------------------------------
template<typename T> inline T* TestModuleContext::GetModule( XActor* pOwnerActor)
{
	return static_cast<T*>( pOwnerActor->GetModule(ModuleToType<T>::Result));
}
//----------------------------------------------------------------------------------------------------