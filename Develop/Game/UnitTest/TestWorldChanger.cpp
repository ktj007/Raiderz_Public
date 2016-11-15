#include "stdafx.h"
#include "XWorldChanger.h"
#include "XFieldInfo.h"

SUITE(WorldChanger)
{
	class TestWorldChanger : public XWorldChanger
	{
	protected:
		virtual void SetupNextFieldInfo(int nNewFieldID) {}
	public:
		using XWorldChanger::m_pNextFieldInfo;
		using XWorldChanger::m_nLastFieldZoneID;
		using XWorldChanger::IsSameZone;
	};

	class FWorldChanger
	{
	public:
		FWorldChanger()
		{
			m_LastFieldInfo.m_nFieldID = 1;
			m_NextFieldInfo.m_nFieldID = 2;

			m_LastFieldInfo.m_nZoneID = 1;
			m_NextFieldInfo.m_nZoneID = 1;
		}
		~FWorldChanger()
		{
			m_WorldChanger.m_pNextFieldInfo = NULL;
			m_WorldChanger.m_nLastFieldZoneID = -1;
		}
		TestWorldChanger	m_WorldChanger;

		XFieldInfo m_LastFieldInfo;
		XFieldInfo m_NextFieldInfo;
	};

	TEST_FIXTURE(FWorldChanger, TestWorldChanger_IsSameZone)
	{
		m_LastFieldInfo.m_nZoneID = 1;
		m_NextFieldInfo.m_nZoneID = 1;

		m_WorldChanger.m_nLastFieldZoneID = m_LastFieldInfo.m_nZoneID;
		m_WorldChanger.m_pNextFieldInfo = &m_NextFieldInfo;

		CHECK_EQUAL(true, m_WorldChanger.IsSameZone());
	}

	TEST_FIXTURE(FWorldChanger, TestWorldChanger_IsSameZone_Negative)
	{

		// 둘 다 NULL
		CHECK_EQUAL(false, m_WorldChanger.IsSameZone());

		// 한쪽만 NULL
		m_WorldChanger.m_nLastFieldZoneID = m_LastFieldInfo.m_nZoneID;
		m_WorldChanger.m_pNextFieldInfo = NULL;
		CHECK_EQUAL(false, m_WorldChanger.IsSameZone());

		m_WorldChanger.m_nLastFieldZoneID = -1;
		m_WorldChanger.m_pNextFieldInfo = &m_NextFieldInfo;
		CHECK_EQUAL(false, m_WorldChanger.IsSameZone());
	}

	TEST_FIXTURE(FWorldChanger, TestWorldChanger_)
	{


	}

}