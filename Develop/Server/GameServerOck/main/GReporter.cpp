#include "stdafx.h"
#include "GReporter.h"
#include "GField.h"
#include "GEntityActor.h"
#include "GGlobal.h"
#include "GFieldMgr.h"

bool GReporter::ReportTargetEntity( MUID uidField, MUID uidTarget )
{
	GField* pField = gmgr.pFieldMgr->GetField(uidField);
	if (!pField)	return false;

	GEntityActor* pTarget = pField->FindActor(uidTarget);
	if (!pTarget)		return false;

	if (pTarget->GetTypeID() == ETID_NPC)
	{
		return ReportNPC(uidField, uidTarget);
	}
	else if (pTarget->GetTypeID() == ETID_PLAYER)
	{
		return ReportPC(uidField, uidTarget);
	}
	return false;
}

bool GReporter::ReportNPC( MUID uidField, MUID uidNPC )
{
	return m_NPCReporter.Report(uidField, uidNPC);
}

bool GReporter::ReportPC( MUID uidField, MUID uidPlayer )
{
	return m_PlayerReporter.Report(uidField, uidPlayer);
}