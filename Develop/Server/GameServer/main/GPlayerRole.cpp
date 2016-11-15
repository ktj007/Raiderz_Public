#include "StdAfx.h"
#include "GPlayerRole.h"
#include "GEntityPlayer.h"
#include "GPlayerTalent.h"
#include "GGlobal.h"
#include "GTalentInfoMgr.h"

GPlayerRole::GPlayerRole( GEntityPlayer* pOwner )
: m_pOwner(pOwner)
{

}

GPlayerRole::~GPlayerRole(void)
{
}

bool GPlayerRole::IsHealer()
{
	int nPlayerLevel = m_pOwner->GetLevel();
	int nHealTalentPoint = 
		CalcHealTalentPoint(nPlayerLevel);

	return (nHealTalentPoint >= GetNeedHealTalentPoint(nPlayerLevel));
}

int GPlayerRole::CalcHealTalentPoint(int nPlayerLevel ) const
{
	int nHealTalentPoint = 0;

	for each (int each in m_pOwner->GetTalent().GetContainer())
	{
		GTalentInfo* pInfo = gmgr.pTalentInfoMgr->Find(each);
		
		if (!pInfo)
			continue; // 유효하지 않은 탤런트

		if (!pInfo->HasHeal())
			continue; // 힐 속성을 가지고 있지 않은 탤런트

		nHealTalentPoint += pInfo->m_nRank;
	}

	return nHealTalentPoint;
}

int GPlayerRole::GetNeedHealTalentPoint( int nPlayerLevel ) const
{
	int nPlayerHealTier = nPlayerLevel/5 + 1;

	switch (nPlayerHealTier)
	{
		case 1:		return 1;
		case 2:		return 1;
		case 3:		return 2;
		case 4:		return 2;
		case 5:		return 4;
		case 6:		return 4;
		case 7:		return 4;
		case 8:		return 4;
		case 9:		return 4;
		case 10:	return 4;
		case 11:	return 4;
	}

	// 도달하지 않음
	return 9999; 
}
