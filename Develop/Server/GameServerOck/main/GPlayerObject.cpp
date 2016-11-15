#include "stdafx.h"
#include "GPlayerObject.h"
#include "GConst.h"
#include "GCalculator.h"
#include "GFatigueSystem.h"
#include "GDBManager.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GPalette.h"
#include "GEnvManager.h"
#include "GAsyncTask.h"
#include "GServer.h"
#include "GAsyncTask_MoveGameServer.h"
#include "GFieldInfoMgr.h"
#include "GSharedField.h"
#include "GMasterServerFacade.h"
#include "GPlayerFieldDynamic.h"
#include "GPlayerFieldMasterServer.h"
#include "GActorCooltimeChecker.h"
#include "SCharacterInfoChecker.h"
#include "GBuffInfo.h"
#include "GPlayerGameGuard.h"
#include "GConfig.h"
#include "GServerGateRouter.h"
#include "GClientFieldRouter.h"
#include "GDBTaskCharData.h"
#include "GCommandCenter.h"
#include "CCommandTable_Login.h"
#include "GMoveServerRouter.h"
#include "GClientMoveServerRouter.h"
#include "GAsyncTask_MoveLoginServer.h"
#include "GAsyncTaskFactory.h"
#include "CCommandResultTable.h"


GPlayerObject::GPlayerObject(const MUID& uid)
: m_UID(uid)
, m_pMyEntity(NULL)
, m_bCreated(false)
, m_nState(POS_SELECT_CHAR)
, m_nUIID(0)
, m_isSetStatIndex(false)
, m_pGameGuard(NULL)
, m_bForceDisconnect(false)
{
	;
}

GPlayerObject::~GPlayerObject()
{
	SAFE_DELETE(m_pGameGuard);
}

void GPlayerObject::Create()
{
	_ASSERT(m_pMyEntity==NULL);
	m_pMyEntity = new GEntityPlayer();
	m_pMyEntity->Create(m_UID);

	if (GConfig::m_bGGEnable)
	{
		m_pGameGuard = new GPlayerGameGuard(m_pMyEntity);
	}
	
	m_bCreated = true;
}

void GPlayerObject::Destroy()
{
	if (!m_bCreated) return;

	m_bCreated = false;

	_ASSERT(NULL != m_pMyEntity);
	if (NULL == m_pMyEntity) return;
	
	PFI_B(5741, "GPlayerObject::Destroy()");
	
	m_pMyEntity->Destroy();
	m_pMyEntity->Drop();
	m_pMyEntity = NULL;

	PFI_E(5741);
}

void GPlayerObject::InitAccountInfo(const unsigned int nAID, const wstring& strUserID, const bool bNewAcc)
{
	m_AccountInfo.nAID = nAID;
	m_AccountInfo.bNewAcc = bNewAcc;
	wcsncpy_s(m_AccountInfo.szUserID, strUserID.c_str(), strUserID.length());	
}

bool GPlayerObject::SerializeSelectCharacter( int nCID, bool bReload )
{
	_ASSERT(m_pMyEntity!=NULL);
	_ASSERT(nCID > 0);

	if (m_AccountInfo.nAID <= 0) return false;
	if (nCID <= 0) return false;
	if (!m_pMyEntity) return false;

	UIID nUIID = m_pMyEntity->GetUIID();

	GDBT_CHAR_SERIALIZE data(m_AccountInfo.nAID, GetUID(), nUIID, nCID, bReload);

	if (!gsys.pDBManager->CharSerialize(data))
	{
		return false;
	}

	return true;
}

void GPlayerObject::AddStatIndex(const PmStatIndex* statIndex, int nPCCafeID)
{
	if (statIndex != NULL)
	{
		m_statIndex = *statIndex;
		m_nPCCafeID = nPCCafeID;
		m_isSetStatIndex = true;
	}
}

PmStatIndex* GPlayerObject::GetStatIndex()
{
	return &m_statIndex;
}

int GPlayerObject::GetPCCafeID()
{
	return m_nPCCafeID;
}

bool GPlayerObject::IsExistStatIndex()
{
	return m_isSetStatIndex;
}

void GPlayerObject::ReadyMoveGameServer(int nMoveTargetGameServerID, GGateInfo& refGateInfo)
{
	if (m_nState != POS_INWORLD)
		return;

	if (m_pMyEntity)
	{
		MUID uidMoveGameServerTask = m_pMyEntity->GetPlayerField().GetFieldMasterServer().ReadyMoveServer(nMoveTargetGameServerID, refGateInfo);
		SetMoveGameServerAsyncTaskUID(uidMoveGameServerTask);
	}

	OnReadyMoveGameServer();
}

void GPlayerObject::GetSetMoveGameServer(TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo)
{
	if (m_nState != POS_READY_MOVE_GAME_SERVER)	return;

	if (m_pMyEntity)
	{
		m_pMyEntity->GetPlayerField().GetFieldMasterServer().SetMoveServerDestInfo(pLoginGameServerInfo);
	}
	OnMoveGameServer();
}

void GPlayerObject::CancelMoveGameServer(void)
{
	// 서버이동 테스크 중지
	GAsyncTaskManager* pAsyncTaskManager = gsys.pServer->GetAsyncTaskManager();
	GAsyncTask* pTask = pAsyncTaskManager->FindTask(m_uidMoveGameServerTask);
	
	if (pTask != NULL)
	{
		pTask->Done();
	}


	// 마스터서버에 서버이동 취소 요청 - 플레이어의 상태를 바꾸고, 목적 서버의 예약을 푼다.	
	GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
	pServerRouter->RequestCancelMoveGameServer(m_UID);


	// 클라이언트에 서버이동 취소 통보
	GClientFieldRouter router;
	router.CancelChangeField(m_UID);

	
	// 상태변경
	m_nState = POS_INWORLD;
}

