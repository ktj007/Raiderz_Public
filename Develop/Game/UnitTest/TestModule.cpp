#include "stdafx.h"
#include "TestModule.h"
#include "XModuleEntity.h"
#include "XUnitTestUtil.h"
#include "MockCharacter.h"
#include "XMotionMgr.h"

void TestModuleContext::_Init( TestGame* pTestGame, minet::MTestNetAgent* pNetAgent )
{
	// TestGame
	_ASSERT( pTestGame != NULL );
	m_pTestGame = pTestGame;

	// NetAgent
	_ASSERT( pNetAgent != NULL );
	m_pNetAgent = pNetAgent;

	// TalentInfo
	m_pTalentInfoMgr = static_cast<MockTalentInfoMgr*>(info.talent);
}

TestModuleContext::TestModuleContext( TestGame* pTestGame, minet::MTestNetAgent* pNetAgent )
{
	_Init(pTestGame, pNetAgent);
}

TestModuleContext::TestModuleContext(SimpleTestFixture* pSimpleTestFixture)
{
	_Init(pSimpleTestFixture->m_pGame, pSimpleTestFixture->m_pNetAgent);
}

TestModuleContext::~TestModuleContext()
{
}

// XActor
XNetPlayer* TestModuleContext::CreateNetPlayerAndEnterToGmae(const vec3& vPos, const vec3& vDir, SEX eSex)
{
	TD_UPDATE_CACHE_PLAYER td_update_cache_player;

	MUID uidPlayer = global.system->GenerateLocalUID();
	UIID nPlayerUIID = XUnitTestUtil::NewUIID();

	td_update_cache_player.uid = uidPlayer;
	td_update_cache_player.nUIID = nPlayerUIID;
	td_update_cache_player.vPos = vPos;
	td_update_cache_player.svDir = vDir;
	td_update_cache_player.nStance = CS_NORMAL;
	td_update_cache_player.nStatusFlag = 0;
	wcsncpy_s(td_update_cache_player.szName, L"testmodule_netplayer", _TRUNCATE);

	// net player feature
	td_update_cache_player.Feature.nSex = eSex;
	td_update_cache_player.Feature.nHair = 0;
	td_update_cache_player.Feature.nFace = 0;
	td_update_cache_player.Feature.nHairColor = 0;
	td_update_cache_player.Feature.nSkinColor = 0;
	td_update_cache_player.Feature.nWeaponSet = 0;
	for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
		td_update_cache_player.Feature.nItemID[i] = 0;
	for (int i = 0; i < FEATURE_ITEMSLOT_MAX; i++)
		td_update_cache_player.Feature.nItemID_DyedColor[i] = 0;

	XPlayer* pPlayer = m_pTestGame->GetGame()->GetCurrentGameState()->InPlayer(uidPlayer, &td_update_cache_player);
	_ASSERT(MIsExactlyClass(XNetPlayer, pPlayer) == true);

	MockAnimationController* ac = dynamic_cast<MockAnimationController*>(pPlayer->GetModuleEntity()->GetAnimationController());
	if (ac)
	{
		ac->SetAniDataSet(&m_pTestGame->GetPlayerAniDataSet());
	}

	pPlayer->GetModuleEntity()->GetActor()->m_pMesh = m_pTestGame->GetDefaultMesh();
	pPlayer->GetActor()->SetAnimationUpdated(true);

	return AsNetPlayer(pPlayer);
}

XMyPlayer* TestModuleContext::GetMyPlayer()
{
	return m_pTestGame->GetMyPlayer();
}

bool TestModuleContext::CreateTalent( const std::wstring& _strXmlData )
{
	return m_pTalentInfoMgr->CreateTalent(_strXmlData);
}

XTalentInfo* TestModuleContext::GetTalent(int nID)
{
	return m_pTalentInfoMgr->Get(nID);
}

void TestModuleContext::UpdateGame(float fSec)
{
	m_pTestGame->Update(fSec);
}

