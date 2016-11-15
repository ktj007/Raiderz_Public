#include "stdafx.h"
#include "GTalentHitCommandFactory.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GEntityActor.h"
#include "CCommandTable.h"
#include "GMotionFactorHandler.h"

MCommand* GTalentHitCommandFactory::MakeHit( GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo, unsigned short nDamage, MF_STATE nMFState, int nMFWeight, uint16 nActHitTime, uint32 nFlags )
{
	PFI_BLOCK_THISFUNC(9098);	

	MCommand* pNewCommand = NULL;

	int nTalentID = (pTalentInfo) ? pTalentInfo->m_nID : 0;

	UIID nAttackerUIID = (pAttacker) ? pAttacker->GetUIID() : UIID_INVALID;
	UIID nVictimUIID = (pVictim) ? pVictim->GetUIID() : UIID_INVALID;

	if (nDamage == 0 && 
		nMFWeight == 0 && 
		(nFlags == 0 || CheckBitSet(nFlags, CTR_ZERO_TALENT_DAMAGE)) )
	{
		PFI_B(9099, "GTalentHitCommandFactory::MakeHit-NoMF");
		pNewCommand = MakeNewCommand(MC_ACTION_TALENT_HIT_NO_MF, 3, NEW_USHORT(nVictimUIID), NEW_USHORT(nAttackerUIID), NEW_INT(nTalentID));
		PFI_E(9099);
	}
	else
	{
		TD_TALENT_HIT td_talent_hit;
		td_talent_hit.nAttackerUIID = nAttackerUIID;
		td_talent_hit.nVictimUIID = nVictimUIID;
		td_talent_hit.nTalentID = nTalentID;
		td_talent_hit.nDamage = nDamage;
		td_talent_hit.nMFWeight = nMFWeight;
		td_talent_hit.nMFState = nMFState;
		td_talent_hit.nHitTime = nActHitTime;
		td_talent_hit.nFlags = (uint16)nFlags;

		if (IsKnockBackCommand(nMFState))
		{
			PFI_B(9100, "GTalentHitCommandFactory::MakeHit-KNOCKBACK_MF");

			TD_TALENT_HIT_EX_KNOCKBACK td_talent_hit_ex_knockback;
			BuildParam_ExKnockBack(td_talent_hit_ex_knockback, pAttacker, pVictim, pTalentInfo, nMFState);

			_ASSERT(pNewCommand == NULL);
			pNewCommand = MakeNewCommand(MC_ACTION_TALENT_HIT_KNOCKBACK_MF, 
				2, 
				NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)),
				NEW_SINGLE_BLOB(&td_talent_hit_ex_knockback, sizeof(TD_TALENT_HIT_EX_KNOCKBACK)) );

			PFI_E(9100);
		}
		else
		{
			PFI_B(9101, "GTalentHitCommandFactory::MakeHit-TalentHit");

			_ASSERT(pNewCommand == NULL);
			pNewCommand = MakeNewCommand(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));

			PFI_E(9101);
		}

	}

	return pNewCommand;
}

bool GTalentHitCommandFactory::CheckNoMotionFactor( unsigned short nDamage, int nMFWeight, uint32 nFlags )
{
	if (nDamage == 0 && 
		nMFWeight == 0 && 
		(nFlags == 0 || CheckBitSet(nFlags, CTR_ZERO_TALENT_DAMAGE)) )
	{
		return true;
	}

	return false;
}

bool GTalentHitCommandFactory::IsKnockBackCommand( MF_STATE nMFState )
{
	if (nMFState == MF_KNOCKBACK ||	
		nMFState == MF_THROWUP ||
		nMFState == MF_UPPERED ||
		nMFState == MF_DRAG ||
		nMFState == MF_FAKE_KNOCKBACK)
	{
		return true;
	}

	return false;
}

void GTalentHitCommandFactory::BuildParam_ExKnockBack( TD_TALENT_HIT_EX_KNOCKBACK &out_td_talent_hit_ex_knockback, GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo, MF_STATE nMFState )
{
	if (pAttacker && pVictim)
	{
		out_td_talent_hit_ex_knockback.vKnockbackVictimPos = pVictim->GetPos();
		out_td_talent_hit_ex_knockback.vKnockbackTargetPos = pVictim->GetScheduleMovePos();

		GMotionFactorHandler mf_handler;
		vec3 vKnockbackDir = mf_handler.GetKnockbackDir(nMFState, pVictim, pAttacker, false);
		out_td_talent_hit_ex_knockback.vKnockbackDir = vKnockbackDir;
	}
	else
	{
		out_td_talent_hit_ex_knockback.vKnockbackDir = vec3::AXISY;
		out_td_talent_hit_ex_knockback.vKnockbackVictimPos = vec3::ZERO;
		out_td_talent_hit_ex_knockback.vKnockbackTargetPos = vec3::ZERO;
	}
}

