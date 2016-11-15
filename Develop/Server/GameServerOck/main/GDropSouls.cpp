#include "StdAfx.h"
#include "GDropSouls.h"
#include "GSTLUtil.h"
#include "GDropSoul.h"
#include "GLootInfo.h"
#include "GEntityNPC.h"
#include "GNPCBParts.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"

GDropSouls::GDropSouls(GEntityNPC* pOwner)
:m_pOwner(pOwner)
{
}

GDropSouls::~GDropSouls(void)
{
	SAFE_DELETE_VECTOR(m_vecDropSoul);
}

void GDropSouls::Clear()
{
	SAFE_DELETE_VECTOR(m_vecDropSoul);
}

bool GDropSouls::IsEmpty()
{
	return m_vecDropSoul.empty();
}

bool GDropSouls::IsAuthorizedCID(int nCID)
{
	for each (GDropSoul* pDropSoul in m_vecDropSoul)
	{
		if (true == pDropSoul->IsAuthorizedCID(nCID)) return true;
	}

	return false;
}


void GDropSouls::Drop(int nBeneficiaryCID, GLootInfo* pLootInfo )
{
	vector<int> vecBeneficiaryCID;
	vecBeneficiaryCID.push_back(nBeneficiaryCID);

	Drop(vecBeneficiaryCID, pLootInfo);
}

void GDropSouls::Drop(const vector<int>& vecBeneficiaryCID, GLootInfo* pLootInfo)
{
	int nBeneficiarySize = static_cast<int>(vecBeneficiaryCID.size());
	for each (LOOT_SOUL_INFO* pLootSoulInfo in pLootInfo->m_vecLootSouls)
	{
		if (NULL == pLootSoulInfo) continue;

		int nPickedBeneficiaryCID = vecBeneficiaryCID[RandomNumber(0, nBeneficiarySize-1)];
		GEntityPlayer* pPickedBeneficiary = gmgr.pPlayerObjectManager->GetEntity(nPickedBeneficiaryCID);
		if (NULL == pPickedBeneficiary) continue;

		float fRate = pLootSoulInfo->m_fRate;
		fRate += (m_pOwner->GetNPCBParts() && true == m_pOwner->GetNPCBParts()->IsBroken(1)) ? pLootSoulInfo->m_fBPart1Rate : 0.0f;
		fRate += (m_pOwner->GetNPCBParts() && true == m_pOwner->GetNPCBParts()->IsBroken(2)) ? pLootSoulInfo->m_fBPart2Rate : 0.0f;
		fRate += (m_pOwner->GetNPCBParts() && true == m_pOwner->GetNPCBParts()->IsBroken(3)) ? pLootSoulInfo->m_fBPart3Rate : 0.0f;
		fRate += (m_pOwner->GetNPCBParts() && true == m_pOwner->GetNPCBParts()->IsBroken(4)) ? pLootSoulInfo->m_fBPart4Rate : 0.0f;

		GPercentDice dice;
		if (!dice.Dice(fRate)) 
			continue;	// È®·ü ½ÇÆÐ

		GDropSoul* pNewDropSoul = new GDropSoul();
		pNewDropSoul->nSoulType = pLootSoulInfo->m_nType;
		pNewDropSoul->nAmount = RandomNumber(pLootSoulInfo->m_nMin, pLootSoulInfo->m_nMax);
		pNewDropSoul->nAuthorizedCID = pPickedBeneficiary->GetCID();
		m_vecDropSoul.push_back(pNewDropSoul);
	}
}

int GDropSouls::GetQuantity(int nCID, SOUL_TYPE nSoulType)
{
	int nQuantity = 0;

	for each (GDropSoul* pDropSoul in m_vecDropSoul)
	{
		if (false == pDropSoul->IsAuthorizedCID(nCID)) continue;
		if (pDropSoul->nSoulType != nSoulType) continue;

		nQuantity += pDropSoul->nAmount;
	}

	return nQuantity;
}