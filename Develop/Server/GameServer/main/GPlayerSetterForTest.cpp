#include "StdAfx.h"
#include "GPlayerSetterForTest.h"
#include "GEntityPlayer.h"
#include "GPlayerEmblem.h"
#include "GCheatChecker.h"
#include "GPlayerPVPArea.h"

#define _SET(original, mock)	SAFE_DELETE(original); original = mock;

GPlayerSetterForTest::GPlayerSetterForTest(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
}

void GPlayerSetterForTest::SetPlayerEmblem( GPlayerEmblem* pPlayerEmblem )
{
	_SET(m_pOwner->m_pPlayerEmblem, pPlayerEmblem);
}

void GPlayerSetterForTest::SetPlayerCheatChecker( GCheatChecker* pChecker )
{
	_SET(m_pOwner->m_pCheatCheck, pChecker);
}

void GPlayerSetterForTest::SetPlayerPVPArea( GPlayerPVPArea* pPVPArea )
{
	_SET(m_pOwner->m_pPvPArea, pPVPArea);
}
