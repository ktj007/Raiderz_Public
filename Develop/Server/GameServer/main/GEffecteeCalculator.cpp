#include "StdAfx.h"
#include "GEffecteeCalculator.h"
#include "GEntityPlayer.h"
#include "GDuel.h"
#include "GParty.h"

GEffecteeCalculator::GEffecteeCalculator(void)
{
}

GEffecteeCalculator::~GEffecteeCalculator(void)
{
}

vector<GEntityActor*> GEffecteeCalculator::Calculate(GField* pField, 
													 GEntityActor* pUser, 
													 TALENT_TARGET nTargetType, 
													 GEntityActor* pTargetActor, 
													 vec3 vPoint, 
													 const float fRadius, 
													 GEntityActor* pAttacker/*=NULL*/, 
													 GEntityActor* pDefender/*=NULL*/ ) const
{
	vector<GEntityActor*> vecEffectees;
	if (!pField)		return vecEffectees; 
	VALID_RET(pUser, vecEffectees);
	VALID_RET(fRadius >= 0.0f, vecEffectees);

	
	if (pTargetActor)
	{
		// 중심 액터가 있다면 좌표를 효과중점으로 대입함
		vPoint = pTargetActor->GetPos();
	}
	else if (vPoint == vec3::ZERO)
	{
		// 중심 액터도 없고 좌표도 설정되어 있지 않다면 사용자를 효과중점으로 대입함
		vPoint = pUser->GetPos();
	}

	// 결투 상대 처리
	GEntityActor* pDuelOpponent = FindValidDuelOpponent(pUser, nTargetType, vPoint, fRadius);
	if (pDuelOpponent)
		vecEffectees.push_back(pDuelOpponent);

	// 대상자 찾기
	ENTITY_TYPEID nFindActorType = GetFindActorType(pUser, nTargetType);

	switch(nTargetType)
	{
	case TALENT_TARGET_AREA_ENEMY:
	case TALENT_TARGET_AREA_ALLIED:
	case TALENT_TARGET_AREA_ALL:
		{
			OnTargetArea(pField, pUser, nFindActorType, vPoint, fRadius, vecEffectees);			
		}break;
	case TALENT_TARGET_SELF:			OnTargetSelf(pUser, vecEffectees);				break;
	case TALENT_TARGET_PARTY:			OnTargetParty(pField, pUser, pUser, fRadius, vecEffectees);		break;
	case TALENT_TARGET_PARTY_TARGET:	OnTargetParty(pField, pUser, pTargetActor, fRadius, vecEffectees);		break;
	case TALENT_TARGET_TARGET:			OnTargetVictim(pTargetActor, vecEffectees);		break;
	case TALENT_TARGET_ATTACKER:		OnTargetAttacker(pAttacker, vecEffectees);		break;
	case TALENT_TARGET_DEFENDER:		OnTargetDefender(pDefender, vecEffectees);		break;
	}

	return vecEffectees;
}

ENTITY_TYPEID 
GEffecteeCalculator::GetFindActorType(GEntityActor* pUser, TALENT_TARGET nTargetType) const
{
	ENTITY_TYPEID nTypeID = ETID_ACTOR;
	if (pUser->IsPlayer())
	{
		if (nTargetType == TALENT_TARGET_AREA_ENEMY)			nTypeID = ETID_NPC;
		else if (nTargetType == TALENT_TARGET_AREA_ALLIED)		nTypeID = ETID_PLAYER;
	}
	else if (pUser->IsNPC())
	{
		if (nTargetType == TALENT_TARGET_AREA_ENEMY)			nTypeID = ETID_PLAYER;
		else if (nTargetType == TALENT_TARGET_AREA_ALLIED)		nTypeID = ETID_NPC;
	}

	return nTypeID;
}

bool 
GEffecteeCalculator::IsValidTarget( GEntityActor* pTarget ) const
{
	if (!pTarget)				return false;
	if (pTarget->IsDead())		return false;
	return true;
}

bool 
GEffecteeCalculator::IsValidDistance( vec3 vPoint, 
								 GEntityActor* pTarget, 
								 const float fEffectRadius ) const
{
	float fDistance = vPoint.DistanceTo(pTarget->GetPos());
	return  (fDistance <= fEffectRadius);
}

bool GEffecteeCalculator::IsSameField( GEntityActor* pUser, GEntityActor* pTarget ) const
{
	return (pUser->GetFieldUID() == pTarget->GetFieldUID());
}

bool GEffecteeCalculator::IsAlreadyInserted(const vector<GEntityActor*> &vecEffectees, 
											GEntityActor* pTarget ) const
{
	return (find(vecEffectees.begin(), vecEffectees.end(), pTarget) != vecEffectees.end());
}

