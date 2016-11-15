#include "stdafx.h"
#include "GReloader.h"
#include "GCommandTable.h"
#include "GCommandCenter.h"
#include "GNPCInfo.h"
#include "GNPCInfoMgr.h"
#include "GNPCMgr.h"
#include "GEntityNPC.h"
#include "GModuleAI.h"
#include "GSpawnManager.h"
#include "GBuffInfo.h"
#include "GModuleCombat.h"
#include "GTalentInfoMgr.h"
#include "GServer.h"
#include "GFieldMgr.h"
#include "GCommand.h"
#include "GScriptGroup_NPC.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GPlayerTalent.h"
#include "GModuleBuff.h"
#include "GFieldInfoMgr.h"
#include "GMsgCommandFacade.h"
#include "GFieldNPCSessionMgr.h"
#include "GNPCSerializer.h"
#include "GPlayerSerializer.h"
#include "GPartySystem.h"
#include "GPartyManager.h"
#include "GScriptGroup_Field.h"

void GReloader::RouteMessage( const wchar_t* pFormat, ... )
{
	wchar_t temp[1024];
	va_list args;
	va_start(args,pFormat);
	vswprintf_s(temp,pFormat,args);
	va_end(args);

	mlog(temp);
	gsys.pMsgCommandFacade->RouteNoticeMsgToAll(temp);
}

bool GReloader::Reload_NPC( int nNPCID )
{
	const wchar_t* pszReloadName = L"NPC";
	RouteMessage(L"> %s Reload Begin (NPCID: %d)\n", pszReloadName, nNPCID);

	GNPCSerializer ns;
	ns.SaveFromManager(nNPCID);

	//
	//	NPC DATA - npc.xml
	//
	RouteMessage(L" NPC data Reloading...\n");
	gmgr.pNPCInfoMgr->Erase(nNPCID);
	if (!gmgr.pNPCInfoMgr->Load(FILENAME_NPCINFO, nNPCID))
	{
		RouteMessage(L"> %s Reload Failed. (npc.xml load failed)\n", pszReloadName);
		return false;
	}

	GNPCInfo* pInfo = gmgr.pNPCInfoMgr->Find(nNPCID);
	if (!pInfo)		
	{
		RouteMessage(L"> %s Reload Failed. (not found npcid)\n", pszReloadName);
		return false;
	}

	ns.RestoreAll();

	RouteMessage(L"> %s Reload Complete\n", pszReloadName);

	return true;
}

bool GReloader::Reload_Field( int nFieldID )
{
	GFieldInfo* pInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	if (!pInfo)		return false;

	const wchar_t* pszReloadName = L"Field";

	RouteMessage(L"> %s Reload Begin (FieldID: %d)\n", pszReloadName, nFieldID);

	vector<GField*> vecFields = gmgr.pFieldMgr->GetFieldList(nFieldID);

	RouteMessage(L" Field Cleanup... (Field Qty: %d)\n", vecFields.size());
	for each (GField* pField in vecFields)
	{
		// 필드 정리
		pField->Clear();
	}
	

	RouteMessage(L" FieldData Reloading...\n");
	if (!gmgr.pFieldInfoMgr->ReloadField(nFieldID))
	{
		RouteMessage(L"> %s Reload Failed. (data load failed)\n", pszReloadName);
		return false;
	}

	for each (GField* pField in vecFields)
	{
		pField->GetSpawn()->Init();
		pField->InitSensor();
		pField->ResetTimer();
		GetScriptField()->OnCreate(pField->GetDelegator());
	}

	RouteMessage(L"> %s Reload Complete\n", pszReloadName);

	return true;
}

bool GReloader::Reload_All()
{
	// 다시 읽기 시작
	RouteMessage(L"Begin reload all.\n");

	gmgr.pNPCMgr->Clear();
	m_setRefreshFieldIDs.clear();
	
	gsys.pPartySystem->GetPartyManager()->Clear();

	if (!DestoryField())	
		return false;

	GPlayerSerializer ps;
	ps.SaveFromManager();

	gsys.pServer->Reload();

	if (!BuildField())		
		return false;
	
	ps.RestoreAll();

	// 다시 읽기 완료
	RouteMessage(L"End reload all.\n");

	return true;
}

bool GReloader::DestoryField()
{
	RouteMessage(L"> Destory fileds.\n");
	
	GFieldMgr::FIELD_BY_UID_MAP& mapField = gmgr.pFieldMgr->GetFieldMap();

	for each(pair<uint64, GField*> pairField in mapField)
	{
		GField* pField = pairField.second;
				
		// 필드ID 목록을 기억해둔다.
		int nFieldID = pField->GetID();
		m_setRefreshFieldIDs.insert(nFieldID);
		
		// 필드 정리
		pField->Clear();
	}
	
	return true;
}

bool GReloader::BuildField()
{
	RouteMessage(L"> Build fields.\n");

	for each(int nFieldID in m_setRefreshFieldIDs)
	{
		if (!gmgr.pFieldInfoMgr->ReloadField(nFieldID))
		{
			RouteMessage(L">> Failed load field.\n");
			return false;
		}

		vector<GField*> vecFields = gmgr.pFieldMgr->GetFieldList(nFieldID);
		for each (GField* pField in vecFields)
		{
			pField->GetSpawn()->Init();
			pField->InitSensor();
			pField->ResetTimer();

			GetScriptField()->OnCreate(pField->GetDelegator());
		}
	}

	return true;
}
