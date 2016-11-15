#pragma once

#include "XUnitTestHelper.h"
#include "MockAnimationController.h"
#include "XTestWrapper.h"
#include "XNPCInfo.h"
#include "XTalentInfo.h"
#include "MockNPCInfo.h"
#include "MockTalentInfo.h"
#include "MockNetClient.h"
#include "FBaseNetClient.h"
#include "FBaseMotion.h"

class TestGame;

struct SimpleTestFixture : public FBaseNetClient, public FBaseMotion
{
	SimpleTestFixture();
	virtual ~SimpleTestFixture();

	XUnitTestHelper		m_Helper;
	TestAniDataSet		m_NPCAniDataSet;
	TestAniDataSet		m_PCAniDataSet;
	XNPCInfo*			m_pNPCInfo;

	TestGame*				m_pGame;

	XTestInfoWrapper2<XNPCInfoMgr, MockNPCInfoMgr>		m_NPCInfoWrapper;
	XTestInfoWrapper2<XTalentInfoMgr, MockTalentInfoMgr>	m_TalentInfoWrapper;
};