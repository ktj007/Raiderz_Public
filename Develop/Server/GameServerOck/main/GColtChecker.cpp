#include "stdafx.h"
#include "GColtChecker.h"
#include "CCommandTable.h"
#include "GEntityNPC.h"
#include "GJob_Talent.h"
#include "GModuleAI.h"
#include "GField.h"
#include "GModuleCombat.h"
#include "GModuleBuff.h"
#include "GNPCStress.h"
#include "GNPCRage.h"
#include "GColtActionInstMgr.h"
#include "GColtDef.h"
#include "GColtInfo.h"
#include "GNPCBParts.h"

GColtChecker::GColtChecker()
{
}

bool GColtChecker::CheckCondition(const GColtCheck& check, GEntityNPC* pNPC)
{
	PFI_BLOCK_THISFUNC(304);

	switch(check.nType)
	{
	case AST_HP:					return OnHP(check, pNPC);				break;
	case AST_ENEMY_HP:				return OnEnemyHP(check, pNPC);			break;
	case AST_UNBREAKPART:			return OnUnbreakpart(check, pNPC);		break;
	case AST_BREAKPART:				return OnBreakpart(check, pNPC);		break;
	case AST_DAMAGE:				return OnDamage(check, pNPC);			break;
	case AST_MOTIONFACTOR:			return OnMotionfactor(check, pNPC);		break;
	case AST_MODE:					return OnMode(check, pNPC);				break;
	case AST_ENEMY_MOTIONFACTOR:	return OnEnemyMotionfactor(check, pNPC);break;
	case AST_DISTANCE:				return OnDistance(check, pNPC);			break;
	case AST_ANGLE:					return OnAngle(check, pNPC);			break;
	case AST_STRESS:				return OnStress(check, pNPC);			break;
	case AST_HIT_CAPSULE:			return OnHitCapsule(check, pNPC);		break;
	case AST_USED_TALENT:			return OnUsedTalent(check, pNPC);		break;
	case AST_GAINED_BUFF:			return OnGainedBuff(check, pNPC);		break;
	case AST_ENEMY_GAINED_BUFF:		return OnEnemyGainedBuff(check, pNPC);	break;
	case AST_REAR:					return OnRear(check, pNPC);				break;
	case AST_FRONT:					return OnFront(check, pNPC);			break;
	case AST_RAGE:					return OnRage(check, pNPC);				break;
	case AST_DEAD:					return OnDead(check, pNPC);				break;
	case AST_COMBAT:				return OnCombat(check, pNPC);			break;
	case AST_PEACE:					return OnPeace(check, pNPC);			break;
	}

	return false;
}

bool GColtChecker::OnHP(const GColtCheck& check, GEntityNPC* pNPC)
{
	const int OwnerNPCHPPercent = (int)(((float)pNPC->GetHP()/(float)pNPC->GetMaxHP())*100);

	int nLowHP	= check.vecParams[0].second,
		nHighHP = check.vecParams[1].second;

	if (check.vecParams[0].first &&
		check.vecParams[1].first &&
		OwnerNPCHPPercent >= nLowHP &&
		OwnerNPCHPPercent < nHighHP)
	{
		return true;
	}

	if (check.vecParams[0].first &&
		!check.vecParams[1].first &&
		OwnerNPCHPPercent >= nLowHP)
	{
		return true;
	}

	if (!check.vecParams[0].first &&
		check.vecParams[1].first &&
		OwnerNPCHPPercent < nHighHP)
	{
		return true;
	}

	return false;
}

bool GColtChecker::OnEnemyHP(const GColtCheck& check, GEntityNPC* pNPC)
{
	GModuleAI* pModuleAI = pNPC->GetModuleAI();
	if (pModuleAI == NULL)			return false;

	GEntityActor* pTargetActor = pNPC->GetTargetActor();
	if (pTargetActor == NULL)		return false;

	const int EnemyHPPercent = (int)(((float)pTargetActor->GetHP()/(float)pTargetActor->GetMaxHP())*100);

	int nLowHP	= check.vecParams[0].second,
		nHighHP = check.vecParams[1].second;

	if (check.vecParams[0].first &&
		check.vecParams[1].first &&
		EnemyHPPercent >= nLowHP &&
		EnemyHPPercent < nHighHP)
	{
		return true;
	}

	if (check.vecParams[0].first &&
		!check.vecParams[1].first &&
		EnemyHPPercent >= nLowHP)
	{
		return true;
	}

	if (!check.vecParams[0].first &&
		check.vecParams[1].first &&
		EnemyHPPercent < nHighHP)
	{
		return true;
	}

	return false;
}

bool GColtChecker::OnUnbreakpart(const GColtCheck& check, GEntityNPC* pNPC)
{
	int nPartID	= check.vecParams[0].second;

	if (check.vecParams[0].first && pNPC->GetNPCBParts())
	{
		if (!pNPC->GetNPCBParts()->IsBroken(nPartID))
		{
			return true;
		}
	}

	return false;
}

