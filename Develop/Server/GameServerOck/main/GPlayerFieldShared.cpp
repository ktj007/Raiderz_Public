#include "stdafx.h"
#include "GPlayerFieldMasterServer.h"
#include "GPlayerFieldShared.h"
#include "GPlayerFieldDynamic.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GEntityPlayer.h"
#include "GPlayerField.h"
#include "GEntityPlayer.h"
#include "GFieldInfoMgr.h"
#include "GFieldMgr.h"
#include "GSharedField.h"
#include "GServerGateRouter.h"
#include "GMasterServerFacade.h"
#include "GConfig.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GCommandCenter.h"
#include "SCmdRouter_Login.h"
#include "GPlayerFieldGateRetryer.h"
#include "GClientFieldRouter.h"
#include "CCommandResultTable.h"


GPlayerFieldShared::GPlayerFieldShared(GEntityPlayer* pOwner, GPlayerField* pPlayerField)
: m_nPrepareChannelTime(0)
, m_nPrepareChannelID(INVALID_CHANNELID)
, m_pOwner(pOwner)
, m_pPlayerField(pPlayerField)
{

}

void GPlayerFieldShared::PrepareChangeChannel( CHANNELID nChannelID )
{
	// 예약 시작 시간 기록
	m_nPrepareChannelTime = gsys.pSystem->GetNowTime();
	m_nPrepareChannelID = nChannelID;
	
	// 응답 메세지 전송
	GClientFieldRouter router;
	router.PrepareChangeChannel(m_pOwner, CR_SUCCESS);
}
bool GPlayerFieldShared::IsPreparedChangeChannel()
{
	if (0 != m_nPrepareChannelTime || INVALID_CHANNELID != m_nPrepareChannelID)
		return true;

	return false;
}
void GPlayerFieldShared::ClearPrepareChannelChange()
{
	m_nPrepareChannelTime = 0;
	m_nPrepareChannelID = INVALID_CHANNELID;
}

bool GPlayerFieldShared::GateRequest(int nReqFieldID, int nReqChannelID, vec3 vecDestPos, vec3 vecDestDir)
{
	/// Gate Retryer 가 작동 중이라면 처리하지 않는다.
	if (m_pPlayerField->GetFieldGateRetryer().IsRunRetryer())
		return false;

	/// 알고 있는 필드인지 검사.
	if (NULL == gmgr.pFieldInfoMgr->Find(nReqFieldID))
	{
		mlog("Error! GPlayerFieldShared::GateRequest(), Not Exist FieldInfo. (AID: %I64d , CID: %d , UID(%u:%u) , id: %d)\n"
			, m_pOwner->GetAID(), m_pOwner->GetCID(), m_pOwner->GetUID().High, m_pOwner->GetUID().Low, nReqFieldID);
		return m_pOwner->FailAndRouteSystemMsg(CR_FAIL_SYSTEM_INVALID_FIELD_ID);
	}

	/// 클라이언트에 Gate 준비 커맨드 전송
	SCmdRouter_Login router(gsys.pCommandCenter);
	router.PreparingToLoad(m_pOwner->GetUID(), nReqFieldID);

	if (GConfig::m_bStandAlone)
	{
		// 입장 가능한 필드가 있으면 입장.
		GSharedField* pSharedField = GetEnterableSharedFIeld(nReqFieldID, nReqChannelID);
		if (NULL == pSharedField) return false;
		
		if (!GateExecute(pSharedField, vecDestPos, vecDestDir))
			return false;
	}
	else
	{
		// 마스터 서버에 묻는다
		if (!m_pPlayerField->GetFieldMasterServer().AskGateSharedField(nReqFieldID, vecDestPos, vecDestDir, nReqChannelID))
			return false;
	}

	return true;
}


GSharedField* GPlayerFieldShared::GetEnterableSharedFIeld(int nFieldID, int nChannelID)
{
	GSharedField* pSharedField = NULL;
	
	// 자기 서버에 채널로 찾되, 없으면 채널 상관하지 않는다; 서버이동 최소화를 위해
	pSharedField = gmgr.pFieldMgr->GetSharedField(nFieldID, nChannelID);
	if (NULL == pSharedField)
	{
		pSharedField = gmgr.pFieldMgr->GetEnterableSharedField(nFieldID);
	}

	return pSharedField;
}

bool GPlayerFieldShared::GateExecute(GSharedField* pSharedField, vec3 vecDestPos, vec3 vecDestDir)
{
	RVALID_RET(pSharedField, false);

	m_pPlayerField->GetFieldDynamic().ClearEnterPos();
	if (m_pLisener)
	{
		m_pLisener->OnGateExecute(pSharedField, vecDestPos, vecDestDir);
	}

	return true;
}
