#include "StdAfx.h"
#include "GGMExpoHandler.h"
#include "GGlobal.h"
#include "GPlayerSystem.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GDBManager.h"


GGMExpoHandler::GGMExpoHandler(void)
{
}

GGMExpoHandler::~GGMExpoHandler(void)
{
}

void GGMExpoHandler::ResetCharacters()
{
	mlog("!Start! - Reset Expo Characters\n");

	/// DB 정보 초기화
	gsys.pDBManager->ResetExpoCharacters();

	/// 모든 캐릭터는 캐릭터 선택화면으로
	GPlayerObjectManager::GPlayerObjectMap::iterator itr;
	itr = gmgr.pPlayerObjectManager->GetObjects().begin();
	for ( ; itr != gmgr.pPlayerObjectManager->GetObjects().end(); ++itr)
	{
		GPlayerObject* pPlayer = (*itr).second;

		ACCOUNT_INFO refAccountInfo = pPlayer->GetAccountInfo();
		wstring strUserID = refAccountInfo.szUserID;
		if (!IsExpoAccount(strUserID))
			continue;

		gsys.pPlayerSystem->MoveToSelectChar(pPlayer, true);
	}

	mlog("!Complete! - Reset Expo Characters\n");
}

void GGMExpoHandler::ResetPublicBoothCharacter( GPlayerObject* pPlayerObject )
{
	if (NULL == pPlayerObject) return;

	gsys.pDBManager->ResetExpoPublicBoothCharacter(pPlayerObject->GetEntity()->GetCID());
	gsys.pPlayerSystem->MoveToSelectChar(pPlayerObject);
}

bool GGMExpoHandler::IsExpoAccount(wstring strUserID)
{
	const tr1::wregex pattern(L"e3expo+(0[1-5]|[1-3][1-5])");

	if (!tr1::regex_match(strUserID, pattern))
		return false;

	return true;
}

bool GGMExpoHandler::IsExpoPublicBoothAccount( wstring strUserID )
{
	const tr1::wregex pattern(L"pe3expo+(0[1-9]|1[0-2])");

	if (!tr1::regex_match(strUserID, pattern))
		return false;

	return true;
}