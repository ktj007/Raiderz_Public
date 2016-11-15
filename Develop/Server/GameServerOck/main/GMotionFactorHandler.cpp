#include "stdafx.h"
#include "GMotionFactorHandler.h"
#include "GField.h"
#include "GEntityActor.h"
#include "GMath.h"
#include "GTestSystem.h"
#include "GGlobal.h"
#include "GActorTalent.h"

void GMotionFactorHandler::Handle( GEntityActor* pVictimActor, const GEntityActor* pOpponent, MF_STATE nMF, int nWeight, bool bKnockbackDirSyncToOpponentDir )
{
	if (!pOpponent)	return;
	if (!pVictimActor) return;

	switch (nMF)
	{
	case MF_KNOCKBACK:
	case MF_THROWUP:
	case MF_UPPERED:
	case MF_DRAG:
		{
			vec3 vPos = MakeKnockbackPos(nMF, pVictimActor, pOpponent, nWeight, bKnockbackDirSyncToOpponentDir);

			pVictimActor->SetScheduleMove(vPos);
		}
		break;
	case MF_FAKE_KNOCKBACK:
		{
			vec3 vPos = MakeKnockbackPos(nMF, pVictimActor, pOpponent, nWeight, bKnockbackDirSyncToOpponentDir);

			pVictimActor->SetPos(vPos);
		}
		break;

	}

}

vec3 GMotionFactorHandler::GetKnockbackDir( MF_STATE nMF, GEntityActor* pVictimActor, const GEntityActor* pOpponent, bool bKnockbackDirSyncToOpponentDir )
{
	vec3 vDir;

	if (nMF == MF_DRAG)
	{
		vDir = pOpponent->GetPos() - pVictimActor->GetPos();
	}
	else
	{
		//if (bKnockbackDirSyncToOpponentDir)
		//{
		//	vDir = pOpponent->GetDir();
		//}
		//else
		{
			vec3 vOpponentPos = 
				pOpponent->GetActorTalent().HasUseTalentPos()?
					pOpponent->GetActorTalent().GetUseTalentPos():
					pOpponent->GetPos();

			// 뒤로 밀려난다.
			vDir = pVictimActor->GetPos() - vOpponentPos;
		}
	}

	GMath::NormalizeZ0(vDir);


	return vDir;
}

vec3 GMotionFactorHandler::MakeKnockbackPos( MF_STATE nMF, GEntityActor* pVictimActor, const GEntityActor* pOpponent, int nWeight, bool bKnockbackDirSyncToOpponentDir )
{
	vec3 vDir = GetKnockbackDir(nMF, pVictimActor, pOpponent, bKnockbackDirSyncToOpponentDir);
	vec3 vPos = pVictimActor->GetPos();

	GField* pField = pVictimActor->GetField();
	if (pField == NULL) return vPos;

	float fDistance = static_cast<float>(nWeight);
	float fVictimRadius = pVictimActor->GetColRadius();

	vec3 vPickOrigin = vPos;
	vPickOrigin.z += fVictimRadius;
	vec3 vPickTo = vPickOrigin + (vDir * (fDistance + fVictimRadius));
	vec3 vPickResult = vPickOrigin;

	vPickOrigin -= vDir;

	if (pField->Pick(vPickOrigin, vPickTo, &vPickResult))
	{
		float fPickDist = vPickResult.DistanceTo(vPickOrigin);
		/*			if (fPickDist < fRadius)
		{
		fDistance = 0.0f;
		}
		else */if (fPickDist < fDistance+fVictimRadius)
		{
			fDistance = fPickDist - fVictimRadius;
		}
	}

	if (fDistance <= 0.0f)
		return vPos; // 피격자의 충돌구보다 작게 넉백되면 무시

	vPos += (vDir*fDistance);
	if (!pField->PickingZ(vPos, vPos))
	{
		gsys.pTestSystem->PickingLog(pVictimActor->GetUID(), L"pick_fail: GMotionFactorHandler::MakeKnockbackPos");
	}

	return vPos;
}