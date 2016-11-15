#include "stdafx.h"
#include "GPlayerReporter.h"
#include "GServer.h"
#include "GEntityNPC.h"
#include "GEntityPlayer.h"
#include "GField.h"
#include "GConst.h"
#include "GModuleAI.h"
#include "GHateTable.h"
#include "GFieldMgr.h"
#include "GActorHateTracker.h"

bool GPlayerReporter::Report( MUID uidField, MUID uidPlayer )
{
	GField* pField = gmgr.pFieldMgr->GetField(uidField);
	if (!pField)	return false;

	GEntityPlayer* pPlayer = pField->FindPlayer(uidPlayer);
	if (!pPlayer)		return false;

	PLAYER_INFO* pPlayerInfo = pPlayer->GetPlayerInfo();

	const wchar_t LINE[] = L"--------------------------------------------------------------------\n";

	mlog(LINE);
	mlog(LINE);
	mlog("Player Report : %s (%I64d)\n\n", MLocale::ConvUTF16ToAnsi(pPlayerInfo->szName).c_str(), pPlayer->GetUID().Value);
	
	
	// 전투 정보
	mlog("EnemyRefCount : %d\n", pPlayer->GetActorHateTracker().GetQty());
	mlog(LINE);

	return true;
}