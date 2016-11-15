#include "stdafx.h"
#include "XDuelFightChecker.h"

void XDuelFightChecker::Init( XPlayer* pPlayer )
{
	m_pPlayer = pPlayer;
}

bool XDuelFightChecker::SetDuel( XObject* pAttacker )
{
	if(m_pPlayer == NULL ||	pAttacker == NULL)
		return false;

	// 플레이어만 가능하다.
	if(pAttacker->GetEntityType() != ETID_PLAYER)
		return false;

	XPlayer* pAttackerPlayer = pAttacker->AsPlayer();
	
	// 공격과 피공격에 둘다 셋팅
	pAttackerPlayer->SetDuel(true, m_pPlayer->GetUID());
	m_pPlayer->SetDuel(true, pAttacker->GetUID());

	return true;
}

void XDuelFightChecker::ReSetDuel()
{
	if(m_pPlayer == NULL)
		return;

	m_pPlayer->SetDuel(false, MUID::ZERO);
}

void XDuelFightChecker::Update()
{
	if(m_pPlayer == NULL)
		return;

	// 죽었으면... 끝이다.
	if(m_pPlayer->IsDead())
		ReSetDuel();
}
