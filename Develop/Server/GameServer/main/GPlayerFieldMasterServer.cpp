#include "stdafx.h"
#include "GPlayerFieldMasterServer.h"
#include "CCommandTable_Login.h"
#include "GPlayerField.h"
#include "GFieldInfoMgr.h"
#include "GCommandCenter.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GServer.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GMasterServerFacade.h"
#include "GSharedFieldMaster.h"
#include "GASyncTask_MoveGameServer.h"
#include "GAsyncTaskFactory.h"
#include "GServerGateRouter.h"
#include "GMsgCommandFacade.h"
#include "CCommandResultTable.h"
#include "GPlayerFieldShared.h"
#include "GPlayerFieldDynamic.h"
#include "GPlayerFieldGateRetryer.h"
#include "GFieldMgr.h"
#include "GClientFieldRouter.h"


GPlayerFieldMasterServer::GPlayerFieldMasterServer( GEntityPlayer* pOwner, GPlayerField* pPlayerField ) : m_pOwner(pOwner), m_pPlayerField(pPlayerField)
{

}

bool GPlayerFieldMasterServer::AskGateSharedField( int nFieldID, vec3 vPos, vec3 vDir, int nChannelID /*= INVALID_CHANNELID*/ )
{
	if (!MakeDestGateInfo(nFieldID, vPos, vDir, nChannelID))
		return false;
	
	GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
	pServerRouter->AskGateSharedField(m_pOwner->GetUID(), &m_DestGateInfo);
	
	return true;
}

bool GPlayerFieldMasterServer::AskGateDynamicField(int nFieldID, vec3 vPos, vec3 vDir, int nFieldGroupID, MUID uidFieldGroup, DYNAMIC_FIELD_TYPE eType, bool isSingleType)
{
	if (!MakeDestGateInfo_FieldGroup(nFieldID, vPos, vDir, uidFieldGroup))
		return false;
	
	GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
	pServerRouter->AskGateDynamicField(m_pOwner->GetUID(), &m_DestGateInfo, nFieldGroupID, eType, isSingleType);

	return true;
}

bool GPlayerFieldMasterServer::AskGateChangeChannel(int nFieldID, vec3 vPos, vec3 vDir, int nChannelID)
{
	if (!MakeDestGateInfo(nFieldID, vPos, vDir, nChannelID))
		return false;

	GServerGateRouter* pServerRouter = gsys.pMasterServerFacade->GetGateRouter();
	pServerRouter->AskGateChangeChannel(m_pOwner->GetUID(), &m_DestGateInfo);

	return true;
}

MUID GPlayerFieldMasterServer::ReadyMoveServer(int nTargetGameServerID, GGateInfo& refGateInfo)
{
	if (!MakeDestGateInfo(refGateInfo)) return MUID::Invalid();

	GAsyncTaskManager* pAsyncTaskManager = gsys.pServer->GetAsyncTaskManager();
	GAsyncTask_MoveGameServer* pNewAsyncTask = GAsyncTaskFactory::NewMoveGameServerTask(pAsyncTaskManager);
	pNewAsyncTask->Start(m_pOwner->GetUID(), nTargetGameServerID);

	return pNewAsyncTask->GetUID();
}

void GPlayerFieldMasterServer::SetMoveServerDestInfo( TD_LOGIN_GAME_SERVER_INFO* pLoginGameServerInfo )
{
	wcsncpy_s(m_LoginGameServerInfo.strHostName, pLoginGameServerInfo->strHostName, _TRUNCATE);
	m_LoginGameServerInfo.nPort = pLoginGameServerInfo->nPort;
	m_LoginGameServerInfo.uidConnectionKey = pLoginGameServerInfo->uidConnectionKey;
}

void GPlayerFieldMasterServer::GoMoveServer( void )
{
	MCommand* pNewCommand = MakeNewCommand(MC_COMM_MOVE_GAME_SERVER, m_pOwner->GetUID(),
		1,
		NEW_BLOB(&m_LoginGameServerInfo, sizeof(m_LoginGameServerInfo), 1)
		);

	gsys.pCommandCenter->PostCommand(pNewCommand);
}

