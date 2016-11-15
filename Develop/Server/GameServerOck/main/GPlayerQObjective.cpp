#include "stdafx.h"
#include "GPlayerQObjective.h"
#include "GQuestInfo.h"
#include "GQObjectiveInfo.h"
#include "CSDef_Quest.h"

GPlayerQObjective::GPlayerQObjective(GQObjectiveInfo* pQObjectiveInfo)
: m_pQObjectiveInfo(pQObjectiveInfo)
, m_nProgress(0)
, m_bComplete(false)
{

}

GQObjectiveInfo* GPlayerQObjective::GetInfo()
{
	return m_pQObjectiveInfo;
}

int GPlayerQObjective::GetProgress()
{
	return m_nProgress;
}

void GPlayerQObjective::UpdateProgress( int nProgress )
{
	m_nProgress = nProgress;
}

bool GPlayerQObjective::IsComplete()
{
	switch (m_pQObjectiveInfo->nType)
	{
	case QOT_INTERACT:
	case QOT_DESTROY:
	case QOT_COLLECT:	return m_nProgress >= m_pQObjectiveInfo->nParam2;
	case QOT_ACT:
	case QOT_SCOUT:
	case QOT_ESCORT:	return m_nProgress == 1;
	}

	return false;
}