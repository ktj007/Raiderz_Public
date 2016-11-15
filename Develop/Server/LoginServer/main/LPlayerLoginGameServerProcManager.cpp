#include "StdAfx.h"
#include "LPlayerLoginGameServerProcManager.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "LGlobal.h"
#include "SCmdRouter_Login.h"
#include "LCommandCenter.h"
#include "LMasterClient.h"
#include "LServer.h"
#include "CCommandTable_Login.h"

const float LPlayerLoginGameServerProcManager::TIME_UPDATE_SEC = 0.1f;
const int LPlayerLoginGameServerProcManager::LIMIT_COUNT_PLAYER_LOGIN_GAMESERVER_A_UPDATE = 100;

LPlayerLoginGameServerProcManager::LPlayerLoginGameServerProcManager()
{
	m_rgltUpdate.SetTickTime((float)TIME_UPDATE_SEC);
	m_rgltUpdate.Start();
}

LPlayerLoginGameServerProcManager::~LPlayerLoginGameServerProcManager()
{
}

void LPlayerLoginGameServerProcManager::Clear()
{
	for (int i = 0; i < (int)m_qSelMyChar.size(); ++i)
		m_qSelMyChar.pop();
}

void LPlayerLoginGameServerProcManager::Update(float fDelta)
{
	if (!m_rgltUpdate.IsReady(fDelta)) return;
	if (m_qSelMyChar.empty()) return;

	for (int i = 0; i < LIMIT_COUNT_PLAYER_LOGIN_GAMESERVER_A_UPDATE;)
	{
		SEL_MY_CHAR_NODE& refSelMyCharNode = m_qSelMyChar.front();

		if (LoginGameServer(refSelMyCharNode.uidPlayer, refSelMyCharNode.nCharIndex))
			++i;

		m_qSelMyChar.pop();
		if (m_qSelMyChar.empty()) break;
	}
}

void LPlayerLoginGameServerProcManager::Add(MUID uidPlayer, int nCharIndex)
{
	SEL_MY_CHAR_NODE stSelMyCharNode(uidPlayer, nCharIndex);
	m_qSelMyChar.push(stSelMyCharNode);
}

bool LPlayerLoginGameServerProcManager::LoginGameServer(MUID uidPlayer, int nCharIndex)
{
	LPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayerObject == NULL)
		return false;

	if (POS_LOGIN_GAME_SERVER != pPlayerObject->GetState())
		return false;

	ACCOUNT_INFO& accountInfo = pPlayerObject->GetAccountInfo();

	SAccountCharFieldInfo* pCharFieldInfo = pPlayerObject->GetCharFieldInfo(nCharIndex);
	if (pCharFieldInfo == NULL)
	{
		pPlayerObject->OnSelectChar(); ///< 캐릭터 선택 상태로 되돌림.
		SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
		cmdRouter.ResponseSelMyChar(pPlayerObject->GetUID(), CR_FAIL_LOGIN_INVALID_CHAR_INFO);
		return false;
	}


	TD_LOGIN_ACCOUNT_INFO LoginAccountInfo;
	LoginAccountInfo.nAID = accountInfo.nAID;
	LoginAccountInfo.nCID = pCharFieldInfo->nCID;
	LoginAccountInfo.nEnterKey = 0;		// TODO: Client IP를 받아넣을 것. - praptor, 2009.06.03

	wcsncpy_s(LoginAccountInfo.szUserID, accountInfo.szUserID, _TRUNCATE);


	TD_LOGIN_FIELD_INFO	LoginFieldInfo;
	LoginFieldInfo.nSharedFieldID = pCharFieldInfo->nExitedSharedFieldID;
	LoginFieldInfo.nDynamicFieldID = pCharFieldInfo->nExitedDynamicFieldID;
	LoginFieldInfo.uidDynamicFieldGroup = pCharFieldInfo->uidExitedDynamicFieldGroup;

	LMasterClient* pMasterClient = gsys.pServer->GetMasterClient();
	VALID_RET(pMasterClient != NULL, false);

	if (!pMasterClient->Post_PlayerLoginToGameServer(uidPlayer, &LoginAccountInfo, &LoginFieldInfo))
	{
		mlog3("LPlayerLoginGameServerProcManager::LoginGameServer(), Failed! Post PlayerLoginToGameServer.\n");
		return false;
	}

	pPlayerObject->StartWatchingSelMyCharTimeout();

	PostEnterWorldMsg(uidPlayer, CR_ENTER_WORLD_FIND_GAMESERVER);
	
	return true;
}

void LPlayerLoginGameServerProcManager::PostEnterWorldMsg(MUID uidPlayer, int nResult)
{
	MCommand* pNewCommand = gsys.pCommandCenter->MakeNewCommand(MC_COMM_ENTER_WORLD_MSG
		, uidPlayer
		, 1
		, NEW_INT(nResult));

	gsys.pCommandCenter->PostCommand(pNewCommand);
}
