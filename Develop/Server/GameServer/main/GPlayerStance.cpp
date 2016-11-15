#include "StdAfx.h"
#include "GPlayerStance.h"
#include "CSChrInfo.h"
#include "CCommandTable.h"
#include "GCommand.h"
#include "GEntityPlayer.h"


GPlayerStance::GPlayerStance(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
}

GPlayerStance::~GPlayerStance(void)
{
}

void GPlayerStance::Change( CHAR_STANCE nStance )
{
	m_nStance = nStance;
}

void GPlayerStance::OnUnarmed()
{
	Change(CS_NORMAL);

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_CHANGE_STANCE, 2, NEW_USHORT(m_pOwner->GetUIID()), NEW_CHAR(CS_NORMAL));
	m_pOwner->RouteToThisCell(pNewCommand);
}
