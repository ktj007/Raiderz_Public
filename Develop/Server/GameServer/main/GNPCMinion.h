#pragma once

class GEntityNPC;

class GNPCMinion : public MTestMemPool<GNPCMinion>
{
public:
	GNPCMinion(GEntityNPC* pOwner);
	~GNPCMinion(void);

	// 부하 소환
	GEntityNPC* Summon(int nNPCID, vec3 vStartPos);

	// 소환자 반환
	GEntityNPC* GetSummoner() const;

	// 주인 UID 설정
	void SetMasterUID(MUID uidMaster)		{ m_uidMaster = uidMaster; }
	// 주인이 있는지 여부
	bool HasMaster()		{ return m_uidMaster.IsValid(); }
	// 부하 갯수 반환
	size_t GetMinionCount()	{ return m_vecMinions.size(); }

	// 소유자가 죽었을때 호출
	void OnDead();
	// 부하가 죽었을때 호출
	void OnDeadMinion(MUID uidMinion);
	// 전투가 종료될 때 호출
	void OnEndCombat();

private:
	// 주인이 죽었을때 호출
	void OnDeadMaster();
	// 모든 부하를 죽임
	void KillAllMinions();
	// 모든 부하를 제거
	void DespawnAllMinions();
	// 주인의 적을 공격하게 설정
	void SetTargetMasterEnemy(GEntityNPC* pMinion);
	
private:
	GEntityNPC*			m_pOwner;
	vector<MUID>		m_vecMinions;
	MUID				m_uidMaster;
};
