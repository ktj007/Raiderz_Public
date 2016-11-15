#include "StdAfx.h"
#include "MockDuel.h"

MockDuel::MockDuel( MUID uid, GField* pDuelField, GEntityPlayer* pFighter1, GEntityPlayer* pFighter2 )
: GDuel(uid, pDuelField, pFighter1, pFighter2)
{	
}
MockDuel::~MockDuel(void)
{
}

void MockDuel::SetState( DUEL_STATE nState )
{
	m_nState = nState;
}