bool GColtChecker::OnBreakpart(const GColtCheck& check, GEntityNPC* pNPC)
{
	int nPartID	= check.vecParams[0].second;

	if (check.vecParams[0].first && pNPC->GetNPCBParts())
	{
		if (pNPC->GetNPCBParts()->IsBroken(nPartID))
		{
			return true;
		}
	}

	return false;
}

bool GColtChecker::OnDamage(const GColtCheck& check, GEntityNPC* pNPC)
{
	const int OwnerLastDamage		= pNPC->GetLastDamagedInfo().nAmount;
	const int OwnerLastDamageType	= pNPC->GetLastDamagedInfo().nType;

	uint32 nDamageType = check.vecParams[0].second;
	int nLowDamage	= check.vecParams[1].second;
	int nHighDamage	= check.vecParams[2].second;

	if (nDamageType != DA_NONE &&
		nDamageType != OwnerLastDamageType)
		return false;		// 피해 타입이 맞지 않음

	if (!check.vecParams[0].first ||
		check.vecParams[0].first)
	{
		if (!check.vecParams[1].first &&
			!check.vecParams[2].first)
		{
			return true;
		}

		if (check.vecParams[1].first &&
			check.vecParams[2].first &&
			OwnerLastDamage >= nLowDamage &&
			OwnerLastDamage < nHighDamage)
		{
			return true;
		}

		if (check.vecParams[1].first &&
			!check.vecParams[2].first &&
			OwnerLastDamage >= nLowDamage)
		{
			return true;
		}

		if (!check.vecParams[1].first &&
			check.vecParams[2].first &&
			OwnerLastDamage < nHighDamage)
		{
			return true;
		}
	}

	return false;
}

bool GColtChecker::OnMotionfactor(const GColtCheck& check, GEntityNPC* pNPC)
{
	int nMotionfactorID	= check.vecParams[0].second;

	if (check.vecParams[0].first && 
		pNPC->GetCurrentMotionFactor() == nMotionfactorID)
	{
		return true;
	}

	return false;
}

bool GColtChecker::OnMode(const GColtCheck& check, GEntityNPC* pNPC)
{
	int nMode	= check.vecParams[0].second;

	if (check.vecParams[0].first && 
		pNPC->GetMode() == nMode)
	{
		return true;
	}

	return false;
}

bool GColtChecker::OnEnemyMotionfactor(const GColtCheck& check, GEntityNPC* pNPC)
{
	GModuleAI* pModuleAI = pNPC->GetModuleAI();
	if (pModuleAI == NULL)			return false;
	
	GEntityActor* pTargetActor = pNPC->GetTargetActor();
	if (pTargetActor == NULL)		return false;

	int nMotionfactorID	= check.vecParams[0].second;

	if (check.vecParams[0].first && 
		pTargetActor->GetCurrentMotionFactor() == nMotionfactorID)
	{
		return true;
	}

	return false;
}

bool GColtChecker::OnDistance(const GColtCheck& check, GEntityNPC* pNPC)
{
	GModuleAI* pModuleAI = pNPC->GetModuleAI();
	if (pModuleAI == NULL)			return false;

	GEntityActor* pTargetActor = pNPC->GetTargetActor();
	if (pTargetActor == NULL)		return false;

	int nLowDistance	= check.vecParams[0].second;
	int nHighDistance	= check.vecParams[1].second;

	vec3 vMyPos = pNPC->GetPos();
	vec3 vTarPos = pTargetActor->GetPos();
	vMyPos.z = vTarPos.z = 0;

	int nDistance		= (int)vMyPos.DistanceTo(vTarPos);

	if (check.vecParams[0].first &&
		check.vecParams[1].first &&
		nDistance >= nLowDistance &&
		nDistance < nHighDistance)
	{
		return true;
	}

	if (check.vecParams[0].first &&
		!check.vecParams[1].first &&
		nDistance >= nLowDistance)
	{
		return true;
	}

	if (!check.vecParams[0].first &&
		check.vecParams[1].first &&
		nDistance < nHighDistance)
	{
		return true;
	}

	return false;
}

bool GColtChecker::OnAngle(const GColtCheck& check, GEntityNPC* pNPC)
{
	GModuleAI* pModuleAI = pNPC->GetModuleAI();
	if ( pModuleAI == NULL)
		return false;

	GEntityActor* pTargetActor = pNPC->GetTargetActor();
	if ( pTargetActor == NULL)
		return false;

	vec3 vDirThis	= pNPC->GetDir();
	float fAngle = RadianToDegree( vDirThis.AngleToXY( pTargetActor->GetPos() - pNPC->GetPos()));		
	int nAngle		= (0 < fAngle) ? (int) (fAngle + 0.5f) : (int) (fAngle -0.5f);
	int nLowAngle	= check.vecParams[ 0].second;

	if ( nLowAngle < 0)			// Rotate CW
	{
		if (nAngle <= nLowAngle)
			return true;
	}
	else if ( nLowAngle > 0)	// Rotate CCW
	{
		if (nAngle >= nLowAngle)
			return true;
	}

	return false;
}

