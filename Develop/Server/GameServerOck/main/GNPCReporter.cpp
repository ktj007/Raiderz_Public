#include "stdafx.h"
#include "GNPCReporter.h"
#include "GServer.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GField.h"
#include "GConst.h"
#include "GModuleAI.h"
#include "GNPCDebugger.h"
#include "GFieldMgr.h"
#include "GHateTable.h"
#include "GNPCMove.h"

GNPCReporter::GNPCReporter()
{

}

GNPCReporter::~GNPCReporter()
{

}

wstring GNPCReporter::GetHateTableString(GEntityNPC* pNPC)
{
	wstring strText;

	GHateTable& hate_table = pNPC->GetHateTable();
	if (hate_table.IsEmpty())	
		return L"         <HateTable is empty>";

	wchar_t buff[512];
	swprintf_s(buff, L"%28s | %s\n", L"< Name(UID) >",L"< Score >");	strText += buff;

	int nIndex = 0;
	vector<pair<MUID, HatePoint> > vecHateTables;
	pNPC->GetHateTable().GetHighestTargets(pNPC->GetHateTable().GetQty(), vecHateTables);

	for each (pair<MUID, HatePoint> each in vecHateTables)
	{
		const MUID		uid		= each.first;
		const HatePoint	score	= each.second;
		wstring strName = L"\"InvalidTarget\"";

		GEntityActor* pTarActor = pNPC->FindActor(uid);
		if (pTarActor)
		{
			if (pTarActor->IsNPC())
			{
				GNPCInfo* pNPCInfo = static_cast<GEntityNPC*>(pTarActor)->GetNPCInfo();
				if (pNPCInfo)
					strName = pNPCInfo->strName;
			}
			else if (pTarActor->IsPlayer())
			{
				PLAYER_INFO* pPCInfo = static_cast<GEntityPlayer*>(pTarActor)->GetPlayerInfo();
				if (pPCInfo)
					strName = pPCInfo->szName;
			}
		}

		swprintf_s(buff, L"[%3d] %16s(%4I64d) | %4.1f\n", nIndex+1, strName.c_str(), uid, score); strText += buff;
		nIndex++;
	}

	return strText;
}


bool GNPCReporter::Report( MUID uidField, MUID uidNPC )
{
	GField* pField = gmgr.pFieldMgr->GetField(uidField);
	if (!pField)	return false;

	GEntityNPC* pNPC = pField->FindNPC(uidNPC);
	if (!pNPC)		return false;

	GNPCInfo* pInfo = pNPC->GetNPCInfo();
	if (!pInfo)		return false;

	GNPCInfo* pNPCInfo = pNPC->GetNPCInfo();
	if (!pNPCInfo)		return false;

	GNPCDebugger* pNPCDebugger = pNPC->GetDebugger();

	const NPC_STATUS& NPCStatus = pNPC->GetNPCStatus();
	

	FILE *file=NULL;
	_wfopen_s(&file, GConst::REPORT_NPC_FILENAME.c_str(), L"w+");
	if (!file)		
	{
		mlog("GNPCReporter::Report() failed. dont create file (name: %s)\n", MLocale::ConvUTF16ToAnsi(GConst::REPORT_NPC_FILENAME.c_str()).c_str());
		return false;
	}

	
#define SEERATE_STRING L"--------------------------------------------------------------------\n"

	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%16s\n", L"[ProtoInfo]");
	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%16s | %s\n", L"Name", pNPCInfo->strName.c_str());
	fwprintf(file, L"%16s | %s\n", L"LuaName", pNPCInfo->strNameInLua.c_str());
	fwprintf(file, L"%16s | %d\n", L"ID", pNPCInfo->nID);
	fwprintf(file, L"%16s | %d\n", L"Level", pNPCInfo->nLevel);
	fwprintf(file, L"%16s | %.1f / %.1f\n", L"Speed / RunSpeed", pNPCInfo->fSpeed, pNPCInfo->fRunSpeed);
	fputws(SEERATE_STRING, file);
// --------------------------------------------------------------------

	fwprintf(file, L"%16s\n", L"[Field]");
	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%16s | %I64d\n", L"FieldUID", pNPC->GetFieldUID());
	if (pField->GetInfo())
	{
		fwprintf(file, L"%16s | %d\n", L"FieldID", pField->GetInfo()->m_nFieldID);
		fwprintf(file, L"%16s | %s\n", L"IsDynamicField", (pField->GetInfo()->m_bDynamic)?L"true":L"false");
	}
// --------------------------------------------------------------------

	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%16s\n", L"[Status]");
	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%16s | %I64d\n", L"EntityUID", pNPC->GetUID());
	fwprintf(file, L"%16s | %4d / %4d\n", L"HP", pNPC->GetHP(), pNPC->GetMaxHP());
	fwprintf(file, L"%16s | %4d / %4d\n", L"EN", pNPC->GetEN(), pNPC->GetMaxEN());
	fwprintf(file, L"%16s | %4d / %4d\n", L"STA", pNPC->GetSTA(), pNPC->GetMaxSTA());
	fwprintf(file, L"%16s | %s\n", L"IsGuarding", (pNPC->IsGuarding())?L"true":L"false");
	fwprintf(file, L"%16s | %s\n", L"IsRetreat", (pNPC->GetNPCMove().IsRetreat())?L"true":L"false");
// --------------------------------------------------------------------

	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%16s\n", L"[Movement]");
	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%16s | %8.1f / %8.1f / %8.1f\n", L"Position", pNPC->GetPos().x, pNPC->GetPos().y, pNPC->GetPos().z);
	fwprintf(file, L"%16s | %8.1f / %8.1f / %8.1f\n", L"Direction", pNPC->GetDir().x, pNPC->GetDir().y, pNPC->GetDir().z);
// --------------------------------------------------------------------

	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%16s\n", L"[Path Finding]");
	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%s\n", pNPCDebugger->GetPathFindingString().c_str());
// --------------------------------------------------------------------

	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%16s\n", L"[A.I]");
	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%16s | %s\n", L"Job", pNPCDebugger->GetCurrentJobName().c_str());
	fwprintf(file, L"%16s | %s\n", L"Task", pNPCDebugger->GetCurrentTaskListName().c_str());
	fwprintf(file, L"%16s | %s\n", L"Motionfactor", pNPCDebugger->GetCurrentMotionfactorName().c_str());
	fwprintf(file, L"%16s | %s\n", L"FSM", pNPCDebugger->GetCurrentFSM().c_str());
	fputws(SEERATE_STRING, file);
// --------------------------------------------------------------------

	fwprintf(file, L"%16s\n", L"[HateTable]");
	fputws(SEERATE_STRING, file);
	fwprintf(file, L"%s\n", GetHateTableString(pNPC).c_str());
// --------------------------------------------------------------------

	fputws(L"\nGenerated by NPC_Reporter\n", file);
	fputws(L"¨Ï 2007, MAIET entertainment, Inc. all rights reserved.\n", file);

	fclose(file);

	return true;
}
