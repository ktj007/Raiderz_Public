#include "stdafx.h"
#include "PPostOfficeSystem.h"
#include "PCmdRouter_Mail.h"
#include "SDef.h"
#include "SProxyPlayer.h"
#include "PGameServerObject.h"
#include "PGlobal.h"
#include "PGameServerObjectManager.h"
#include "PBounceMail.h"
#include "PSystem.h"
#include "SProxyPlayerManager.h"
#include "PServer.h"

PPostOfficeSystem::PPostOfficeSystem()
{
	m_pRouter = new PCmdRouter_Mail();
}

PPostOfficeSystem::~PPostOfficeSystem()
{
	SAFE_DELETE(m_pRouter);

	while(!m_queueBounceMail.empty())
	{
		SAFE_DELETE(m_queueBounceMail.front());
		m_queueBounceMail.pop();
	}
}

void PPostOfficeSystem::SetNewRouter(PCmdRouter_Mail* pRouter)
{
	VALID(pRouter != NULL);

	if (m_pRouter != NULL)
	{
		SAFE_DELETE(m_pRouter);
	}

	m_pRouter = pRouter;
}

void PPostOfficeSystem::RouteNewMailToReceiver(const SProxyPlayer* pPlayer, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo)
{
	VALID(pPlayer != NULL);
	VALID(pInfo != NULL);

	
	// 플레이어가 있는 게임서버 찾기
	int nGameServerID = pPlayer->GetGameServerID();
	
	PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(nGameServerID);
	
	if (pGameServer == NULL)
	{
		// 게임서버를 찾을 수 없다면, 통보하지 않는다. 
		mlog("[Warning] Not find GameServer(ID=%d) for mail notify.", nGameServerID);
		return;
	}


	// 통보하기
	MUID uidGameServer = pGameServer->GetUID();
	CID nCID = pPlayer->GetCID();	
	m_pRouter->TransNewMailReq(uidGameServer, nCID, pInfo);
}

void PPostOfficeSystem::RetryNewMailToReceiver(const SProxyPlayer* pPlayer, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo)
{
	VALID(pPlayer != NULL);
	VALID(pInfo != NULL);


	// 반송 메일 준비
	uint32 nNowTime = gsys.pSystem->GetNowTime();
	CID nCID = pPlayer->GetCID();
	PBounceMail* pMail = new PBounceMail(nNowTime, nCID, pInfo);

	m_queueBounceMail.push(pMail);
}

void PPostOfficeSystem::Update(void)
{
	// 반송 메일 시간 검사
	while(!m_queueBounceMail.empty())
	{		
		PBounceMail* pMail = m_queueBounceMail.front();

		// 처리 시간 안됬으면 처리하지 않는다.
		uint32 nNowTime = gsys.pSystem->GetNowTime();		

		if (!pMail->IsReady(nNowTime))
		{
			return;
		}


		// 이하 결과에 관계없이 스코프를 벗어나면, pMail을 제거한다.
		auto_ptr<PBounceMail> mail(pMail);
		m_queueBounceMail.pop();


		// 통보 플레이어를 확인한다.
		CID nCID = pMail->GetCID();		
		SProxyPlayerManager* pPlayerManager = gsys.pServer->GetPlayerManager();
		SProxyPlayer* pPlayer = pPlayerManager->FindPlayer(nCID);

		if (pPlayer == NULL)
		{
			// 플레이어는 게임을 떠났다.
			return;
		}


		// 게임서버를 확인한다.
		int nGameServerID = pPlayer->GetGameServerID();
		PGameServerObject* pGameServer = gmgr.pGameServerObjectManager->Find(nGameServerID);

		if (pGameServer == NULL)
		{
			// 게임서버가 잘못되었다.
			return;
		}		


		// 재 통보
		MUID uidGameServer = pGameServer->GetUID();
		TD_MAIL_MAILBOX_MAIL_INFO info;		
		pMail->Export(&info);

		m_pRouter->TransNewMailReq(uidGameServer, nCID, &info);
	}
}
