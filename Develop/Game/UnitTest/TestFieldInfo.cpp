#include "stdafx.h"
#include "XFieldInfo.h"

SUITE(FieldInfo)
{
	class FFieldInfo
	{
	public:
		FFieldInfo()
		{

		}
		~FFieldInfo()
		{

		}
		void AddPvpArea(MRect rcArea)
		{
			PVP_AREA_INFO areaInfo;
			areaInfo.rcArea = rcArea;

			m_FieldInfo.m_PvPAreas.push_back(areaInfo);
		}

		XFieldInfo m_FieldInfo;
	};
	TEST_FIXTURE(FFieldInfo, FieldInfo_Cook_PvPAreas)
	{
		m_FieldInfo.m_fMinX = 0.0f;
		m_FieldInfo.m_fMinY = 0.0f;
		m_FieldInfo.m_fMaxX = 10000.0f;
		m_FieldInfo.m_fMaxY = 10000.0f;

		// #define FIELD_ATTR_CELL_SIZE	300 // cm

		AddPvpArea(MRect(250.0f, 350.0f, 3001.0f, 3001.0f));

		m_FieldInfo.Cook();

		CHECK_EQUAL(0.0f, m_FieldInfo.m_PvPAreas[0].rcArea.left);
		CHECK_EQUAL(300.0f, m_FieldInfo.m_PvPAreas[0].rcArea.top);
		CHECK_EQUAL(3300.0f, m_FieldInfo.m_PvPAreas[0].rcArea.right);
		CHECK_EQUAL(3300.0f, m_FieldInfo.m_PvPAreas[0].rcArea.bottom);
	}
}