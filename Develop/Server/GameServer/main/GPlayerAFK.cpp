#include "StdAfx.h"
#include "GPlayerAFK.h"
#include "GEntityPlayer.h"
#include "GConst.h"
#include "GCommand.h"
#include "CCommandTable.h"

GPlayerAFK::GPlayerAFK(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
, m_bNowAFK(false)
{
	m_pOwner->AttachObserver(this);

	m_rgrTimeout.SetTickTime(GConst::AFK_TIMEOUT);
	m_rgrTimeout.Start();
}

GPlayerAFK::~GPlayerAFK(void)
{
	m_pOwner->DetachObserver(this);
}

void GPlayerAFK::OnDoSomething()
{
	m_rgrTimeout.Reset();

	if (!m_bNowAFK)
		return; // 이미 자리비움이지 않음

	OnAfkEnd();
}

void GPlayerAFK::OnDie()
{
	m_rgrTimeout.Reset();

	if (!m_bNowAFK)
		return; // 이미 자리비움이지 않음

	OnAfkEnd();
}

void GPlayerAFK::Update( float fDelta )
{
	if (!m_pOwner)
		return;

	if (m_pOwner->IsDead())
		return; // 이미 죽었음

	if (m_bNowAFK)
		return; // 이미 자리비움 중

	if (!m_rgrTimeout.IsReady(fDelta))
		return; // 자리비움 준비시간이 되지 않음

	OnAfkBegin();
}

bool GPlayerAFK::IsNowAFK() const
{
	return m_bNowAFK;
}

void GPlayerAFK::OnAfkBegin()
{
	m_bNowAFK = true;
	m_rgrTimeout.Reset();

	//TestNarration("자리비움 시작");

	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_AFK_BEGIN, 1, 
		NEW_UID(m_pOwner->GetUID()));
	m_pOwner->RouteToThisCell(pNewCmd);
}

void GPlayerAFK::OnAfkEnd()
{
	m_bNowAFK = false;
	m_rgrTimeout.Reset();

	//TestNarration("자리비움 종료");

	MCommand* pNewCmd = MakeNewCommand(MC_ACTION_AFK_END, 1, 
		NEW_UID(m_pOwner->GetUID()));
	m_pOwner->RouteToThisCell(pNewCmd);
}

void GPlayerAFK::Toggle()
{
	if (m_pOwner->IsDead())
		return; // 이미 죽었음

	m_rgrTimeout.Reset();

	m_bNowAFK = !m_bNowAFK;

	if (m_bNowAFK)
		OnAfkBegin();
	else
		OnAfkEnd();
}

#include "GStrings.h"

void GPlayerAFK::TestNarration(const char* pszText)
{
	MCommand* pNewCommand = MakeNewCommand(	MC_NPC_NARRATION, 
		1, 
		NEW_WSTR(MLocale::ConvAnsiToUTF16(pszText).c_str()));

	m_pOwner->RouteToMe(pNewCommand);
}

float GPlayerAFK::GetTimeoutSecond() const
{
	return m_rgrTimeout.GetElapsedTime();
}

