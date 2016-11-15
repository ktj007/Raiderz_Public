#include "stdafx.h"
#include "GLoginLogic.h"
#include "CCommandTable.h"
#include "CCommandTable_Login.h"
#include "CCommandResultTable.h"
#include "GPlayerAcceptManager.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GServer.h"
#include "GReserveCmdManager.h"
#include "GPlayerAcceptor_FromLoginServer.h"
#include "GPlayerAcceptor_FromGameServer.h"
#include "PmStatIndexParser.h"
#include "PmRegionStat.h"
#include "GPMSSystem.h"
#include "GLoginLogicMoveGameServer.h"
#include "GClientMoveServerRouter.h"
#include "SCmdRouter_Login.h"
#include "GServerPlayerRouter.h"
#include "GMasterServerFacade.h"


bool GLoginLogic::LoginGameServer(MUID uidPlayer, MUID uidConnectionKey, bool bOnPmang/*=false*/, wstring strPmangStatIndex/*=L""*/, int nPCCafeID)
{
	GPlayerAcceptManager* pPlayerAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
	GPlayerAcceptor_FromLoginServer* pAcceptor = pPlayerAcceptManager->FindPlayerAcceptor_FromLoginServer(uidConnectionKey);

	GClientMoveServerRouter router;

	if (pAcceptor == NULL)
	{
		router.RouteResponseLoginGameServer(uidPlayer, CR_FAIL_LOGIN);
		return false;
	}


	GPlayerObject* pNewPlayerObject = NewPlayerObjectAndInitAccount(uidPlayer, pAcceptor);

	if (!pNewPlayerObject->SerializeSelectCharacter(pAcceptor->GetCID()))
	{
		// 비동기 작업 시작전에 실패.
		router.RouteResponseLoginGameServer(uidPlayer, CR_FAIL_LOGIN);
		return false;
	}

	pPlayerAcceptManager->Del(uidConnectionKey, GPlayerAcceptor::FROM_LOGIN_SERVER);
	pPlayerAcceptManager->Update();

	// Pmang 정보 셋팅
	if (bOnPmang)
		AddPmangStatIndex(uidPlayer, strPmangStatIndex, nPCCafeID);

	return true;
}

bool GLoginLogic::MoveGameServer(MUID uidPlayerSession, MUID uidConnectionKey, bool bOnPmang/*=false*/, wstring strPmangStatIndex/*=L""*/, int nPCCafeID)
{
	GLoginLogicMoveGameServer loginMoveGameServer;
	if (!loginMoveGameServer.Init(uidPlayerSession, uidConnectionKey)) return false;

	if (!loginMoveGameServer.Check()) return false;
	if (!loginMoveGameServer.Apply(bOnPmang, strPmangStatIndex)) return false;

	// Pmang 정보 셋팅
	if (bOnPmang)
	{
		MUID uidRealPlayerUID = loginMoveGameServer.GetRealPlayerUID();
		AddPmangStatIndex(uidRealPlayerUID, strPmangStatIndex, nPCCafeID);
	}

	return true;
}

GPlayerObject* GLoginLogic::NewPlayerObjectAndInitAccount( MUID uidPlayer, GPlayerAcceptor* pAcceptor )
{
	GPlayerObject* pNewPlayerObject = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
	gmgr.pPlayerObjectManager->AddPlayer(pNewPlayerObject);

	// 계정 정보 설정
	pNewPlayerObject->InitAccountInfo(pAcceptor->GetAID(), pAcceptor->GetUserID(), false);

	return pNewPlayerObject;
}

bool GLoginLogic::CheckCommandVersion(MUID uidPlayer, int nCommandVersion)
{
	// 커맨드 버전 확인
	if (!CCommandVersionChecker::IsValid(nCommandVersion))
		return false;
	
	return true;
}

void GLoginLogic::AddPmangStatIndex(MUID uidPlayer, wstring strStatIndex, int nPCCafeID)
{
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayerObject == NULL)	return;

	
	PmStatIndexParser parser(strStatIndex);
	
	if (!parser.Parser())
	{
		return;
	}

	PmStatIndex statIndex = parser.GetStatIndex();
	pPlayerObject->AddStatIndex(&statIndex, nPCCafeID);
	
	
	PmRegionStat* pRegionStat = gsys.pPMSSystem->GetRegionStat();

	PmRegionCode regionCode = statIndex.nRegionCode;
	PmAgeCode ageCode = statIndex.nAgeCode;
	PmGenderCode genderCode = statIndex.nGenderCode;

	pRegionStat->AddUser(regionCode, ageCode, genderCode, nPCCafeID);
}


bool GLoginLogic::DuplicatedLogin(MUID uidPlayer, MUID uidConnectionKey)
{
	/// 플레이어 객체가 있을 경우 연결을 끊는다.
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (pPlayerObject != NULL)
	{
		// 접속해있는 유저가 있으면 로그아웃을 통보한다.
		SCmdRouter_Login router(gsys.pCommandCenter);
		router.DuplicatedPlayerLogin(uidPlayer);

		gsys.pServer->HardDisconnect(uidPlayer);
		gsys.pServer->GetServerInfo().nDisconnCountByDup++;

		return true;
	}

	GPlayerAcceptManager* pPlayerAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
	RVALID_RET(pPlayerAcceptManager, false);

	/// 로그인서버에서 이동 중일 경우 Acceptor 제거.
	GPlayerAcceptor_FromLoginServer* pAcceptorFromLoginServer = pPlayerAcceptManager->FindPlayerAcceptor_FromLoginServer(uidConnectionKey);
	if (pAcceptorFromLoginServer != NULL)
	{
		// 마스터서버에 로그아웃했다고 통보
		GServerPlayerRouter* pServerRouter = gsys.pMasterServerFacade->GetPlayerRouter();
		pServerRouter->RouteDeletePlayer(pAcceptorFromLoginServer->GetAID(), pAcceptorFromLoginServer->GetPlayerUID());

		pPlayerAcceptManager->Del(uidConnectionKey, GPlayerAcceptor::FROM_LOGIN_SERVER);

		return true;
	}

	/// 게임서버간 이동 중일 경우 Acceptor 제거.
	GPlayerAcceptor_FromGameServer* pAcceptorFromGameServer = pPlayerAcceptManager->FindPlayerAcceptor_FromGameServer(uidConnectionKey);
	if (pAcceptorFromGameServer != NULL)
	{
		// 마스터서버에 로그아웃했다고 통보
		GServerPlayerRouter* pServerRouter = gsys.pMasterServerFacade->GetPlayerRouter();
		pServerRouter->RouteDeletePlayer(pAcceptorFromGameServer->GetAID(), pAcceptorFromGameServer->GetPlayerUID());
	
		pPlayerAcceptManager->Del(uidConnectionKey, GPlayerAcceptor::FROM_GAME_SERVER);

		return true;
	}
	
	mlog("Error! GLoginLogic::DuplicatedLogin(), Did not handled!\n");
	return false;
}
