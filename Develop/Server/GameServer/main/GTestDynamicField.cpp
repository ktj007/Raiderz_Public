#include "StdAfx.h"
#include "GTestDynamicField.h"
#include "GEntityPlayer.h"
#include "GDynamicFieldFactory.h"
#include "AStlUtil.h"
#include "GFieldMgr.h"
#include "GGlobal.h"
#include "GDynamicFieldMaster.h"
#include "GServer.h"
#include "GConst.h"

GTestDynamicField::GTestDynamicField(void)
: m_bInit(false)
, m_nCurSensorID(1)
, m_nCurFieldQty(0)
{
	m_rgrSensorTick.SetTickTime(GConst::DFTEST_SENSOR_ENTER_INTERVAL);
	m_rgrFieldTick.SetTickTime(0.1f);
}

GTestDynamicField::~GTestDynamicField(void)
{
	Fini();
}

GTestDynamicField& GTestDynamicField::Instance()
{
	static GTestDynamicField inst;
	return inst;
}

void GTestDynamicField::Init()
{
	if (!GConst::DFTEST_ENABLE)
		return;

	if (m_bInit)
		return;

	m_rgrSensorTick.Start();
	m_bInit = true;
}

void GTestDynamicField::Fini()
{
	if (!GConst::DFTEST_ENABLE)
		return;

	if (!m_bInit)
		return;

	m_bInit = false;

	SAFE_DELETE_VECTOR(m_vecPlayers);
}

void GTestDynamicField::Update(float fDelta)
{
	if (!GConst::DFTEST_ENABLE)
		return;

	if (!m_bInit)
	{
		Init();
		return;
	}

	if (m_rgrSensorTick.IsReady(fDelta))
	{
		for each (GEntityPlayer* each in m_vecPlayers)
		{
			each->EnterSensor(m_nCurSensorID++);
		}
	}

	if (m_rgrFieldTick.IsReady(fDelta))
	{
		mlog("Create Field (%d/%d)\n", m_nCurFieldQty, GConst::DFTEST_FIELDGROUP_QTY);
		for (int i=0; i<20; ++i)
		{
			CreateDF();
			++m_nCurFieldQty;

			if (GConst::DFTEST_FIELDGROUP_QTY <= m_nCurFieldQty)
			{
				m_rgrFieldTick.Stop();
				break;
			}
		}
	}
}

void GTestDynamicField::CreateDF()
{
	GDynamicFieldFactory factory;
	GEntityPlayer* pPlayer = new GEntityPlayer;
	pPlayer->Create(gsys.pServer->NewUID());
	m_vecPlayers.push_back(pPlayer);
	GDynamicFieldMaster* pMaster = factory.CreateDynamicField(DFT_TRIAL, pPlayer, GConst::DFTEST_FIELDGROUP_ID);
	VALID(pMaster);
	GDynamicField* pField = pMaster->GetFieldContainer().begin()->second;
	VALID(pField);
	
	// 센서 영역 확장
	const SENSOR_INFO_MAP_BY_ID& mapSensors = pField->GetInfo()->GetSensorInfoContainer();
	for (SENSOR_INFO_MAP_BY_ID::const_iterator it = mapSensors.begin();
		 it != mapSensors.end();
		 ++it)
	{
		const SENSOR_INFO* pSensor = &it->second;
		const_cast<SENSOR_INFO*>(pSensor)->fRadius = 9999999.9f;
	}

	pPlayer->EnterToField(pField, vec3::ZERO, vec3::ZERO);
}