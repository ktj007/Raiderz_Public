#include "stdafx.h"
#include "GServerDumper.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GConst.h"
#include "GConfig.h"
#include "GFieldMgr.h"
#include "GField.h"
#include "GSharedField.h"
#include "GSharedFieldMaster.h"
#include "GDynamicFieldMaster.h"
#include "GServer.h"
#include "MNetServer.h"
#include "MLinkMap.h"
#include "MLink.h"


GServerDumper::GServerDumper()
{

}

GServerDumper::~GServerDumper()
{

}

void GServerDumper::Dump(std::wstring strID, MLogger* pLogger/* = NULL*/)
{	
	if (pLogger == NULL)
	{
		std::wstring strPath = L"./";
		std::wstring strFileName;
		strFileName = strPath + MakeFileName(strID);

		pLogger = &m_Logger;
		pLogger->Init(MLOG_LEVEL_DEBUG, MLOG_FILE, MLocale::ConvUTF16ToAnsi(strFileName.c_str()).c_str(), NULL, NULL);
		pLogger->SetPrintHeader(false);
	}

	Dump_Links(pLogger);
	Dump_Players(pLogger);
	Dump_Fields(pLogger);
}

void GServerDumper::Log( MLogger* pLogger, const wchar_t* pFormat, ... )
{
	wchar_t buf[8192];

	va_list args;
	va_start(args, pFormat);
	vswprintf_s(buf, pFormat,args);
	va_end(args);

	pLogger->Output(buf, MLOG_LEVEL_INFORMATION);

}

void GServerDumper::Dump_Links(MLogger* pLogger)
{
	Log(pLogger, L"\n====== Links ======\n");

	MLinkMap* pLinkMap = gsys.pServer->GetNetServer()->GetLinks();

	for each (MLinkMap::LINK_MAP::value_type pair in pLinkMap->GetLinkMap())
	{
		minet::MLink* pLink = pair.second;

		Dump_Link(pLogger, pLink);
	}
}
void GServerDumper::Dump_Link(MLogger* pLogger, minet::MLink* pLink)
{
	Log(pLogger, L"UID = (%u:%u) , Socket = %u \n"
		, pLink->GetUID().High, pLink->GetUID().Low
		, pLink->GetSocket()
		);
}

void GServerDumper::Dump_Players(MLogger* pLogger)
{
	// 플레이어 정보
	// ID, UID, MUID, Field, Sector
	//
	Log(pLogger, L"\n====== Players ======\n");

	GPlayerObjectManager* pPlayerObjectManager = gmgr.pPlayerObjectManager;
	if (!pPlayerObjectManager) return;

	for each (GPlayerObjectManager::GPlayerObjectMap::value_type pair in pPlayerObjectManager->GetObjects())
	{
		GPlayerObject* pPlayerObject = pair.second;
		GEntityPlayer* pEntityPlayer = pPlayerObject->GetEntity();

		Dump_Player(pLogger, pPlayerObject, pEntityPlayer);
	}

}
void GServerDumper::Dump_Player( MLogger* pLogger, GPlayerObject* pPlayerObject, GEntityPlayer* pEntityPlayer )
{
	Log(pLogger, L"AID = %d , ID = %s , Name = %s , UID = (%u:%u) , UIID = %d , CID = %d , PS = %d\n"
		, pPlayerObject->GetAccountInfo().nAID
		, pPlayerObject->GetAccountInfo().szUserID
		, pEntityPlayer->GetName()
		, pEntityPlayer->GetUID().High, pEntityPlayer->GetUID().Low
		, pEntityPlayer->GetUIID()
		, pEntityPlayer->GetCID()
		, pPlayerObject->GetState()
		);

	GField* pField = pEntityPlayer->GetField();
	if (pField)
	{
		Log(pLogger, L"\tFieldID = %d , Field UID = (%u:%u) , Sector = (%d,%d) , pos = (%.1f,%.1f,%.1f) , dir = (%.1f,%.1f,%.1f)\n"
			, pField->GetID()
			, pField->GetUID().High, pField->GetUID().Low
			, pEntityPlayer->GetSectorX(), pEntityPlayer->GetSectorY()
			, pEntityPlayer->GetPos().x, pEntityPlayer->GetPos().y, pEntityPlayer->GetPos().z
			, pEntityPlayer->GetDir().x, pEntityPlayer->GetDir().y, pEntityPlayer->GetDir().z
			);
	}

}

void GServerDumper::Dump_Fields(MLogger* pLogger)
{
	Log(pLogger, L"\n====== Fields ======\n");

	GFieldMgr* pFieldMgr = gmgr.pFieldMgr;
	if (!pFieldMgr) return;

	GFieldMgr::SHARED_FIELD_MASTER_MAP& refSharedFieldMasterMap = pFieldMgr->GetSharedFieldMap();

	for each (GFieldMgr::SHARED_FIELD_MASTER_MAP::value_type pair in pFieldMgr->GetSharedFieldMap())
	{
		GSharedFieldMaster* pFieldMaster = pair.second;

		for each (SHAREDFIELD_MAP::value_type pairChannel in pFieldMaster->GetChannels())
		{
			GSharedField* pField = pairChannel.second;
			Dump_SharedField(pLogger, pField);
		}
	}

	for each (GFieldMgr::DYNAMIC_FIELD_GROUP_MAP::value_type pair in pFieldMgr->GetDynamicFieldGroupMap())
	{
		GDynamicFieldMaster* pFieldMaster = pair.second;

		Dump_DynamicField(pLogger, pFieldMaster);
	}
}

void GServerDumper::Dump_SharedField( MLogger* pLogger, GSharedField* pField )
{
	Log(pLogger, L"SharedField(%d:%s) , PlayerQty = %d , UID = (%u:%u) , Channel = %d , Default = %s \n"
		, pField->GetID()
		, pField->GetInfo()->m_strFieldName.c_str()
		, pField->GetPlayerQty()
		, pField->GetUID().High, pField->GetUID().Low
		, static_cast<GSharedField* >(pField)->GetChannelID()
		, static_cast<GSharedField* >(pField)->IsDefaultChannel() ? L"TRUE" : L"FALSE"
		);
}
void GServerDumper::Dump_DynamicField( MLogger* pLogger, GDynamicFieldMaster* pFieldMaster )
{
	tstring strName = (pFieldMaster->GetInfo()->pStartFieldInfo == NULL ? _T("NULL") : pFieldMaster->GetInfo()->pStartFieldInfo->m_strFieldName);

	Log(pLogger, L"DynamicField(%d:%s) , PlayerQty = %d , UID = (%u:%u) \n"
		, pFieldMaster->GetGroupID()
		, strName.c_str()
		, pFieldMaster->GetPlayerQty()
		, pFieldMaster->GetUID().High, pFieldMaster->GetUID().Low
		);
}

std::wstring GServerDumper::MakeFileName(std::wstring strID)
{
	wchar_t szFilename[MAX_PATH];

	SYSTEMTIME time;
	::GetLocalTime(&time);
	swprintf_s( szFilename, L"ServerDump_%s_%d%02d%02d_%02d%02d%02d.txt"
								, strID.c_str()
								, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	return std::wstring(szFilename);
}
