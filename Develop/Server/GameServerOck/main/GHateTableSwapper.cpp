#include "StdAfx.h"
#include "GHateTableSwapper.h"
#include "GField.h"
#include "GEntityNPC.h"

GHateTableSwapper::GHateTableSwapper( GEntityNPC* pOwner, HatePointMap& mapAggroScore ) 
: m_pOwner(pOwner)
, m_mapAggroScore(mapAggroScore)
{

}

bool GHateTableSwapper::ApplyEvent( HATE_TARGET_SWAP_EVENT nEventType, float fDuration )
{
	if (m_rgrExpirer.IsActive())
	{
		m_rgrExpirer.Stop();
		Exit();
	}

	if (fDuration <= 0.0f)
	{
		CompleateFlip(nEventType);
	}
	else
	{
		m_rgrExpirer.SetTickTime(fDuration);
		m_rgrExpirer.Start();
		if (!Enter(nEventType))
			return false;
	}

	return true;
}

void GHateTableSwapper::Update( float fDelta )
{
	if (!IsValidTarget(m_uidTarget))
	{
		m_uidTarget.SetInvalid();
	}

	if (m_rgrExpirer.IsReady(fDelta))
	{
		Exit();
	}
}

void GHateTableSwapper::ResetTarget()
{
	m_rgrExpirer.Stop();
	Exit();

	m_rgrExpirer.Reset();
}

bool GHateTableSwapper::Enter(HATE_TARGET_SWAP_EVENT nEventType)
{
	if (m_mapAggroScore.size() <= 1)	return false;

	m_nEventType = nEventType;

	MUID uidTop, uidBottom;
	OnFilp(m_nEventType, uidTop, uidBottom);

	m_uidTarget = uidTop;

	return true;
}

void GHateTableSwapper::Exit()
{
	m_nEventType = HATESWAP_NONE;
	m_uidTarget.SetInvalid();
}

void GHateTableSwapper::CompleateFlip(HATE_TARGET_SWAP_EVENT nEventType)
{
	m_nEventType = HATESWAP_NONE;
	m_uidTarget.SetInvalid();

	MUID uidTop, uidBottom;
	OnFilp(nEventType, uidTop, uidBottom);

	HatePointMap::iterator iterTop = m_mapAggroScore.find(uidTop);
	HatePointMap::iterator iterBottom = m_mapAggroScore.find(uidBottom);

	if (iterTop		== m_mapAggroScore.end() ||
		iterBottom	== m_mapAggroScore.end())
	{
		return;
	}

	HatePoint fTopScore		= iterTop->second;
	HatePoint fBottomScore	= iterBottom->second;

	m_mapAggroScore[uidTop]		= fBottomScore;
	m_mapAggroScore[uidBottom]	= fTopScore;
}

void GHateTableSwapper::_SortByScore(HateReverseMap& mapScore)
{
	for (HatePointMap::iterator iter = m_mapAggroScore.begin();
		iter != m_mapAggroScore.end();
		++iter)
	{
		mapScore.insert(HateReverseMap::value_type(iter->second, iter->first));
	}
}

void GHateTableSwapper::OnApply_Flip(const HateReverseMap& mapScore, MUID& outuidTop, MUID& outuidBottom)
{
	HateReverseMap::const_iterator iter = mapScore.begin();
	outuidBottom = iter->second;
	iter++;
	outuidTop = iter->second;
}

void GHateTableSwapper::OnApply_Random(const HateReverseMap& mapScore, MUID& outuidTop, MUID& outuidBottom)
{
	HateReverseMap::const_iterator iter = mapScore.begin();
	outuidBottom = iter->second;

	int nRandomCounter = MMath::RandomNumber(1, (int)mapScore.size()-1);
	for (int i=0; i<nRandomCounter; i++) 
		iter++;
	outuidTop = iter->second;
}

void GHateTableSwapper::OnApply_Near(const HateReverseMap& mapScore, MUID& outuidTop, MUID& outuidBottom)
{
	GField* pField = m_pOwner->GetField();
	VALID(pField);

	HateReverseMap::const_iterator iter = mapScore.begin();
	outuidBottom = iter->second;

	vec3 vOwnerPos = m_pOwner->GetPos();
	float fBestDistance = 10000.0f;
	for (iter;
		iter != mapScore.end();
		++iter)
	{
		MUID uidEach = iter->second;
		if (!IsValidTarget(uidEach))
			continue;

		GEntityActor* pTargetActor = pField->FindActor(uidEach);
		float fDistance = vOwnerPos.DistanceTo(pTargetActor->GetPos());
		if (fDistance <= fBestDistance)	
		{
			outuidTop = uidEach;
			fBestDistance = fDistance;
		}
	}
}