GEntityActor*
GEffecteeCalculator::FindValidDuelOpponent(GEntityActor* pUser, 
									  TALENT_TARGET nTargetType,
									  vec3 vPoint,
									  const float fRadius) const
{
	GEntityPlayer* pUsePlayer = ToEntityPlayer(pUser);
	if (!pUsePlayer)				return NULL;
	if (!pUsePlayer->HasDuel())		return NULL;

	// 적대적인 효과가 아니라면 무시
	if (nTargetType != TALENT_TARGET_AREA_ALL &&
		nTargetType != TALENT_TARGET_AREA_ENEMY)			
	{
		return NULL;
	}

	GDuel* pDuel = pUsePlayer->GetDuel();
	if (!pDuel)												
		return NULL;

	GEntityPlayer* pDuelOpponent = pDuel->GetOpponent(pUsePlayer);
	if (!pDuelOpponent)										
		return NULL;

	if (!IsValidTarget(pDuelOpponent))						return NULL;
	if (!IsValidDistance(vPoint, pDuelOpponent, fRadius))	return NULL;

	return pDuelOpponent;
}

void GEffecteeCalculator::OnTargetArea(GField* pField, 
									   GEntityActor* pUser,
									   ENTITY_TYPEID nFindActorType, 
									   vec3 vPoint, 
									   const float fRadius,
									   vector<GEntityActor*> &vecEffectees) const
{
	if (vPoint == vec3::ZERO) 
		return; // 좌표가 설정되어있어야한다.
	VALID(fRadius >= 0.0f);		// 범위 정보가 있어야된다.

	// 범위가 0이면 중심자만 선택됨
	if (fRadius == 0.0f)
	{
		VALID(pUser);
		vecEffectees.push_back(pUser);
		return;
	}

	GSector* ppSectors[NEIGHBOR_SECTORS_NUM] = {NULL, }; // 일반 섹터+대형 섹터
	MPoint ptSector = pField->GetSectorIndex(vPoint.x, vPoint.y, false);
	pField->GetNeighborSectors(ptSector.x, ptSector.y, ppSectors);

	for (int i = 0; i < NEIGHBOR_SECTORS_NUM; i++)
	{
		GSector* pSector = ppSectors[i];
		if (!pSector)	continue;

		for (GSector::GSectorEntitySet::iterator itor = pSector->ItorEntityBegin(nFindActorType);
			itor != pSector->ItorEntityEnd(nFindActorType);
			++itor)
		{
			MUID& uid = (*itor);
			GEntityActor* pTarget = pField->FindActor(uid);

			if (!IsValidTarget(pTarget))					continue;
			if (!IsValidDistance(vPoint, pTarget, fRadius))	continue;
			if (IsAlreadyInserted(vecEffectees, pTarget))	continue;

			vecEffectees.push_back(pTarget);
		}
	}
}

void GEffecteeCalculator::OnTargetSelf(GEntityActor* pUser, vector<GEntityActor*> &vecEffectees) const
{
	vecEffectees.push_back(pUser);
}

void GEffecteeCalculator::OnTargetParty(GField* pField, 
										GEntityActor* pUser, 
										GEntityActor* pCenterActor, 
										const float fRadius,
										vector<GEntityActor*> &vecEffectees) const
{
	// 중심 액터가 없다면 자기 자신의 파티로 진행한다
	GEntityActor* pTarget = pCenterActor?pCenterActor:pUser;

	// 파티는 플레이어만 가능하다.
	VALID(pTarget->IsPlayer());	

	GEntityPlayer* pTargetPlayer = ToEntityPlayer(pTarget);
	
	if (!pTargetPlayer->HasParty() || !pTargetPlayer->GetParty() ||	// 파티가 없으면 자기 자신만 효과를 얻음
		fRadius == 0.0f	// 범위가 0이면 중심자만 선택됨
		)
	{
		vecEffectees.push_back(pTargetPlayer);
		return;
	}

	vector<MUID> vecPartyMembers = pTargetPlayer->GetParty()->GetPartyMembers();
	for each (MUID uidMember in vecPartyMembers)
	{
		GEntityPlayer* pMember = pField->FindPlayer(uidMember);

		if (!IsValidTarget(pMember))							continue;
		if (!IsSameField(pTarget, pMember))						continue;
		if (!IsValidDistance(pTargetPlayer->GetPos(), pMember, fRadius))	continue;

		vecEffectees.push_back(pMember);
	}
}

void GEffecteeCalculator::OnTargetVictim(GEntityActor* pTargetActor, 
										 vector<GEntityActor*> &vecEffectees ) const
{
	if (!IsValidTarget(pTargetActor))			return;
	vecEffectees.push_back(pTargetActor);
}

void GEffecteeCalculator::OnTargetAttacker(GEntityActor* pAttacker, 
										 vector<GEntityActor*> &vecEffectees ) const
{
	if (!IsValidTarget(pAttacker))			return;
	vecEffectees.push_back(pAttacker);
}

void GEffecteeCalculator::OnTargetDefender(GEntityActor* pDefender, 
										   vector<GEntityActor*> &vecEffectees ) const
{
	if (!IsValidTarget(pDefender))			return;
	vecEffectees.push_back(pDefender);
}

