#pragma once

#include "TestGame.h"
#include "MMockNetClient.h"
#include "XTestWrapper.h"
#include "XNPCInfo.h"
#include "XTalentInfo.h"
#include "XUnitTestHelper.h"
#include "MockNPCInfo.h"
#include "MockTalentInfo.h"
#include "MockItemInfo.h"
#include "XMotion.h"
#include "XFactionInfoMgr.h"
#include "XFieldInfo.h"

class TestModuleContext;
class XNetPlayer;

class FTestGame
{
protected:
	TestGame	m_TestGame;
	minet::MTestNetAgent*	m_pNetAgent;
	TestModuleContext*		m_pTestContext;
	XGame*					m_pGame;
	XUnitTestHelper			m_Helper;
	TestAniDataSet			m_NPCAniDataSet;
	TestAniDataSet			m_PCAniDataSet;

	XTestInfoWrapper2<XNPCInfoMgr, MockNPCInfoMgr>		m_NPCInfoWrapper;
	XTestInfoWrapper2<XTalentInfoMgr, MockTalentInfoMgr>	m_TalentInfoWrapper;
	XTestInfoWrapper2<XItemManager, MockItemInfoMgr>		m_ItemWrapper;
	XTestInfoWrapper<XFactionInfoMgr>						m_FactionInfoWrapper;
	XTestInfoWrapper<XMotionMgr>	m_MotionInfoWrapper;
	XTestInfoWrapper<XFieldInfoMgr>	m_FieldInfoWrapper;
public:
	FTestGame();
	virtual ~FTestGame();

	virtual void Update(float fDelta);

	XMyPlayer* GetMyPlayer();
	XNetPlayer* NewNetPlayer(vec3 vPos=vec3::ZERO, vec3 vDir=vec3(0.0f, 1.0f, 0.0f), SEX nSex=SEX_FEMALE);
	XNonPlayer* NewNonPlayer(int nNPCID, vec3 vPos=vec3::ZERO, vec3 vDir=vec3(0.0f, 1.0f, 0.0f));

	XNPCInfo* NewNPCInfo(int nNPCID = 1, const wchar_t* szName = L"SampleNPC", int nGrade = 3, const wchar_t* szMeshName = L"hm_adult", const wchar_t* szMeshPath = L"NPC/hm_adult");
};