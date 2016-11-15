#pragma once

class GTalent;
class GTalentInfo;
class GEntityActor;

//////////////////////////////////////////////////////////////////////////
//
//	GTalentHit
//
//////////////////////////////////////////////////////////////////////////

class GTalentHit
{
public:
	GTalentHit();

	// 탤런트 판정처리
	void UpdateHit(GTalent* pTalent, float fElapsedTime);
	// 탤런트에 히트된 액터가 있는지 여부
	bool HasVictim()			{ return m_VictimUIDSet.empty()==false; }
public:
	// 해당 탤런트로 피해를 입혔을 때
	void			OnGainDamage(GEntityActor* pTarget);
	// 탤런트가 종료될때 호출 (Finish or Canceled)
	void			OnExit(GTalent* pTalent);
private:
	// 세그먼트 단위 판정처리
	void UpdateHitSeg(GTalent* pTalent, int nHitColIndex, int nAddtiveDamage, float fCheckTime);
private:
	// 진행된 히트 인덱스
	int				m_nHitIndex;
	// 히트된 액터UID
	set<MUID>		m_VictimUIDSet;
	// 히트캡슐에 맞은 횟수
	map<MUID, int>	m_mapHitTable;
	// 세그먼트별로 피해입혔는지
	set<int>		m_setHitsegment;
};
