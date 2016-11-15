#include "stdafx.h"
#include "GTestForward.h"
#include "GPickingZTable.h"
#include "GFieldInfo.h"
//
//SUITE(PickingZTable)
//{
//	struct Fixture
//	{
//		Fixture()
//		{
//			field_info.m_nZoneID = 111;
//			field_info.m_fMinX = -10000;
//			field_info.m_fMaxX = 10000;
//			field_info.m_fMinY = -10000;
//			field_info.m_fMaxY = 10000;
//
//			picking_table = new GPickingZTable(&field_info);
//		}
//		~Fixture()
//		{
//			SAFE_DELETE(picking_table);
//		}
//
//		GFieldInfo field_info;
//		GPickingZTable* picking_table;
//	};
//
//	TEST_FIXTURE(Fixture, Init)
//	{
//		CHECK_EQUAL(20000/PICKING_CELL_SIZE+1,	picking_table->m_nSizeX);
//		CHECK_EQUAL(20000/PICKING_CELL_SIZE+1,	picking_table->m_nSizeY);
//		CHECK_EQUAL(picking_table->m_nSizeX,	picking_table->m_PickingTable.size());
//		CHECK_EQUAL(picking_table->m_nSizeY,	picking_table->m_PickingTable.front().size());
//		CHECK_EQUAL(picking_table->m_nSizeY,	picking_table->m_PickingTable.front().size());
//	}
//
//	TEST_FIXTURE(Fixture, ConvertToIndex)
//	{
//		MPoint pt;
//		pt = picking_table->ConvertToIndex(-10000, -10000);
//		CHECK_EQUAL(pt.x, 0);	
//		CHECK_EQUAL(pt.y, 0);
//		pt = picking_table->ConvertToIndex(0, 0);
//		CHECK_EQUAL(pt.x, 250);	
//		CHECK_EQUAL(pt.y, 250);
//		pt = picking_table->ConvertToIndex(10000, 10000);
//		CHECK_EQUAL(pt.x, 500);	
//		CHECK_EQUAL(pt.y, 500);
//	}
//
//	TEST_FIXTURE(Fixture, Picking)
//	{
//		CHECK_EQUAL(INVALID_PICKING_HEIGHT, picking_table->Pick(-10000.0f, -10000.0f, 0.0f));
//		CHECK_EQUAL(INVALID_PICKING_HEIGHT, picking_table->Pick(0.0f, 0.0f, 0.0f));
//		CHECK_EQUAL(INVALID_PICKING_HEIGHT, picking_table->Pick(10000.0f, 10000.0f, 0.0f));
//	}
//}
