#include "stdafx.h"
#include "GServerPartyRouter.h"
#include "CTransData.h"
#include "GEntityPlayer.h"
#include "GParty.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GPlayerObject.h"

void GServerPartyRouter::MakePartyMemberInfo(const GParty* pParty, MUID uidMember, TD_PARTY_MEMBER* pPartyMember, vector<int>* pVecBuff)
{
	_ASSERT(pParty != NULL);
	_ASSERT(pPartyMember != NULL);
	_ASSERT(pVecBuff != NULL);

	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidMember);
	
	if (pPlayerObject == NULL || pPlayerObject->IsLogout())
	{
		wstring strMemberName = pParty->FindName(uidMember);
		wcsncpy_s(pPartyMember->szName, strMemberName.c_str(), _TRUNCATE);

		pPartyMember->nStatusFlag = 0;
		SetBitSet(pPartyMember->nStatusFlag, UPS_OFFLINE);

		pPartyMember->nHP = 0;
		pPartyMember->nEN = 0;
		pPartyMember->nSTA = 0;
		pPartyMember->nLevel = 0;

		pVecBuff->clear();
	}
	else
	{
		GEntityPlayer* pMember = pPlayerObject->GetEntity();

		pPartyMember->m_uidPlayer = pMember->GetUID();
		wcsncpy_s(pPartyMember->szName, pMember->GetName(), _TRUNCATE);
			
		if (pMember->IsDead())
		{
			pPartyMember->nStatusFlag = 0;
			SetBitSet(pPartyMember->nStatusFlag, UPS_DEAD);

			pPartyMember->nHP = 0;
			pPartyMember->nEN = 0;
			pPartyMember->nSTA = 0;
			pPartyMember->nLevel = 0;

			pVecBuff->clear();
		}
		else
		{
			pPartyMember->nStatusFlag = 0;
			pPartyMember->nLevel = pMember->GetLevel();

			pMember->GetSimpleStatus(pPartyMember->nHP, pPartyMember->nEN, pPartyMember->nSTA);
			pMember->GetBuffList(*pVecBuff);			
		}
	}
}
