#include "StdAfx.h"
#include "GUTHelper_Player.h"
#include "SUnitTestUtil.h"
#include "GPlayerObjectManager.h"
#include "MockPlayerObject.h"
#include "GPlayerFieldDynamic.h"
#include "GPlayerPVPArea.h"
#include "GPlayerSetterForTest.h"
#include "CSDef_QuestPVP.h"

GUTHelper_Player::GUTHelper_Player(void)
{
}

GUTHelper_Player::~GUTHelper_Player(void)
{
}

GEntityPlayer* GUTHelper_Player::NewPlayer( GField* pField/*=NULL*/, vec3 pos/*=vec3(0,0,0)*/, MUID uidNew/*=MUID(0,0)*/, MARKER_INFO markerExit/*=MARKER_INFO()*/ )
{
	if (uidNew.IsInvalid()) 
	{
		uidNew = SUnitTestUtil::NewUID();
	}

	GEntityPlayer* pNewEntityPlayer = new GEntityPlayer;
	pNewEntityPlayer->Create(uidNew);

	if (markerExit.nID != 0)
	{
		pNewEntityPlayer->GetPlayerField().GetFieldDynamic().SetEnterPos(markerExit);
	}

	pNewEntityPlayer->GetPlayerInfo()->nCID = SUnitTestUtil::NewCID();

	if (NULL != pField)
	{
		pNewEntityPlayer->EnterToField(pField, pos);
	}	
	pNewEntityPlayer->GetChrInfo()->nMaxHP = 10000;
	pNewEntityPlayer->GetChrInfo()->nMaxEN = 10000;
	pNewEntityPlayer->GetChrInfo()->nMaxSTA = 10000;
	pNewEntityPlayer->GetChrInfo()->fSpeed = 100;	
	pNewEntityPlayer->SetHP(pNewEntityPlayer->GetMaxHP());
	pNewEntityPlayer->SetEN(pNewEntityPlayer->GetMaxEN());
	pNewEntityPlayer->SetSTA(pNewEntityPlayer->GetMaxSTA());
	pNewEntityPlayer->SetDir(vec3(0,1,0));

	// 능력치에 의한 보너스/패널티 0
	pNewEntityPlayer->GetPlayerInfo()->nSTR = 100;
	pNewEntityPlayer->GetPlayerInfo()->nDEX = 100;
	pNewEntityPlayer->GetPlayerInfo()->nINT = 100;
	pNewEntityPlayer->GetPlayerInfo()->nCON = 100;
	pNewEntityPlayer->GetPlayerInfo()->nCHA = 100;

	wstring strName = SUnitTestUtil::NewName();
	wcsncpy_s(pNewEntityPlayer->GetPlayerInfo()->szName, strName.c_str(), strName.length());

	pNewEntityPlayer->SetRegenActive(false);
	pNewEntityPlayer->SetTimeSynchorActive(false);

	MUID uidPlayer = pNewEntityPlayer->GetUID();
	MockPlayerObject* pMockPlayerObject = new MockPlayerObject(uidPlayer);
	gmgr.pPlayerObjectManager->AddPlayer(pMockPlayerObject);
	pMockPlayerObject->Create(pNewEntityPlayer);
	gmgr.pPlayerObjectManager->AddCID(pMockPlayerObject);	

	pMockPlayerObject->GetAccountInfo().nAID = SUnitTestUtil::NewUINT32ID();

	return pNewEntityPlayer;
}

class MockPlayerPVPArea_Locate : public GPlayerPVPArea
{
public:
	MockPlayerPVPArea_Locate(GEntityPlayer* pOwner)
		: GPlayerPVPArea(pOwner)	{}
	MOCK_METHOD0(IsLocatedPvPArea, bool());
};

class MockPlayerPVPArea_Team : public GPlayerPVPArea
{
public:
	MockPlayerPVPArea_Team(GEntityPlayer* pOwner)
	: GPlayerPVPArea(pOwner)	{}
	MOCK_METHOD0(IsLocatedPvPArea, bool());
	MOCK_METHOD0(GetTeam, QUESTPVP_TEAM());
};

void GUTHelper_Player::SetMockPlayerPVPArea_Locate( GEntityPlayer* pPlayer )
{
	MockPlayerPVPArea_Locate* pMockPlayerPVPArea = new MockPlayerPVPArea_Locate(pPlayer);
	pPlayer->GetPlayerSetterForTest().SetPlayerPVPArea(pMockPlayerPVPArea);

	EXPECT_CALL(*pMockPlayerPVPArea, IsLocatedPvPArea())
		.WillRepeatedly(testing::Return(true));	
}

void GUTHelper_Player::SetMockPlayerPVPArea_LocateAndTeamOne( GEntityPlayer* pPlayer )
{
	MockPlayerPVPArea_Team* pMockPlayerPVPArea = new MockPlayerPVPArea_Team(pPlayer);
	pPlayer->GetPlayerSetterForTest().SetPlayerPVPArea(pMockPlayerPVPArea);

	EXPECT_CALL(*pMockPlayerPVPArea, IsLocatedPvPArea())
		.WillRepeatedly(testing::Return(true));	

	EXPECT_CALL(*pMockPlayerPVPArea, GetTeam())
		.WillRepeatedly(testing::Return(QT_ONE));
}

void GUTHelper_Player::SetMockPlayerPVPArea_LocateAndTeamTwo( GEntityPlayer* pPlayer )
{
	MockPlayerPVPArea_Team* pMockPlayerPVPArea = new MockPlayerPVPArea_Team(pPlayer);
	pPlayer->GetPlayerSetterForTest().SetPlayerPVPArea(pMockPlayerPVPArea);

	EXPECT_CALL(*pMockPlayerPVPArea, IsLocatedPvPArea())
		.WillRepeatedly(testing::Return(true));	

	EXPECT_CALL(*pMockPlayerPVPArea, GetTeam())
		.WillRepeatedly(testing::Return(QT_TWO));
}

void GUTHelper_Player::ResetMockPlayerPVPArea( GEntityPlayer* pPlayer )
{
	pPlayer->GetPlayerSetterForTest().SetPlayerPVPArea(new GPlayerPVPArea(pPlayer));
}