XNonPlayer* TestModuleContext::CreateNPCAndEnterToGame( int nNPCID, const vec3& vPos /*= vec3::ZERO*/, const vec3& vDir /*= vec3::AXISX*/, MCapsule* pColCapsule, const wchar_t* szMeshName )
{
	TD_UPDATE_CACHE_NPC td_update_cache_npc = XUnitTestHelper::Make_TD_UPDATE_CACHE_NPC(global.system->GenerateLocalUID(),
												nNPCID,
												vPos,
												vDir);

	SetBitSet(td_update_cache_npc.nStatusFlag, UNS_ATTACKABLE_ALWAYS);

	m_pTestGame->GetGame()->GetCurrentGameState()->InNPCs(&td_update_cache_npc, 1);
	XNonPlayer* pNPC = m_pTestGame->GetGame()->GetCurrentGameState()->GetObjectManager()->FindNPC(td_update_cache_npc.uid);

	if (pNPC)
	{
		MCapsule capCol(vec3(0.0f, 0.0f, 40.0f), vec3(0.0f, 0.0f, 150.0f), 40.0f);
		if (pColCapsule) capCol = *pColCapsule;

		if (pNPC)
		{
			wchar_t colName[64];
			if (szMeshName) wcscpy_s(colName, szMeshName);
			else swprintf_s(colName, L"NPC_%d", nNPCID);

			// 메쉬 충돌 정보 입력
			XUnitTestHelper::SetMeshColCapsule(colName, capCol);

			MockCharacter* pMockCharacter = dynamic_cast<MockCharacter*>(pNPC->GetActor());
			if (pMockCharacter)
			{
				pMockCharacter->CreateMeshForCol(colName);
			}

			pNPC->GetActor()->SetAnimationUpdated(true);

			CSMeshInfo* pMeshInfo = info.mesh_info->GetInfo(colName);
			pNPC->GetModuleCollision()->InitCol(pMeshInfo);
		}
	}

	return pNPC;
}

bool TestModuleContext::MakeContextRecvNetMoveCmd( XNetPlayer* pMoved, const vec3* pMovedPosIfChanged /*= NULL*/, const vec3* pMovedDirIfChanged /*= NULL*/, MOVEMENT_FLAG _eMovementFlag /*= MOVEMENTFLAG_FORWARD*/ )
{
	if (m_pNetAgent == NULL || pMoved == NULL )
	{
		return false;
	}

	// MUID
	MUID movedUid = pMoved->GetUID();
	UIID movedUIID = pMoved->GetUIID();

	// Position
	vec3 vMovedPos;
	if (pMovedPosIfChanged)
	{
		vMovedPos = *pMovedPosIfChanged;
	}
	else
	{
		vMovedPos = pMoved->GetPosition();
	}

	// Direction
	vec3 vMovedDir;
	if (pMovedDirIfChanged)
	{
		vMovedDir = *pMovedDirIfChanged;
	}
	else
	{
		vMovedDir = pMoved->GetDirection();

	}

	TD_PC_MOVE mi;
	mi.vTarPos	= vMovedPos;
	mi.svDir.SetX(vMovedDir.x);
	mi.svDir.SetY(vMovedDir.y);
	mi.nFlags	= _eMovementFlag;

	m_pNetAgent->OnRecv( MC_ACTION_MOVE, 2,

		NEW_USHORT( movedUIID ),
		NEW_SINGLE_BLOB(&mi, sizeof(TD_PC_MOVE))

	);

	return true;
}

bool TestModuleContext::MakeContextFieldLoadingCompleteIgnore()
{
	if (m_pNetAgent == NULL)
	{
		return false;
	}

	m_pNetAgent->IgnoreCmd(MC_FIELD_LOADING_COMPLETE);
	return true;
}

void TestModuleContext::TalentClear()
{
	map<int , CSTalentInfo* >::iterator itInfo = m_pTalentInfoMgr->begin();
	while(itInfo != m_pTalentInfoMgr->end())
	{
		SAFE_DELETE((*itInfo).second);
		itInfo++;
	}

	m_pTalentInfoMgr->clear();
}