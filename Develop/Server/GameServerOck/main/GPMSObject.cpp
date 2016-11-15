#include "StdAfx.h"
#include "GPMSObject.h"
#include "GGlobal.h"
#include "GMsgCommandFacade.h"
#include "PMPMSConnector.h"
#include "PmRegionStat.h"
#include "MLocale.h"

GPMSObject::GPMSObject(PMPMSConnector* pPMS)
: PMPMSObject(pPMS)
{
}

GPMSObject::~GPMSObject(void)
{
}

BOOL GPMSObject::OnHeartbeatReq( LONG lIndex )
{
	return TRUE;
}

BOOL GPMSObject::OnAnnounceReq( DWORD dwSSN, DWORD dwCategoryID, LPCSTR lpszMsg )
{
	VALID_RET(lpszMsg, FALSE);

	gsys.pMsgCommandFacade->RouteNoticeMsgToAll(MLocale::ConvAnsiToUCS2(lpszMsg).c_str());

	return TRUE;
}

BOOL GPMSObject::OnRegionInfoReq( IPMSRegionInfoList* plstRegionInfo )
{
	VALID_RET(plstRegionInfo, FALSE);

	DWORD nSSN = GetPMS()->GetSSN();
	DWORD nCategory = GetPMS()->GetCategory();
	VLONG* vRegionStat = GetPMS()->GetRegionStat()->GetRegionStat();

	plstRegionInfo->AddRegionInfo(nSSN, nCategory, vRegionStat);

	return TRUE;
}

BOOL GPMSObject::OnStatInfoReq( IPMSStatInfoList* plstStatInfo )
{
	DWORD nSSN = GetPMS()->GetSSN();
	DWORD nCategory = GetPMS()->GetCategory();
	DWORD nCU = GetPMS()->GetRegionStat()->GetTotalUserCount();

	plstStatInfo->AddStatInfoList(nSSN, nCategory, nCU, 0, 0, 0, NULL);

	return TRUE;
}

BOOL GPMSObject::OnRegionInfoPCReq(IPMSRegionInfoListPC* plstRegionInfo)
{
	VALID_RET(plstRegionInfo, FALSE);

	DWORD nSSN = GetPMS()->GetSSN();
	DWORD nCategory = GetPMS()->GetCategory();
	VLONG* vRegionStat = GetPMS()->GetRegionStat()->GetRegionStatPC();

	plstRegionInfo->AddRegionInfoPC(nSSN, nCategory, vRegionStat);

	return TRUE;
}

BOOL GPMSObject::OnStatInfoPCReq(IPMSStatInfoListPC* plstStatInfo)
{
	DWORD nSSN = GetPMS()->GetSSN();

	map<int, int>& mapTotalUserCountPC = GetPMS()->GetRegionStat()->GetTotalUserCountPC();

	for each(const pair<int, int>& pairPCCafeCount in mapTotalUserCountPC)
	{
		DWORD nCategory = pairPCCafeCount.first;
		DWORD nCU = pairPCCafeCount.second;

		plstStatInfo->AddStatInfoListPC(nSSN, nCategory, nCU, 0, 0, 0, NULL);
	}

	return TRUE;
}
