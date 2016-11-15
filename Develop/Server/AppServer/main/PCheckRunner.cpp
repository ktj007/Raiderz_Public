#include "stdafx.h"
#include "PCheckRunner.h"
#include "PLogicChecker.h"

PCheckRunner::PCheckRunner()
{
	// do nothing
}

PCheckRunner::~PCheckRunner()
{
	Clear();
}

void PCheckRunner::AddChecker(PLogicChecker* pChecker)
{
	m_vecChecker.push_back(pChecker);
}

bool PCheckRunner::Run(void)
{
	bool bResult = true;

	for each(PLogicChecker* pChecker in m_vecChecker)
	{
		if (pChecker->Check())
		{
			pChecker->OnSuccess();
		}
		else
		{
			pChecker->OnFail();			
			bResult = false;
			break;
		}
	}

	Clear();
	return bResult;
}

void PCheckRunner::Clear(void)
{
	for each(PLogicChecker* pChecker in m_vecChecker)
	{
		SAFE_DELETE(pChecker);
	}
	m_vecChecker.clear();
}
