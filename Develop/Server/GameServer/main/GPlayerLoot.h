#pragma once

class MRegulator;
class GEntityPlayer;
class GEntityNPC;

class GPlayerLoot : public MTestMemPool<GPlayerLoot>
{
public:
	GPlayerLoot(GEntityPlayer*	pOwner);
	~GPlayerLoot();

	void Update(float fDelta);

	void BeginByKillNPC(MUID uidNPC);
	void BeginByInteractNPC(MUID uidNPC, float fTime);
	void BeginByGatherNPC(MUID uidNPC, float fTime, int nLootID);
	void EndLoot();

	GEntityNPC* GetLootingNPC();

	bool IsLootingNPC(const MUID& uidNPC);
	bool IsILootRegulatorActive();

private:
	bool UpdateInteractionLoot(float fDelta);

private:
	GEntityPlayer*	m_pOwner;
	MRegulator*		m_pILootRegulator;	///< IET_LOOT 소요시간 타이머
	MUID			m_LootingNPCUID;	///< 루팅중인 NPC UID
	int				m_nLootID;			///< 채집 시, 사용 루팅테이블
};
