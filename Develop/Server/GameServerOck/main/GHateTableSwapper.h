#pragma once

#include "GHateTableDef.h"

class GEntityNPC;

// 책임:	셀렉션-이벤트에 맞게 타겟셀렉터를 조정해준다.
//			원래의 기능으로 되돌리는 책임도 맡는다.
//
// 관계:	GTargetSelectionEvent, GHateTable
class GHateTableSwapper
{
public:
	GHateTableSwapper(GEntityNPC* pOwner, HatePointMap& mapAggroScore);
	
	bool ApplyEvent(HATE_TARGET_SWAP_EVENT nEventType, float fDuration);
	void Update(float fDelta);
	MUID GetTargetByEvent() const;
	void ResetTarget();
	bool HasAppled() const;
private:
	bool Enter(HATE_TARGET_SWAP_EVENT nEventType);
	void Exit();
	void CompleateFlip(HATE_TARGET_SWAP_EVENT nEventType);
	void OnFilp(HATE_TARGET_SWAP_EVENT nEventType, MUID& outuidTop, MUID& outuidBottom );
	void _SortByScore(HateReverseMap& mapScore);
	bool IsValidTarget(MUID id);

	void OnApply_Flip(const HateReverseMap& mapScore, MUID& outuidTop, MUID& outuidBottom);
	void OnApply_Random(const HateReverseMap& mapScore, MUID& outuidTop, MUID& outuidBottom);
	void OnApply_Near(const HateReverseMap& mapScore, MUID& outuidTop, MUID& outuidBottom);
	void OnApply_Far(const HateReverseMap& mapScore, MUID& outuidTop, MUID& outuidBottom);
	void OnApply_Short(const HateReverseMap& mapScore, MUID& outuidTop, MUID& outuidBottom);
private:
	HATE_TARGET_SWAP_EVENT	m_nEventType;
	MUID					m_uidTarget;
	MRegulator				m_rgrExpirer;
	HatePointMap&			m_mapAggroScore;
	GEntityNPC*				m_pOwner;
};
