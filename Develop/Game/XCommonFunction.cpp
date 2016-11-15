#include "stdafx.h"
#include "XCommonFunction.h"
#include "XNetwork.h"
#include "XItemManager.h"

void XCommonFunction::Connect( const wchar_t* szLoginID, const wchar_t* szPassword, const wchar_t* szIP )
{
	
	if(wcslen(szLoginID) >= 1)
	{
		gvar.MyInfo.strLoginID = szLoginID;
	}

	if(wcslen(szPassword) >= 1)
	{
		wcscpy_s(gvar.Temp.szPassword, szPassword);
	}
	

	if (global.net) global.net->PostConnectToLoginServer(gvar.Network.szIP);


#ifndef _PUBLISH
	mlog("Try to Connect(%s)\n", MLocale::ConvUTF16ToAnsi(szIP).c_str());
#endif
}

const wchar_t* XCommonFunction::GetPlayerName( MUID uid )
{
	XNetPlayer* pNetPlayer = gg.omgr->FindNetPlayer(uid);
	if( !pNetPlayer) return L"";
	if (pNetPlayer->GetEntityType() != ETID_PLAYER) return L"";

	return pNetPlayer->GetName();
}

const wchar_t* XCommonFunction::GetItemName( int nItemID )
{
	XItemData* pItemData = info.item->GetItemData(nItemID);
	if( pItemData == NULL) return L"";

	return pItemData->GetName();
}