bool GPlayerFieldMasterServer::MakeDestGateInfo(int nFieldID, vec3 vPos, vec3 vDir, int nChannelID)
{
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	if (pFieldInfo == NULL)	return false;

	m_DestGateInfo.nFieldID = nFieldID;
	m_DestGateInfo.vPosition = vPos;
	m_DestGateInfo.vDirection = vDir;

	if (nChannelID == INVALID_CHANNELID)
	{
		// 현재채널 설정
		GField* pField = m_pOwner->GetField();
		m_DestGateInfo.nChannelID = ::GetChannelID(pField);
	}
	else
	{
		// 요청채널 설정
		m_DestGateInfo.nChannelID = nChannelID;
	}

	m_DestGateInfo.bDynamicField = false;
	return true;
}

bool GPlayerFieldMasterServer::MakeDestGateInfo_FieldGroup(int nFieldID, vec3 vPos, vec3 vDir, MUID uidFieldGroup)
{
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
	if (pFieldInfo == NULL)	return false;

	m_DestGateInfo.nFieldID = nFieldID;
	m_DestGateInfo.vPosition = vPos;
	m_DestGateInfo.vDirection = vDir;
	m_DestGateInfo.bDynamicField = true;
	m_DestGateInfo.uidDynamicFieldGroup = uidFieldGroup;
	m_DestGateInfo.nChannelID = INVALID_CHANNELID;

	return true;
}

bool GPlayerFieldMasterServer::MakeDestGateInfo(GGateInfo& refGateInfo)
{
	GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(refGateInfo.nFieldID);
	if (pFieldInfo == NULL)	return false;

	m_DestGateInfo = refGateInfo;
	return true;
}


bool GPlayerFieldMasterServer::ExecuteGate(const GGateInfo* pGateInfo)
{
	RVALID_RET(NULL != pGateInfo, false);
	if (pGateInfo->bDynamicField)
	{
		m_pPlayerField->SetDynamicFieldGroupUID(pGateInfo->uidDynamicFieldGroup);
		return m_pPlayerField->GetFieldDynamic().GateExecute(pGateInfo->uidDynamicFieldGroup, pGateInfo->nFieldID, pGateInfo->vPosition, pGateInfo->vDirection);
	}
	else
	{
		GSharedField* pSharedField = gmgr.pFieldMgr->GetSharedField(pGateInfo->nFieldID, pGateInfo->nChannelID);
		if (NULL == pSharedField)
		{
			mlog3("Failed! GPlayerFieldMasterServer::ExecuteGate(), pSharedField is NULL! FieldID(%d), ChannelID(%d)\n", pGateInfo->nFieldID, pGateInfo->nChannelID);
			return false;
		}
		return m_pPlayerField->GetFieldShared().GateExecute(pSharedField, pGateInfo->vPosition, pGateInfo->vDirection);
	}
	return false;
}

bool GPlayerFieldMasterServer::StartRetryGate(const GGateInfo* pGateInfo)
{
	RVALID_RET(NULL != pGateInfo, false);
	m_pPlayerField->GetFieldGateRetryer().StartRetryer(pGateInfo->nFieldID, pGateInfo->vPosition, pGateInfo->vDirection);
	return true;
}

bool GPlayerFieldMasterServer::RemainField()
{	
	gsys.pMsgCommandFacade->RouteSystemMsg(m_pOwner->GetUID(), CR_FAIL_ENTER_FIELD);
	return true;
}

void GPlayerFieldMasterServer::FailGate(const GGateInfo* pGateInfo)
{
	VALID(NULL != pGateInfo);

	GClientFieldRouter router;
	router.CancelChangeField(m_pOwner->GetUID());

	// 필드 이동 실패
	dlog("Gate Failure! (FieldID=%d, ChannelID=%d, FieldGroupUID=%I64u)\n"
		, pGateInfo->nFieldID, pGateInfo->nChannelID, pGateInfo->uidDynamicFieldGroup);
}

