#include "StdAfx.h"
#include "GTestForward.h"
#include "GJob_Patrol.h"
#include "GJob_Broker.h"

SUITE(JobPatrol)
{
	struct Fixture
	{
		Fixture()
		{ 
			m_pField = GUTHelper_Field::DefaultMockField();

			m_pFieldInfo = m_pField->GetInfo();
			{
				MARKER_INFO Marker;
				Marker.nFieldID = 1;
				for (int i=0; i<4; i++)
				{
					Marker.nID = i;	
					Marker.vPoint = vec3(100+(float)i, 100+(float)i, 100+(float)i);	
					const_cast<GFieldInfo*>(m_pFieldInfo)->InsertMarkerInfo(Marker);
				}
			}
			
			m_pNPC = GUTHelper_NPC::SpawnNPC(m_pField);
		}

		~Fixture() 
		{
			m_pField->Destroy();
		}

		GUTHelper		m_Helper;
		MockField*		m_pField;
		const GFieldInfo*		m_pFieldInfo;
		GEntityNPC*		m_pNPC;

		GTestSysWrapper<GScriptManager>				m_ScriptMgr;
		GTestMgrWrapper<GFieldInfoMgr>				m_FieldInfoMgr;
		GTestMgrWrapper<GNPCInfoMgr>				m_NPCInfoMgr;
	};

	TEST_FIXTURE(Fixture, Patrol_Once)
	{
		MARKER_LIST marker_list;
		marker_list.push_back(1);
		marker_list.push_back(2);
		marker_list.push_back(3);
		GJob_Patrol* pJob = dynamic_cast<GJob_Patrol*>(GJobBroker::NewJob_Patrol(m_pNPC->GetModuleAI(), marker_list, PT_ONCE));
		TVALID(pJob);
		
		MARKER_INFO marker;
		pJob->Start();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(1, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(2, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(3, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(3, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(3, marker.nID);

		SAFE_DELETE(pJob);
	}

	TEST_FIXTURE(Fixture, Patrol_OnceReturn)
	{
		MARKER_LIST marker_list;
		marker_list.push_back(1);
		marker_list.push_back(2);
		marker_list.push_back(3);
		GJob_Patrol* pJob = dynamic_cast<GJob_Patrol*>(GJobBroker::NewJob_Patrol(m_pNPC->GetModuleAI(), marker_list, PT_ONCE_RETURN));
		TVALID(pJob);

		MARKER_INFO marker;
		pJob->Start();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(1, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(2, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(3, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(1, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(1, marker.nID);

		SAFE_DELETE(pJob);
	}

	TEST_FIXTURE(Fixture, Patrol_Loop)
	{
		MARKER_LIST marker_list;
		marker_list.push_back(1);
		marker_list.push_back(2);
		marker_list.push_back(3);
		GJob_Patrol* pJob = dynamic_cast<GJob_Patrol*>(GJobBroker::NewJob_Patrol(m_pNPC->GetModuleAI(), marker_list, PT_LOOP));
		TVALID(pJob);

		MARKER_INFO marker;
		pJob->Start();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(1, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(2, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(3, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(1, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(2, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(3, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(1, marker.nID);

		SAFE_DELETE(pJob);
	}

	TEST_FIXTURE(Fixture, Patrol_BackOrder)
	{
		MARKER_LIST marker_list;
		marker_list.push_back(1);
		marker_list.push_back(2);
		marker_list.push_back(3);
		GJob_Patrol* pJob = dynamic_cast<GJob_Patrol*>(GJobBroker::NewJob_Patrol(m_pNPC->GetModuleAI(), marker_list, PT_LOOP_BACKORDER));
		TVALID(pJob);

		MARKER_INFO marker;
		pJob->Start();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(1, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(2, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(3, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(2, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(1, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(2, marker.nID);
		pJob->RunAdvance();
		pJob->GetCurrentPatrolMarker(marker);
		CHECK_EQUAL(3, marker.nID);

		SAFE_DELETE(pJob);
	}
}