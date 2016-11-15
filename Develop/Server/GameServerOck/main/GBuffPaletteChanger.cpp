#include "StdAfx.h"
#include "GBuffPaletteChanger.h"
#include "GBuffInfo.h"
#include "GEntityPlayer.h"
#include "GPaletteItem.h"

void GBuffPaletteChanger::OnGain( MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo )
{
	if (!pTarget->IsPlayer())
		return; // 플레이어만 적용 가능

	if (!pBuffInfo->m_bUseChangePalette)
		return; // 팔레트 변경하지 않음

	GEntityPlayer* pTargetPlayer = ToEntityPlayer(pTarget);

	VEC_PALETTE_ITEM& vecRealPalettes = 
		pTargetPlayer->GetPalette().GetContainer();
	m_vecOldPalettes = vecRealPalettes;

	for (int i=0; i<PALETTESLOT_MAX; ++i)
	{
		GPaletteItem item;
		item.m_nType = PIT_TALENT;
		item.m_nItemIDorTalentID = pBuffInfo->m_arrPalettes[i];
		vecRealPalettes[i] = item;
	}

	pTargetPlayer->GetPalette().Lock();
}

void GBuffPaletteChanger::OnLost( MUID uidUser, GEntitySync* pTarget, GBuffInfo* pBuffInfo, bool bRoute )
{
	if (!pTarget->IsPlayer())
		return; // 플레이어만 적용 가능

	if (!pBuffInfo->m_bUseChangePalette)
		return; // 팔레트 변경하지 않음

	GEntityPlayer* pTargetPlayer = ToEntityPlayer(pTarget);
	pTargetPlayer->GetPalette().Unlock();
	pTargetPlayer->GetPalette().GetContainer() = m_vecOldPalettes;
}
