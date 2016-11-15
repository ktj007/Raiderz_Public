#include "StdAfx.h"
#include "ZServerInfoDumper.h"
#include "MLocale.h"
#include "ZFieldInfo.h"
#include "ZFieldInfoManager.h"
#include "ZFieldManager.h"
#include "ZSharedFieldMaster.h"
#include "ZSharedField.h"
#include "ZPlayerManager.h"
#include "ZDynamicFieldMaster.h"


ZServerInfoDumper::ZServerInfoDumper(void)
{
}

ZServerInfoDumper::~ZServerInfoDumper(void)
{
}

void ZServerInfoDumper::Dump(wstring strPlayerName)
{
	std::wstring strPath = L"./";
	std::wstring strFileName;
	strFileName = strPath + MakeFileName(strPlayerName);

	m_Logger.Init(MLOG_LEVEL_DEBUG, MLOG_FILE, MLocale::ConvUTF16ToAnsi(strFileName.c_str()).c_str(), NULL, NULL);
	m_Logger.SetPrintHeader(false);

	DumpFields();
	DumpPlayers();
}

void ZServerInfoDumper::DumpFields()
{
	Log(L"\n====== Fields ======\n");

	Log(L"\n==== SharedField Count = %d ====\n", gmgr.pFieldManager->GetSharedFieldCount());

	for (ZBaseFieldManager::ZSharedFieldMasterMap::iterator itor = gmgr.pFieldManager->GetSharedFieldMasterMap().begin();
		itor != gmgr.pFieldManager->GetSharedFieldMasterMap().end(); ++itor)
	{
		ZSharedFieldMaster* pFieldMaster = (*itor).second;
		ZSharedFieldMaster::ZChannelVector& refChannelVec = pFieldMaster->GetChannelVec();

		ZFieldInfo* pFieldInfo = gmgr.pFieldInfoManager->Find(pFieldMaster->GetFieldID());
		if (pFieldInfo == NULL) continue;

		for (size_t i = 0; i < refChannelVec.size(); ++i)
		{
			ZSharedField* pSharedField = refChannelVec[i];
			Log(L"ID = %d , Name = %s , ChannelID = %d , PlayerQnt = %d , IsDefault = %s , GameServerID = %d \n"
				, pFieldMaster->GetFieldID()
				, pFieldInfo->m_strFieldName.c_str()
				, pSharedField->GetChannelID()
				, pSharedField->GetPlayerCount()
				, pSharedField->IsDefaultChannel() ? L"true" : L"false"
				, pSharedField->GetGameServerID()
				);
		}
	}

	Log(L"\n==== DynamicFieldMaster Count = %d ====\n", gmgr.pFieldManager->GetDynamicFieldMasterCount());
	for (ZBaseFieldManager::ZDynamicFieldMasterMap::iterator itor = gmgr.pFieldManager->GetDynamicFieldMasterMap().begin();
		itor != gmgr.pFieldManager->GetDynamicFieldMasterMap().end(); ++itor)
	{
		ZDynamicFieldMaster* pDynamicFieldMaster = (*itor).second;

		Log(L"ID = %d , Type = %d , PlayerCount = %d , GameServerID = %d \n"
			, pDynamicFieldMaster->GetInfo()->nID
			, pDynamicFieldMaster->GetType()
			, pDynamicFieldMaster->GetPlayerCount()
			, pDynamicFieldMaster->GetGameServerID()
			);
	}
}

void ZServerInfoDumper::DumpPlayers()
{
	Log(L"\n====== Players ======\n");

	ZPlayerAIDMap::iterator itor = const_cast<ZPlayerAIDMap& >(gmgr.pPlayerManager->GetPlayerMap()).begin();
	for (; itor != gmgr.pPlayerManager->GetPlayerMap().end(); ++itor)
	{
		ZPlayer* pPlayer = (*itor).second;

		Log(L"AID = %d , UserID = %s , CID = %d , UID = (%u:%u) , Name = %s , FieldID = %d , PartyUID = (%u:%u) , State = %d , GameServerID = %d \n"
			, pPlayer->GetAID()
			, pPlayer->GetUserID().c_str()
			, pPlayer->GetCID()
			, pPlayer->GetUID().High, pPlayer->GetUID().Low
			, pPlayer->GetPlayerName().c_str()
			, pPlayer->GetFieldID()
			, pPlayer->GetPartyUID().High, pPlayer->GetPartyUID().Low
			, pPlayer->GetState()
			, pPlayer->GetGameServerID()
			);
	}
}



void ZServerInfoDumper::Log( const wchar_t* pFormat, ... )
{
	wchar_t buf[8192];

	va_list args;
	va_start(args, pFormat);
	vswprintf_s(buf, pFormat,args);
	va_end(args);

	m_Logger.Output(buf, MLOG_LEVEL_INFORMATION);

}

std::wstring ZServerInfoDumper::MakeFileName(std::wstring strPlayerName)
{
	wchar_t szFilename[MAX_PATH];

	SYSTEMTIME time;
	::GetLocalTime(&time);
	swprintf_s( szFilename, L"ServerDump_%s_%d%02d%02d_%02d%02d%02d.txt"
		, strPlayerName.c_str()
		, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	return std::wstring(szFilename);
}
