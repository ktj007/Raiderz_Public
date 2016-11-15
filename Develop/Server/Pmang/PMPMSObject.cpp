#include "StdAfx.h"
#include "PMPMSObject.h"
#include "PMPMSConnector.h"
#include "MProcessController.h"
#include "CpuUsage.h"

PMPMSObject::PMPMSObject(PMPMSConnector* pPMS)
: m_pPMS(pPMS)
{
	GetCpuUsage(); // dummy call
}

PMPMSObject::~PMPMSObject(void)
{
}

BOOL PMPMSObject::OnHeartbeatReq( LONG lIndex )
{
	UNUSED(lIndex);
	return TRUE;
}

BOOL PMPMSObject::OnOrderReq( LPCSTR lpszCmdName, LPCSTR lpszCtlVal, LPSTR lpResult, LONG *lpResultLen, DWORD dwSSN, DWORD dwCategoryID )
{
	UNUSED(lpszCmdName);
	UNUSED(lpszCtlVal);
	UNUSED(lpResult);
	UNUSED(lpResultLen);
	UNUSED(dwSSN);
	UNUSED(dwCategoryID);

	return FALSE;
}

BOOL PMPMSObject::OnAnnounceReq( DWORD dwSSN, DWORD dwCategoryID, LPCSTR lpszMsg )
{
	UNUSED(dwSSN);
	UNUSED(dwCategoryID);
	UNUSED(lpszMsg);

	GetCpuUsage(); // CPU
	return TRUE;
}

BOOL PMPMSObject::OnPerformInfoReq( IPMSPerformanceInfo *pvPerform )
{
	VLONG vData;
	vData.push_back(GetCpuUsage()); // CPU Usage
	vData.push_back(MGetCurrProcessMemory()); // Memory Usage
	pvPerform->AddPerformanceInfo(&vData);

	return TRUE;
}

BOOL PMPMSObject::OnRegionInfoReq( IPMSRegionInfoList* plstRegionInfo )
{
	UNUSED(plstRegionInfo);

	return FALSE;
}

BOOL PMPMSObject::OnStatInfoReq( IPMSStatInfoList* plstStatInfo )
{
	UNUSED(plstStatInfo);

	return FALSE;
}

BOOL PMPMSObject::OnRegionInfoPCReq( IPMSRegionInfoListPC* plstRegionInfo )
{
	UNUSED(plstRegionInfo);

	return FALSE;
}

BOOL PMPMSObject::OnStatInfoPCReq( IPMSStatInfoListPC* plstStatInfo )
{
	UNUSED(plstStatInfo);

	return FALSE;
}

PMPMSConnector* PMPMSObject::GetPMS()
{
	return m_pPMS;
}