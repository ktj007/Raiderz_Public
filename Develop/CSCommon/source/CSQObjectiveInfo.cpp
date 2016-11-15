#include "stdafx.h"
#include "CSQObjectiveInfo.h"
#include "CSDef_Quest.h"

CSQObjectiveInfo::CSQObjectiveInfo()
: nID(0)
, nQuestID(0)
, nType(QOT_NONE)
, nParam2(0)
, nFieldID(0)
, nMarkerID(0)
, nFieldID2(0)
, nMarkerID2(0)
, nPublicProgress(QPP_SELF)
{

}

CSQObjectiveInfo::~CSQObjectiveInfo()
{

}