#include "StdAfx.h"
#include "GASyncTask_LazyLoginMoveGameServer.h"
#include "GLoginLogicMoveGameServer.h"
#include "GLoginLogic.h"
#include "GGlobal.h"
#include "GSystem.h"
#include "GServer.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"


const float GASyncTask_LazyLoginMoveGameServer::TIMEOUT_SEC = 300; ///< 300УЪ/5Ка

GASyncTask_LazyLoginMoveGameServer::GASyncTask_LazyLoginMoveGameServer(MUID uid, uint32 nNowTime)
: GAsyncTask(uid, nNowTime)
{
	const float UPDATE_SEC = 1.0f;
	m_rgltUpdate.SetTickTime(UPDATE_SEC);
	m_rgltTimeout.SetTickTime(TIMEOUT_SEC);
	m_nLastTime = gsys.pSystem->GetNowTime();
}

void GASyncTask_LazyLoginMoveGameServer::Init(MUID uidPlayerSession, MUID uidConnectionKey, MUID uidRealPlayerUID, bool bOnPmang/*=false*/, wstring strPmangStatIndex/*=L""*/)
{
	m_uidPlayerSession	= uidPlayerSession;
	m_uidConnectionKey	= uidConnectionKey;
	m_uidRealPlayerUID	= uidRealPlayerUID;
	m_bOnPmang			= bOnPmang;
	m_strPmangStatIndex	= strPmangStatIndex;
}

void GASyncTask_LazyLoginMoveGameServer::OnUpdate()
{
	float fTick = (gsys.pSystem->GetNowTime() - m_nLastTime) * 0.001f;
	m_nLastTime = gsys.pSystem->GetNowTime();

	if (m_rgltTimeout.IsReady(fTick))
	{
		OnTimeout();
		Done();
		return;
	}

	if (!m_rgltUpdate.IsReady(fTick))
		return;

	if (GLoginLogicMoveGameServer::IsExistPastPlayerObject(m_uidRealPlayerUID))
		return;

	GLoginLogic loginLogic;
	loginLogic.MoveGameServer(m_uidPlayerSession, m_uidConnectionKey, m_bOnPmang, m_strPmangStatIndex);

	Done();
}

void GASyncTask_LazyLoginMoveGameServer::OnTimeout()
{
	GPlayerObject* pPastPlayer = gmgr.pPlayerObjectManager->GetPlayer(m_uidRealPlayerUID);
	if (NULL == pPastPlayer)
		return;
	
	mlog3("Error! GASyncTask_LazyLoginMoveGameServer::OnTimeout(), PastPlayerState=%d, IsMovingServer=%d\n"
		, pPastPlayer->GetState(), (int)pPastPlayer->IsMoveGameServer());
}
