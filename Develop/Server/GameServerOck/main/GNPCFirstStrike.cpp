#include "StdAfx.h"
#include "GNPCFirstStrike.h"
#include "GConst.h"

GNPCFirstStrike::GNPCFirstStrike(GEntityNPC* pOwner)
: m_pOwner(pOwner)
, m_bEnableSpawnDelay(true)
, m_bSpawnDelay(false)
{
	m_rgrFirstEnable.SetTickTime(GConst::NPC_FIRSTSTRIKE_DELAY);
	m_rgrFirstEnable.Start();
}

GNPCFirstStrike::~GNPCFirstStrike(void)
{
}

void GNPCFirstStrike::Update( float fDelta )
{
	PFC_THISFUNC;
	PFI_BLOCK_THISFUNC(4003);

	if (m_bEnableSpawnDelay)
	{
		if (m_rgrFirstEnable.IsReady(fDelta))
		{
			m_bEnableSpawnDelay = false;
			m_bSpawnDelay = true;
		}
	}
}

bool GNPCFirstStrike::IsEnable() const
{
	if (!m_bSpawnDelay)
		return false;

	return true;
}

void GNPCFirstStrike::SetEnableSpawnDelay( bool var )
{
	m_bEnableSpawnDelay = var;
	// 스폰딜레이를 사용하지 않으면 즉시 선제 공격 가능
	m_bSpawnDelay = !var;
}