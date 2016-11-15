#include "stdafx.h"
#include "PLoginLogic.h"
#include "PLoginRouter.h"
#include "CCommandTable.h"
#include "PGameServerObjectManager.h"
#include "PCheckRunner.h"
#include "PCheckerFactory.h"
#include "PGlobal.h"
#include "PCmdRouter_Chat.h"

PLoginLogic::PLoginLogic()
{
	m_pRouter = NewRouter();
	m_pChecker = NewCheckRunner();
}

PLoginLogic::~PLoginLogic()
{
	SAFE_DELETE(m_pRouter);
	SAFE_DELETE(m_pChecker);
}

PLoginRouter* PLoginLogic::NewRouter(void)
{
	return new PLoginRouter();
}

PCheckRunner* PLoginLogic::NewCheckRunner(void)
{
	return new PCheckRunner();
}

const PLoginRouter* PLoginLogic::GetRouter(void) const
{
	return m_pRouter;
}

void PLoginLogic::RequestLogin(MUID uidClient, const SERVER_LOGIN_DESC& desc) const
{
	// 유효한 요청인지 검사
	m_pChecker->AddChecker(PCheckerFactory::NewCommandVersionChecker(uidClient, desc.nCommandVersion));
	m_pChecker->AddChecker(PCheckerFactory::NewWorldIDChecker(uidClient, desc.nWorldID));

	if (!m_pChecker->Run())		return;
		

	// 서버 오브젝트 생성
	SCommObject* pCommObject = gmgr.pGameServerObjectManager->NewObject(uidClient, desc.nServerID);
	gmgr.pGameServerObjectManager->AddObject(pCommObject);
	
	m_pRouter->RouteLoginResponse(uidClient, CRM_SUCCESS);

	PCmdRouter_Chat chatRouter;
	chatRouter.RouteAllChannelInfo(uidClient);
	chatRouter.RouteAllChatPlayerInfo(uidClient);

	mlog("Game Server Logined : ServerID(%d)\n", desc.nServerID);
}
