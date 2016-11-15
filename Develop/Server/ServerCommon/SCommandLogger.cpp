#include "stdafx.h"
#include "SCommandLogger.h"

void SCommandLogger::IgnoreCommand(int nID, bool bIgnore)
{
	if (bIgnore == true)
	{
		m_IgnoredCmdSet.insert(nID);
	}
	else
	{
		m_IgnoredCmdSet.erase(nID);
	}
}

void SCommandLogger::IgnoreCommandAll(bool bIgnore, int nMaxIgnoreID/*=MAX_IGNORE_COMMAND_ID*/)
{
	if (bIgnore == true)
	{
		for (int i = 0; i < nMaxIgnoreID; i++)
		{
			m_IgnoredCmdSet.insert(i);
		}
	}
	else
	{
		m_IgnoredCmdSet.clear();
	}
}

void SCommandLogger::Log(minet::MCommand* pCmd)
{
#ifndef _DEBUG
	return;
#endif

	if (m_IgnoredCmdSet.find(pCmd->GetID()) != m_IgnoredCmdSet.end()) return;
}