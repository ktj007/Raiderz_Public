#include "StdAfx.h"
#include "GReserveCmd.h"
#include "AStlUtil.h"
#include "GGlobal.h"
#include "GSystem.h"

GReserveCmd::GReserveCmd(MCommand* pCmd)
: m_nReserveTime(0)
{
	Insert(pCmd);
}

GReserveCmd::~GReserveCmd(void)
{
	Clear();
}

void GReserveCmd::Clear()
{
	SAFE_DELETE_VECTOR(m_vecCmd);
	m_nReserveTime = 0;
}

void GReserveCmd::Insert(MCommand* pCmd)
{
	if (true == m_vecCmd.empty())
	{
		m_nReserveTime = gsys.pSystem->GetNowTime();
	}

	m_vecCmd.push_back(pCmd);
}

void GReserveCmd::Run()
{
	for each (MCommand* pCmd in m_vecCmd)
	{
		if (NULL == pCmd) continue;

		pCmd->Run();
	}
}

bool GReserveCmd::IsTimeOver(uint32 nNowTime)
{
	bool bTimeOver = (nNowTime - m_nReserveTime >= TIMEOUT_TICK_MOVE_SERVER);

	return bTimeOver;
}