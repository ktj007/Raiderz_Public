#include "StdAfx.h"
#include "GPlayerDoing.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GTradeSystem.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "GLootSystem.h"
#include "GPlayerSystem.h"
#include "GTradeCanceler.h"
#include "GLootEnder.h"


GPlayerDoing::GPlayerDoing(GEntityPlayer* pOwnerPlayer)
: m_pOwnerPlayer(pOwnerPlayer)
, m_eDoing(CD_NORMAL)
{
}

bool GPlayerDoing::IsBusy()
{
	return (CD_NORMAL != m_eDoing);
}

bool GPlayerDoing::IsEndableBusy()
{
	if (CD_CUTSCENING == m_eDoing) return false;
	if (CD_DYEING == m_eDoing) return false;
	if (CD_ENCHANTING == m_eDoing) return false;

	return true;
}

void GPlayerDoing::ChangeDoing(CHAR_DOING eDoing)
{
	m_eDoing = eDoing;
}

void GPlayerDoing::EndDoing()
{
	if (IsNowTrading())
	{
		gsys.pTradeSystem->GetCanceler().Cancel(m_pOwnerPlayer);
	}

	if (IsNowInteracting())
	{
		gsys.pInteractionSystem->GetNPCInteractor().End(m_pOwnerPlayer);
	}

	if (IsNowLooting())
	{
		gsys.pLootSystem->GetEnder().End(m_pOwnerPlayer);
	}

	if (IsNowEnchanting())
	{
		ChangeDoing(CD_NORMAL);
	}

	if (IsNowDyeing())
	{
		ChangeDoing(CD_NORMAL);
	}
}