void GHateTableSwapper::OnApply_Far(const HateReverseMap& mapScore, MUID& outuidTop, MUID& outuidBottom)
{
	GField* pField = m_pOwner->GetField();
	VALID(pField);

	HateReverseMap::const_iterator iter = mapScore.begin();
	outuidBottom = iter->second;

	vec3 vOwnerPos = m_pOwner->GetPos();
	float fBestDistance = 0;
	for (iter;
		iter != mapScore.end();
		++iter)
	{
		MUID uidEach = iter->second;
		if (!IsValidTarget(uidEach))
			continue;

		GEntityActor* pTargetActor = pField->FindActor(uidEach);
		float fDistance = vOwnerPos.DistanceTo(pTargetActor->GetPos());
		if (fDistance >= fBestDistance)	
		{
			outuidTop = uidEach;
			fBestDistance = fDistance;
		}
	}
}

void GHateTableSwapper::OnApply_Short(const HateReverseMap& mapScore, MUID& outuidTop, MUID& outuidBottom)
{
	GField* pField = m_pOwner->GetField();
	VALID(pField);
	VALID(m_pOwner->GetWalkSpeed() != 0);
	VALID(m_pOwner->GetRotateSpeed() != 0);

	HateReverseMap::const_iterator iter = mapScore.begin();
	outuidBottom = iter->second;

	vec3 vOwnerPos = m_pOwner->GetPos();
	float fBestPoint = 10000.0f;;
	for (iter;
		iter != mapScore.end();
		++iter)
	{
		MUID uidEach = iter->second;
		if (!IsValidTarget(uidEach))
			continue;

		GEntityActor* pTargetActor = pField->FindActor(uidEach);
		float fDistance = vOwnerPos.DistanceTo(pTargetActor->GetPos());
		vec3 vDir = pTargetActor->GetPos() - vOwnerPos;
		GMath::NormalizeZ0(vDir);
		float fAngleDegree = MMath::RadianToDegree(abs(vDir.AngleToXY(m_pOwner->GetDir())));

		float fPoint = 0.0f;
		fPoint += (fDistance / m_pOwner->GetWalkSpeed());
		fPoint += (fAngleDegree / m_pOwner->GetRotateSpeed());

		if (fPoint < fBestPoint)	
		{
			outuidTop = uidEach;
			fBestPoint = fPoint;
		}
	}
}


void GHateTableSwapper::OnFilp(HATE_TARGET_SWAP_EVENT nEventType, MUID& outuidTop, MUID& outuidBottom )
{
	GField* pField = m_pOwner->GetField();
	VALID(pField);

	HateReverseMap mapScore;
	_SortByScore(mapScore);
	if (mapScore.size() <= 1)	
		return; // 바꿀 목록 갯수가 충분치 앟음

	outuidTop.SetInvalid();
	outuidBottom.SetInvalid();

	// 방향값이 없으면 short을 near로 처리
	if (m_pOwner->GetRotateSpeed() == 0 &&
		nEventType == HATESWAP_SHORT)
	{
		nEventType = HATESWAP_NEAR;
	}

	switch (nEventType)
	{
	case HATESWAP_FLIP:		{ OnApply_Flip(mapScore, outuidTop, outuidBottom);	 }break;
	case HATESWAP_RANDOM:	{ OnApply_Random(mapScore, outuidTop, outuidBottom); }break;
	case HATESWAP_NEAR:		{ OnApply_Near(mapScore, outuidTop, outuidBottom);	 }break;
	case HATESWAP_FAR:		{ OnApply_Far(mapScore, outuidTop, outuidBottom);	 }break;
	case HATESWAP_SHORT:	{ OnApply_Short(mapScore, outuidTop, outuidBottom);	 }break;
	default:
		VALID(L"정의되지 않은 HateTable SwapType");
	}
}

bool GHateTableSwapper::IsValidTarget(MUID id)
{
	GField* pField = m_pOwner->GetField();
	if (!pField)
		return false; // 필드가 존재하지 않음

	GEntityActor* pTargetActor = pField->FindActor(id);
	if (!pTargetActor) 
		return false; // 목표 액터를 찾을 수 없음
	if (pTargetActor->IsDead()) 
		return false; // 목표가 죽어있음

	return true;
}

MUID GHateTableSwapper::GetTargetByEvent() const
{
	return m_uidTarget;
}

bool GHateTableSwapper::HasAppled() const
{
	return GetTargetByEvent().IsValid();
}