#include "stdafx.h"
#include "GMeleeTalent.h"

void GMeleeTalent::OnStart()
{
	// È÷Æ®Ä¸½¶ Å¸°Ù¸¸ À¯È¿ÇÕ´Ï´Ù. (°­Á¦ °íÁ¤)
	m_pTalentInfo->m_EffectInfo.m_nPoint = CSEffectInfo::POINT_HITCAPSULE;

	GTalent::OnStart();
}