bool GColtChecker::OnStress(const GColtCheck& check, GEntityNPC* pNPC)
{
	int nLowStress	= check.vecParams[0].second;
	int nHighStress	= check.vecParams[1].second;
	int nCurStress	= pNPC->GetStress();

	if (!check.vecParams[0].first &&
		!check.vecParams[1].first)
	{
		pNPC->GetNPCStress().Clear();
		return true;
	}

	if (check.vecParams[0].first &&
		check.vecParams[1].first &&
		nCurStress >= nLowStress &&
		nCurStress < nHighStress)
	{
		pNPC->GetNPCStress().Clear();
		return true;
	}

	if (check.vecParams[0].first &&
		!check.vecParams[1].first &&
		nCurStress >= nLowStress)
	{
		pNPC->GetNPCStress().Clear();
		return true;
	}

	if (!check.vecParams[0].first &&
		check.vecParams[1].first &&
		nCurStress < nHighStress)
	{
		pNPC->GetNPCStress().Clear();
		return true;
	}

	return false;
}

bool GColtChecker::OnHitCapsule(const GColtCheck& check, GEntityNPC* pNPC)
{
	if (!check.vecParams[0].first) return false;

	int nHitCapsuleIndex	= check.vecParams[0].second;

	if (pNPC->GetCapsuleGroupIndex() == nHitCapsuleIndex)
	{
		return true;
	}

	return false;
}

bool GColtChecker::OnUsedTalent(const GColtCheck& check, GEntityNPC* pNPC)
{
	if (!check.vecParams[0].first) return false;

	int nTalentID	= check.vecParams[0].second;

	if (pNPC->GetModuleCombat() &&
		pNPC->GetModuleCombat()->GetUsingTalentID() == nTalentID)
	{
		return true;
	}

	return false;
}

bool GColtChecker::OnGainedBuff(const GColtCheck& check, GEntityNPC* pNPC)
{
	if (!check.vecParams[0].first) return false;

	int nBuffID	= check.vecParams[0].second;

	return pNPC->HasBuff(nBuffID);
}

bool GColtChecker::OnEnemyGainedBuff(const GColtCheck& check, GEntityNPC* pNPC)
{
	if (!check.vecParams[0].first) return false;

	int nBuffID	= check.vecParams[0].second;

	GEntityActor* pTargetActor = pNPC->GetTargetActor();
	if (pTargetActor == NULL)		return false;

	return pTargetActor->HasBuff(nBuffID);
}

bool GColtChecker::OnRear(const GColtCheck& check, GEntityNPC* pNPC)
{
	GEntityActor* pTargetActor = pNPC->GetTargetActor();
	if (pTargetActor == NULL)		return false;

	return pTargetActor->IsBack(pNPC->GetPos());
}

bool GColtChecker::OnFront(const GColtCheck& check, GEntityNPC* pNPC)
{
	GEntityActor* pTargetActor = pNPC->GetTargetActor();
	if (pTargetActor == NULL)		return false;

	return !pTargetActor->IsBack(pNPC->GetPos());
}


bool GColtChecker::OnRage(const GColtCheck& check, GEntityNPC* pNPC)
{
	const int nRage = pNPC->GetNPCRage().GetRage();

	int nLow	= check.vecParams[0].second,
		nHigh	= check.vecParams[1].second;

	if (check.vecParams[0].first &&
		check.vecParams[1].first &&
		nRage >= nLow &&
		nRage < nHigh)
	{
		pNPC->GetNPCRage().ResetRage();
		return true;
	}

	if (check.vecParams[0].first &&
		!check.vecParams[1].first &&
		nRage >= nLow)
	{
		pNPC->GetNPCRage().ResetRage();
		return true;
	}

	if (!check.vecParams[0].first &&
		check.vecParams[1].first &&
		nRage < nHigh)
	{
		pNPC->GetNPCRage().ResetRage();
		return true;
	}

	return false;
}

bool GColtChecker::OnDead(const GColtCheck& check, GEntityNPC* pNPC)
{
	if (pNPC == NULL)		return false;
	return pNPC->IsDead();
}

bool GColtChecker::OnCombat(const GColtCheck& check, GEntityNPC* pNPC)
{
	if (pNPC == NULL)		return false;
	return pNPC->IsNowCombat();
}

bool GColtChecker::OnPeace(const GColtCheck& check, GEntityNPC* pNPC)
{
	if (pNPC == NULL)		return false;
	return !pNPC->IsNowCombat();
}

