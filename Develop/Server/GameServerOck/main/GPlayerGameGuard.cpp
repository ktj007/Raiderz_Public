#include "StdAfx.h"
#include "GPlayerGameGuard.h"
#include "ggsrv26.h" // 게임가드
#include "GConfig.h"
#include "GCommandCenter.h"
#include "GGlobal.h"
#include "GServer.h"
#include "CCommandTable.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GGameGuard.h"
#include "GEntityPlayer.h"

GPlayerGameGuard::GPlayerGameGuard(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_pCSA(NULL)
{
	m_pCSA = new CCSAuth2();
	m_pCSA->Init();
	
	m_rgrTick.SetTickTime((float)GConfig::m_nGGAuthInterval); // 권장 틱 체크 : 5분
	m_rgrTick.Start();
}

GPlayerGameGuard::~GPlayerGameGuard(void)
{
	SAFE_DELETE(m_pCSA);
}

void GPlayerGameGuard::AuthAnswer( GG_AUTH_DATA* paaData )
{
	memcpy(&m_pCSA->m_AuthAnswer, paaData, sizeof(GG_AUTH_DATA));
	DWORD dwRet = m_pCSA->CheckAuthAnswer();

	if (IsAuthEnable() &&
		dwRet != ERROR_SUCCESS)
	{
		mlog("np_error(@interval auth: server recv). code: %d\n", dwRet);
		Disconnect();	
	}
}

void GPlayerGameGuard::Update( float fDelta )
{
	if (!IsAuthEnable())
		return;

	if (!GConfig::m_bGGEnable)
		return;

	if (!m_rgrTick.IsReady(fDelta))
		return;

	GG_AUTH_DATA ggData;
	DWORD dwRet = m_pCSA->GetAuthQuery();

	if (IsAuthEnable())
	{
		if (dwRet != ERROR_SUCCESS)
		{
			mlog("np_error(@interval auth: send client). code: %d\n", dwRet);
			Disconnect();
		}

		memcpy(&ggData, &m_pCSA->m_AuthQuery, sizeof(GG_AUTH_DATA));

		MCommand* pNewCommand = MakeNewCommand(MC_GG_AUTH_REQ, 1, 
			NEW_SINGLE_BLOB(&ggData, sizeof(GG_AUTH_DATA)));
	
		m_pOwner->RouteToMe(pNewCommand);
	}
}

void GPlayerGameGuard::Disconnect()
{
	VALID(m_pOwner);
	gsys.pServer->Disconnect(m_pOwner->GetUID());
}

bool GPlayerGameGuard::IsAuthEnable()
{
	return gsys.pGameGuard->IsEnable();
}