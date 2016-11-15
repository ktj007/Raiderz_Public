#include "stdafx.h"
#include "MToolTipManager.h"

using namespace mint3;

SUITE(ToolTipManager)
{
	class TestToolTipManager : public MToolTipManager
	{
	public:
		using MToolTipManager::CalcToolTipPosition;
	};

	class FToolTipManager
	{
	public:
		FToolTipManager()
		{
			m_ptCursor = MPOINT(0, 0);
			m_ptOffset = MPOINT(0, 0);
			m_WorkspaceSize = MSIZE(100, 100);
			m_ToolTipSize = MSIZE(10, 10);
		}
		
		TestToolTipManager	m_ToolTipManager;

		MPOINT	m_ptCursor;
		MPOINT	m_ptOffset;
		MSIZE	m_WorkspaceSize;
		MSIZE	m_ToolTipSize;
	};

	TEST_FIXTURE(FToolTipManager, TestCalcToolTipPosition_CursorPos)
	{
		m_ptCursor = MPOINT(0, 0);
		MPOINT ptPos = m_ToolTipManager.CalcToolTipPosition(m_ptCursor, m_ptOffset, m_WorkspaceSize, m_ToolTipSize);
		CHECK(MPOINT(0, 0) == ptPos);

		m_ptCursor = MPOINT(50, 50);
		ptPos = m_ToolTipManager.CalcToolTipPosition(m_ptCursor, m_ptOffset, m_WorkspaceSize, m_ToolTipSize);
		CHECK(MPOINT(50, 50) == ptPos);
	}

	TEST_FIXTURE(FToolTipManager, TestCalcToolTipPosition_Offset)
	{
		m_ptCursor = MPOINT(0, 0);
		m_ptOffset = MPOINT(15, 20);

		MPOINT ptPos = m_ToolTipManager.CalcToolTipPosition(m_ptCursor, m_ptOffset, m_WorkspaceSize, m_ToolTipSize);
		CHECK(MPOINT(0 + 15, 0 + 20) == ptPos);

		m_ptCursor = MPOINT(50, 50);
		ptPos = m_ToolTipManager.CalcToolTipPosition(m_ptCursor, m_ptOffset, m_WorkspaceSize, m_ToolTipSize);
		CHECK(MPOINT(50 + 15, 50 + 20) == ptPos);
	}

	TEST_FIXTURE(FToolTipManager, TestCalcToolTipPosition_Right)
	{
		m_ptCursor = MPOINT(90, 0);
		m_ptOffset = MPOINT(15, 20);
		m_ToolTipSize = MSIZE(20, 10);

		MPOINT ptPos = m_ToolTipManager.CalcToolTipPosition(m_ptCursor, m_ptOffset, m_WorkspaceSize, m_ToolTipSize);
		CHECK(MPOINT(90 - 15 - 20, 0 + 20) == ptPos);
	}

	TEST_FIXTURE(FToolTipManager, TestCalcToolTipPosition_Bottom)
	{
		m_ptCursor = MPOINT(0, 90);
		m_ptOffset = MPOINT(15, 20);
		m_ToolTipSize = MSIZE(20, 20);

		MPOINT ptPos = m_ToolTipManager.CalcToolTipPosition(m_ptCursor, m_ptOffset, m_WorkspaceSize, m_ToolTipSize);
		CHECK(MPOINT(15, 90 - 20 - 20) == ptPos);
	}

	TEST_FIXTURE(FToolTipManager, TestCalcToolTipPosition_Boundary)
	{
		m_ptCursor = MPOINT(-50, -100);
		m_ptOffset = MPOINT(5, 10);

		MPOINT ptPos = m_ToolTipManager.CalcToolTipPosition(m_ptCursor, m_ptOffset, m_WorkspaceSize, m_ToolTipSize);
		CHECK(MPOINT(0, 0) == ptPos);



		m_ptCursor = MPOINT(500, 500);
		m_ptOffset = MPOINT(5, 10);

		ptPos = m_ToolTipManager.CalcToolTipPosition(m_ptCursor, m_ptOffset, m_WorkspaceSize, m_ToolTipSize);
		CHECK(MPOINT(500 - 10 - 5, 500 - 10 - 10) == ptPos);

	}

}