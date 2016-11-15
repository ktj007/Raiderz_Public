#include "StdAfx.h"
#include "LMoveServerLogicFromGameServer.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "LGlobal.h"
#include "LPlayerAcceptor_FromGameServer.h"
#include "LPlayerAcceptManager.h"
#include "MLocale.h"
#include "LCmdRouter_MoveServer.h"
#include "PmStatIndexParser.h"
#include "PmRegionStat.h"
#include "LPMSSystem.h"
#include "LCmdRouter_Player.h"


LMoveServerLogicFromGameServer::LMoveServerLogicFromGameServer(void)
{
}

LMoveServerLogicFromGameServer::~LMoveServerLogicFromGameServer(void)
{
}

bool LMoveServerLogicFromGameServer::MoveLoginServer(MUID uidPlayer, MUID uidConnectionKey, bool bOnPmang/*=false*/, wstring strPmangStatIndex/*=L""*/, int nPCCafeID/* = 0*/)
{
	if (!Check(uidPlayer, uidConnectionKey)) return false;
	Apply(uidPlayer, uidConnectionKey);

	if (bOnPmang)
		AddPmangStatIndex(uidPlayer, strPmangStatIndex, nPCCafeID);

	return true;
}

bool LMoveServerLogicFromGameServer::Check(MUID uidPlayer, MUID uidConnectionKey)
{
	LPlayerAcceptor_FromGameServer* pAcceptor = gmgr.pPlayerAcceptManager->FindAcceptor(uidConnectionKey);
	if (pAcceptor == NULL)
	{
		LCmdRouter_MoveServer::RouteResponseMoveLoginServer(uidPlayer, CR_FAIL_LOGIN, MUID::ZERO);
		return false;
	}

	return true;
}
void LMoveServerLogicFromGameServer::Apply(MUID uidPlayer, MUID uidConnectionKey)
{
	LPlayerAcceptor_FromGameServer* pAcceptor = gmgr.pPlayerAcceptManager->FindAcceptor(uidConnectionKey);
	if (pAcceptor == NULL) return;

	/// PlayerObject 추가
	LPlayerObject* pNewPlayer = (LPlayerObject*)gmgr.pPlayerObjectManager->NewObject(uidPlayer);
	gmgr.pPlayerObjectManager->AddObject(pNewPlayer);

	/// 계정 정보 설정
	ACCOUNT_INFO AccInfo;
	AccInfo.nAID	= pAcceptor->GetAID();
	AccInfo.bNewAcc = false;

	tstring tszName = MLocale::ConvUTF16ToTCHAR(pAcceptor->GetName().c_str());
	_tcsncpy_s(AccInfo.szUserID, tszName.c_str(), tszName.length());
	AccInfo.szUserID[tszName.length()] = 0;

	pNewPlayer->InitAccountInfo(AccInfo);

	/// 사용된 Acceptor 제거
	gmgr.pPlayerAcceptManager->Del(uidConnectionKey);

	/// MasterServer 에 상태 변경 알림
	LCmdRouter_Player::PostPlayerState_SELECT_CHAR(AccInfo.nAID, uidPlayer);

	/// 클라이언트에 응답
	LCmdRouter_MoveServer::RouteResponseMoveLoginServer(uidPlayer, CR_SUCCESS, uidPlayer);
}

void LMoveServerLogicFromGameServer::AddPmangStatIndex(MUID uidPlayer, wstring strStatIndex, int nPCCafeID)
{
}
