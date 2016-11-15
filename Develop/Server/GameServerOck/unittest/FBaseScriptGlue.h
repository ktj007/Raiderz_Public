#pragma once

#include "FBaseMockLink.h"
#include "GUTHelper.h"
#include "GTestWrapper.h"
#include "GFieldMgr.h"

class TestFieldMgr : public GFieldMgr
{
public:
	virtual MUID	NewUID()
	{
		return SUnitTestUtil::NewUID();
	}
};

class FBaseScriptGlue: public FBaseMockLink
{
public:
	FBaseScriptGlue();
	virtual ~FBaseScriptGlue();

	GTalentInfo* MakeSimpleDmgAtkTalent(const int nDamage, int nTalentID=INVALID_TALENT_ID);

protected:
	GUTHelper		m_Helper;
	MockField*		m_pField;
	GFieldInfo*		m_pFieldInfo;
	GNPCInfo*		m_pNPCInfo;
	SENSOR_INFO		m_SensorInfo;

	GTestSysWrapper<GScriptManager>				m_ScriptMgr;
	GTestMgrWrapper2<GFieldMgr, TestFieldMgr>	m_FieldMgr;
	GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
	GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;
};
