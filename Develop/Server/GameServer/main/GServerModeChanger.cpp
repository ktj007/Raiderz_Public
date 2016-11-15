#include "StdAfx.h"
#include "GServerModeChanger.h"
#include "GConfig.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GMasterServerFacade.h"
#include "CCommandTable_Master.h"
#include "CCommandTable.h"
#include "GStrings.h"
#include "GCommand.h"
#include "GCommandCenter.h"


GServerModeChanger::GServerModeChanger()
{
}

GServerModeChanger::~GServerModeChanger()
{
}

void GServerModeChanger::ChangeServerMode(int nServerMode)
{
	VALID(nServerMode > SERVER_MODE_INVALID);
	VALID(nServerMode < SERVER_MODE_MAX);

	if (GConfig::IsStandAloneMode())
	{
		ChangeServerModeImpl(nServerMode);
	}
	else
	{
		RequestToMasterServer(nServerMode);
	}
}

void GServerModeChanger::ChangeServerModeImpl(int nServerMode)
{
	VALID(nServerMode > SERVER_MODE_INVALID);
	VALID(nServerMode < SERVER_MODE_MAX);

	if (GConfig::m_nServerMode == nServerMode)
		return;

	GConfig::m_nServerMode = nServerMode;

	wchar_t buff[512];
	swprintf_s(buff, L"Change ServerMode : %d (0 : Normal, 1 : Expo)", GConfig::m_nServerMode);

	// logging
	mlog("%s - %s\n", __FUNCTION__, MLocale::ConvUTF16ToAnsi(buff).c_str());

	// 알림 메시지 라우팅
	float fMaintainTime = GStrings::CalcSpeakMaintainTime(buff);

	MCommand* pCmdNarration = MakeNewCommand(MC_NPC_NARRATION, 
		1, 
		NEW_WSTR(buff));
	gsys.pCommandCenter->RouteToAll(pCmdNarration);

	// 설정 커멘드 응답
	MCommand* pCmdChangeServerMode = MakeNewCommand(MC_GM_CHANGE_SERVER_MODE, 
		1,
		NEW_INT(nServerMode));
	gsys.pCommandCenter->RouteToAll(pCmdChangeServerMode);
}

void GServerModeChanger::RequestToMasterServer(int nServerMode)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_ADMIN_CHANGE_SERVER_MODE_REQ,
		1,
		NEW_INT(nServerMode)
		);

	gsys.pMasterServerFacade->Route(pNewCmd);
}
