#include "StdAfx.h"
#include "GLoginLogicMoveGameServer.h"
#include "CCommandTable.h"
#include "CCommandTable_Login.h"
#include "CCommandResultTable.h"
#include "GGlobal.h"
#include "GServer.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"
#include "GPlayerAcceptManager.h"
#include "GPlayerAcceptor_FromGameServer.h"
#include "GAsyncTaskFactory.h"
#include "GASyncTask_LazyLoginMoveGameServer.h"
#include "GReserveCmdManager.h"
#include "GCommandCenter.h"
#include "GGuildSystem.h"
#include "GClientMoveServerRouter.h"


GLoginLogicMoveGameServer::GLoginLogicMoveGameServer()
: m_pGlobalPlayerAcceptManager(NULL)
, m_pAcceptorRef(NULL)
{
}

GLoginLogicMoveGameServer::~GLoginLogicMoveGameServer()
{
}

bool GLoginLogicMoveGameServer::Init(MUID uidPlayerSession, MUID uidConnectionKey)
{
	// Acceptor 찾기
	m_pGlobalPlayerAcceptManager = gmgr.pPlayerObjectManager->GetAcceptManager();
	if (NULL == m_pGlobalPlayerAcceptManager)
		return false;

	m_pAcceptorRef = m_pGlobalPlayerAcceptManager->FindPlayerAcceptor_FromGameServer(uidConnectionKey);
	if (NULL == m_pAcceptorRef)
	{
		dlog("Error! GLoginLogicMoveGameServer::Init(), pAcceptor is NULL\n");

		GClientMoveServerRouter router;
		router.RouteResponseMoveGameServer(uidPlayerSession, CR_FAIL_MOVE_GAME_SERVER);
		return false;
	}

	m_uidPlayerSession = uidPlayerSession;
	m_uidConnectionKey = uidConnectionKey;
	m_uidRealPlayerUID = m_pAcceptorRef->GetPlayerUID();
	
	return true;
}

bool GLoginLogicMoveGameServer::Check()
{
	return true;
}

bool GLoginLogicMoveGameServer::Apply(bool bOnPmang/*=false*/, wstring strPmangStatIndex/*=L""*/)
{
	// 옛 플레이어 Object 가 남아있는지 체크
	if (IsExistPastPlayerObject(m_uidRealPlayerUID))
	{
		NewLazyLoginMoveGameServerTask(bOnPmang, strPmangStatIndex);
		return false;
	}
	
	// 세션의 UID 를 이전 서버에서 사용하던 UID 로 재할당.
	if (!gsys.pServer->ReAllocLinkUID(m_uidPlayerSession, m_uidRealPlayerUID))
		return false;
	
	GPlayerObject* pNewPlayerObject = NewPlayerObjectAndInitAccount(m_uidRealPlayerUID, m_pAcceptorRef);
	if (NULL == pNewPlayerObject) return false;

	GEntityPlayer* pNewEntityPlayer = pNewPlayerObject->GetEntity();
	if (NULL == pNewEntityPlayer) return false;

	GClientMoveServerRouter router;

	if (!m_pAcceptorRef->SerializePlayerGameData(pNewEntityPlayer))
	{
		dlog("Error! Acceptor, SerializePlayerGameData is Failed!\n");
		router.RouteResponseMoveGameServer(m_uidRealPlayerUID, CR_FAIL_MOVE_GAME_SERVER);
		return false;
	}
	DelAcceptor();

	gmgr.pPlayerObjectManager->AddCID(pNewPlayerObject);
	pNewPlayerObject->GetEntity()->OnMovedFromOtherGameServer();

	gsys.pGuildSystem->Serialize(pNewEntityPlayer);

	// 서버 이동 처리 중에 받았던 커맨드들 일괄 처리
	gmgr.pReserveCmdManager->RunAndDeleteReserveCmd(pNewEntityPlayer->GetCID());

	// 응답
	UIID nUIID = pNewPlayerObject->GetEntity()->GetUIID();

	router.RouteResponseMoveGameServer(m_uidRealPlayerUID, CR_SUCCESS, m_uidRealPlayerUID, nUIID);

	return true;
}

void GLoginLogicMoveGameServer::NewLazyLoginMoveGameServerTask(bool bOnPmang/*=false*/, wstring strPmangStatIndex/*=L""*/)
{
	GAsyncTaskManager* pTaskMgr = gsys.pServer->GetAsyncTaskManager();
	GASyncTask_LazyLoginMoveGameServer* pTask = GAsyncTaskFactory::NewTask_LazyLoginMoveGameServer(pTaskMgr);
	pTask->Init(m_uidPlayerSession, m_uidConnectionKey, m_uidRealPlayerUID, bOnPmang, strPmangStatIndex);
}

void GLoginLogicMoveGameServer::DelAcceptor()
{
	m_pGlobalPlayerAcceptManager->Del(m_uidConnectionKey, GPlayerAcceptor::FROM_GAME_SERVER);
	m_pGlobalPlayerAcceptManager->Update();
}

bool GLoginLogicMoveGameServer::IsExistPastPlayerObject(MUID uidPlayer)
{
	GPlayerObject* pPastPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (NULL == pPastPlayerObject)
		return false;
	
	if (true == pPastPlayerObject->IsMoveGameServer() ||
		POS_LOGOUT == pPastPlayerObject->GetState())
	{
		return true;
	}
	
	/// 이 메시지가 출력되면 aibeast 에게 알려주세요.
	mlog3("Error! GLoginLogicMoveGameServer::IsExistPastPlayerObject(), Past PlayerObject State is Not MovingServer!!! (CurState=%d)\n"
		, pPastPlayerObject->GetState());

	return true;
}

GPlayerObject* GLoginLogicMoveGameServer::NewPlayerObjectAndInitAccount( MUID uidPlayer, GPlayerAcceptor* pAcceptor )
{
	GPlayerObject* pNewPlayerObject = gmgr.pPlayerObjectManager->NewPlayerObject(uidPlayer);
	gmgr.pPlayerObjectManager->AddPlayer(pNewPlayerObject);

	// 계정 정보 설정
	pNewPlayerObject->InitAccountInfo(m_pAcceptorRef->GetAID(), m_pAcceptorRef->GetUserID(), false);

	return pNewPlayerObject;
}