MCommand* GTalentHitCommandFactory::MakeImmume( GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo )
{
	uint32 nResultFlags=0;
	SetBitSet(nResultFlags, CTR_IMMUNE);

	TD_TALENT_HIT td_talent_hit;
	memset(&td_talent_hit, 0, sizeof(TD_TALENT_HIT));

	if (pAttacker)
	{
		td_talent_hit.nAttackerUIID = pAttacker->GetUIID();
	}

	td_talent_hit.nMFState = MF_NONE;
	td_talent_hit.nVictimUIID = pVictim->GetUIID();
	td_talent_hit.nTalentID = (pTalentInfo) ? pTalentInfo->m_nID : 0;
	td_talent_hit.nFlags = (uint16)nResultFlags;

	return MakeNewCommand(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));
}

MCommand* GTalentHitCommandFactory::MakeAvoid( GEntityActor* pAttacker, GEntityActor* pVictim, GTalentInfo* pTalentInfo )
{
	uint32 nResultFlags=0;
	SetBitSet(nResultFlags, CTR_AVOID);

	TD_TALENT_HIT td_talent_hit;
	memset(&td_talent_hit, 0, sizeof(TD_TALENT_HIT));

	if (pAttacker)
	{
		td_talent_hit.nAttackerUIID = pAttacker->GetUIID();
	}

	td_talent_hit.nMFState = MF_NONE;
	td_talent_hit.nVictimUIID = pVictim->GetUIID();
	td_talent_hit.nTalentID = (pTalentInfo) ? pTalentInfo->m_nID : 0;
	td_talent_hit.nFlags = (uint16)nResultFlags;

	return MakeNewCommand(MC_ACTION_TALENT_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_TALENT_HIT)));
}

MCommand* GTalentHitCommandFactory::MakeMiss( const GEntityActor* pAttacker, const GEntityActor* pVictim, GTalentInfo* pTalentInfo )
{
	UIID nAttackerUIID = (pAttacker) ? pAttacker->GetUIID() : UIID_INVALID;
	UIID nVictimUIID = (pVictim) ? pVictim->GetUIID() : UIID_INVALID;

	int nTalentID = (pTalentInfo) ? pTalentInfo->m_nID : 0;

	MCommand* pNewCommand = MakeNewCommand(MC_ACTION_TALENT_HIT_MISS, 3, 
		NEW_USHORT(nVictimUIID), 
		NEW_USHORT(nAttackerUIID),
		NEW_INT(nTalentID)
		);
	return pNewCommand;
}

MCommand* GTalentHitCommandFactory::MakeBuffHit( const GEntityActor* pVictim, int nBuffID, unsigned short nDamage, MF_STATE nMFState, int nMFWeight, uint32 nFlags )
{
	MUID uidVictim = (pVictim)?pVictim->GetUID():MUID::Invalid();
	UIID nVictimUIID = (pVictim) ? pVictim->GetUIID() : UIID_INVALID;

	MCommand* pNewCommand = NULL;
	if (nDamage == 0 && nMFWeight == 0)
	{
		pNewCommand = MakeNewCommand(MC_ACTION_BUFF_HIT_NO_MF, 2, 
			NEW_USHORT(nVictimUIID), 
			NEW_INT(nBuffID));
	}
	else
	{
		TD_BUFF_HIT td_talent_hit;
		td_talent_hit.nVictimUIID = nVictimUIID;
		td_talent_hit.nBuffID = nBuffID;
		td_talent_hit.nDamage = nDamage;
		td_talent_hit.nMFWeight = nMFWeight;
		td_talent_hit.nMFState = nMFState;
		td_talent_hit.nFlags = nFlags;

		pNewCommand = MakeNewCommand(MC_ACTION_BUFF_HIT, 1, NEW_SINGLE_BLOB(&td_talent_hit, sizeof(TD_BUFF_HIT)));
	}

	return pNewCommand;
}