#include "StdAfx.h"
#include "XCmdHandler_Fatigue.h"
#include "XStrings.h"

XCmdHandler_Fatigue::XCmdHandler_Fatigue(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_FATIGUE_UPDATE,	OnFatigueUpdate);
}

MCommandResult XCmdHandler_Fatigue::OnFatigueUpdate(MCommand* pCommand, MCommandHandler* pHandler)
{
	FATIGUE_TYPE nFatigueType;
	if (pCommand->GetParameter(&nFatigueType, 0, MPT_INT)==false) return CR_ERROR;
		
	if (gvar.MyInfo.nFatigueType != nFatigueType)
	{
		const TCHAR* pStr;

		switch (nFatigueType)
		{
		case FT_TOO_BAD:	pStr = XGetStr( L"FATIGUE_CHANGEDTOWORST");		break;
		case FT_BAD:		pStr = XGetStr( L"FATIGUE_CHANGEDTOBAD");		break;
		case FT_NORMAL:		pStr = XGetStr( L"FATIGUE_CHANGEDTONORMAL");	break;
		case FT_GOOD:		pStr = XGetStr( L"FATIGUE_CHANGEDTOGOOD");		break;
		case FT_VERY_GOOD:	pStr = XGetStr( L"FATIGUE_CHANGEDTOBEST");		break;
		}

		global.ui->OnSystemMsg( pStr );
		global.ui->OnPresentationLower(pStr , PRESENTATION_ICON_EXCLAMATION);

		gvar.MyInfo.nFatigueType = nFatigueType;
	}

	if ( global.script)		global.script->GetGlueGameEvent().OnGameEvent("CHARINFO", "FATIGUE");
	
	return CR_TRUE;
}