void GPlayerObject::GoMoveGameServer(void)
{
	if (m_nState != POS_MOVE_GAME_SERVER) return;

	if (m_pMyEntity)
	{
		m_pMyEntity->GetPlayerField().GetFieldMasterServer().GoMoveServer();	
	}
}


void GPlayerObject::ReadyMoveLoginServer()
{
	if (m_nState != POS_INWORLD)
		return;

	OnReadyMoveLoginServer();

	GAsyncTaskManager* pAsyncTaskManager = gsys.pServer->GetAsyncTaskManager();
	GAsyncTask_MoveLoginServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveLoginServerTask(pAsyncTaskManager);
	pNewAsyncTask->Start(m_UID);

	MUID uidMoveLoginServerTask = pNewAsyncTask->GetUID();
	SetMoveLoginServerAsyncTaskUID(uidMoveLoginServerTask);
}

void GPlayerObject::SetMoveLoginServerConnectionKey(MUID uidConnectionKey)
{
	if (m_nState != POS_READY_MOVE_LOGIN_SERVER) return;

	m_uidMoveLoginServerConnectionKey = uidConnectionKey;

	OnMoveLoginServer();
}

void GPlayerObject::CancelMoveLoginServer(int nResult)
{
	if (m_nState != POS_READY_MOVE_LOGIN_SERVER) return;

	// 서버이동 테스크 중지
	GAsyncTaskManager* pAsyncTaskManager = gsys.pServer->GetAsyncTaskManager();
	GAsyncTask* pTask = pAsyncTaskManager->FindTask(m_uidMoveLoginServerTask);
	if (pTask != NULL)
	{
		pTask->Done();
	}


	// 마스터서버에 서버이동 취소 요청 - 플레이어의 상태를 바꾸고, 목적 서버의 예약을 푼다.	
	GMoveServerRouter* pServerRouter = gsys.pMasterServerFacade->GetMoveServerRouter();
	pServerRouter->RouteCancelMoveLoginServer(m_UID);


	// 클라이언트에 로그린서버로 이동 취소 통보
	GClientMoveServerRouter router;
	router.RouteStartMoveLoginServerRes(m_UID, nResult, MUID::ZERO);

	// 상태변경
	m_nState = POS_INWORLD;
}

void GPlayerObject::GoMoveLoginServer(int nResult)
{
	if (m_nState != POS_MOVE_LOGIN_SERVER) return;

	MUID uidConnectionKey = MUID::ZERO;
	if (nResult == CR_SUCCESS)
		uidConnectionKey = m_uidMoveLoginServerConnectionKey;

	GClientMoveServerRouter router;
	router.RouteStartMoveLoginServerRes(m_UID, nResult, uidConnectionKey);
}

bool GPlayerObject::IsMovingServer(void)
{
	if (m_nState == POS_READY_MOVE_GAME_SERVER ||
		m_nState == POS_MOVE_GAME_SERVER ||
		m_nState == POS_READY_MOVE_LOGIN_SERVER ||
		m_nState == POS_MOVE_LOGIN_SERVER) return true;
	return false;
}


void GPlayerObject::Gate(RESPONSE_GATE_TYPE eResponseGateType, const GGateInfo* pGateInfo)
{
	_ASSERT(m_pMyEntity != NULL);

	switch(eResponseGateType)
	{
	case RGT_REMAIN:
		if (m_pMyEntity->GetPlayerField().GetFieldMasterServer().RemainField())
			return;
	case RGT_EXECUTE:
		if (m_pMyEntity->GetPlayerField().GetFieldMasterServer().ExecuteGate(pGateInfo))
			return;
	case RGT_RETRY:
		if (m_pMyEntity->GetPlayerField().GetFieldMasterServer().StartRetryGate(pGateInfo))
			return;
		return;
	}
	m_pMyEntity->GetPlayerField().GetFieldMasterServer().FailGate(pGateInfo);
}

// EntityPlayer도 함께 등록한다.
void GPlayerObject::RegUIID( UIID nUIID )
{
	m_nUIID = nUIID;

	if (m_pMyEntity)
	{
		m_pMyEntity->RegUIID(nUIID);
	}
}

void GPlayerObject::ClearCharFieldInfo()
{
	m_vecCharFieldInfo.clear();
}

void GPlayerObject::AddCharFieldInfo(const SAccountCharFieldInfo& accountCharFieldInfo)
{
	m_vecCharFieldInfo.push_back(accountCharFieldInfo);
}

void GPlayerObject::RemoveCharFieldInfo(size_t nIndex)
{
	if (nIndex < m_vecCharFieldInfo.size())
	{
		m_vecCharFieldInfo.erase(m_vecCharFieldInfo.begin() + nIndex);
	}
}

int GPlayerObject::GetCharFieldInfoCount()
{
	return m_vecCharFieldInfo.size();
}

SAccountCharFieldInfo* GPlayerObject::GetCharFieldInfo(size_t nIndex)
{
	if (nIndex < m_vecCharFieldInfo.size())
	{
		return &m_vecCharFieldInfo[nIndex];
	}

	return NULL;
}

void GPlayerObject::Update( float fDelta )
{
	if (m_pGameGuard)
		m_pGameGuard->Update(fDelta);
}

GPlayerGameGuard* GPlayerObject::GetGameGuard()
{
	return m_pGameGuard;
}