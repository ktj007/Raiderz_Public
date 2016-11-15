#include "StdAfx.h"
#include "XDummyHelper.h"
#include "XFieldInfo.h"
#include "XBirdDummyAgent.h"


int XDummyHelper::GetRandomFieldID(int nIgnoreFieldID)
{
	vector<int> vecFieldID;
	CSFieldList& rFieldList = info.field->GetFieldList();
	for(CSFieldList::iterator it = rFieldList.begin(); it != rFieldList.end(); it++)
	{		
		CSFieldFileInfo* pInfo = it->second;
		if (pInfo == NULL)					continue;
		if (pInfo->bDynamic)				continue;

		int nFiedlID = it->first;
		vecFieldID.push_back(nFiedlID);
	}
	if (vecFieldID.empty()) return INVALID_ID;

	int nRearIndex = vecFieldID.size() - 1;

	// 현재 필드와 다른 필드가 나올때까지 반복한다 - 만약만약만약 계속 같은 필드 나오면 문제 포기.
	for(int i = 0; i < 10000; i++)
	{
		int nRandomIndex = RandomNumber(0, nRearIndex);
		int nFieldID = vecFieldID[nRandomIndex];

		if (nFieldID == nIgnoreFieldID)	continue;

		return nFieldID;
	}

	return INVALID_ID;
}

int XDummyHelper::GetRandomIDFromList(vector<int>& vecIDList, int nIgnoreID)
{
	if (vecIDList.size() < 2) return INVALID_ID;

	if (vecIDList.size() == 2)
	{
		if (nIgnoreID == vecIDList[0]) return vecIDList[1];
		return vecIDList[0];
	}

	int nRearIndex = vecIDList.size() - 1;
	for(int i = 0; i < 10000; i++)
	{
		int nRandomIndex = RandomNumber(0, nRearIndex);
		int nCurID = vecIDList[nRandomIndex];

		if (nCurID == nIgnoreID)	continue;

		return nCurID;
	}

	return INVALID_ID;
}

void XDummyHelper::PostMoveRandomPosition(XBirdDummyAgent* pAgent)
{
	int nFieldID = pAgent->GetLoginFieldID();
	int nChannelID = pAgent->GetLoginChannelID();

	XFieldInfo* pInfo = info.field->GetSimpleFieldInfo(nFieldID);
	if (NULL == pInfo) return;
	vec3 vPos = RandomNumber(vec3(0, 0, 0), vec3(pInfo->m_fMaxX, pInfo->m_fMaxY, 0));

	pAgent->SetLoginField(nFieldID, nChannelID, vPos);

	XBirdClient* pClient = pAgent->GetClient();
	XBIRDPOSTCMD2(pClient, MC_GM_MOVE_REQ, MCommandParameterInt(nFieldID), MCommandParameterVector(vPos));
}

std::wstring XDummyHelper::MakeAutoID()
{
	wstring strName = GetComputerName();
	return strName;

}

wstring XDummyHelper::GetComputerName()
{
	wchar_t    szBuffer[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD   dwNameSize = MAX_COMPUTERNAME_LENGTH + 1;

	::GetComputerName(szBuffer, &dwNameSize);
	wstring strComputerName = wstring(szBuffer);
	wstring ret = wstring(L"D") + strComputerName;
	return ret;
}

std::wstring XDummyHelper::MakeCharacterName( wstring strLoginID, int nAgentID )
{
	wchar_t szID[64];
	_itow_s(nAgentID, szID, 64, 10);

	wstring strAgentID = szID;
	wstring strAgentIDToABC;

	for (size_t i = 0; i < strAgentID.size(); ++i)
	{
		if (strAgentID[i] == '0') strAgentIDToABC += L"A";
		else if (strAgentID[i] == '1') strAgentIDToABC += L"B";
		else if (strAgentID[i] == '2') strAgentIDToABC += L"C";
		else if (strAgentID[i] == '3') strAgentIDToABC += L"D";
		else if (strAgentID[i] == '4') strAgentIDToABC += L"E";
		else if (strAgentID[i] == '5') strAgentIDToABC += L"F";
		else if (strAgentID[i] == '6') strAgentIDToABC += L"G";
		else if (strAgentID[i] == '7') strAgentIDToABC += L"H";
		else if (strAgentID[i] == '8') strAgentIDToABC += L"I";
		else if (strAgentID[i] == '9') strAgentIDToABC += L"J";
		else strAgentIDToABC += L"K";
	}

	wstring ret = strLoginID + strAgentIDToABC;

	return ret;
}
