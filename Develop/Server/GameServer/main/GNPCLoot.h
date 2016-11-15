#pragma once

class GEntityNPC;
class GRewarderSelector;
class GDropList;
class GDropList_InteractNPC;

class GNPCLoot : public MTestMemPool<GNPCLoot>
{
TEST_FRIEND_NPCSETTER;

private:
	set<int>				m_setLootingPlayerCID;		///< 루팅중인 플레이어의 CID
	GDropList*				m_pDropList;
	int						m_nBPartLootID;

public:
	GNPCLoot(GEntityNPC* pOwner);
	~GNPCLoot();

	void Begin(int nCID);
	void End(int nCID);
	set<int> GetLootingPlayerCID();
	bool IsLootingPlayer(int nCID);
	bool IsLootingPlayerEmpty();

	GDropList&	GetDropList() { _ASSERT(m_pDropList); return (*m_pDropList); }

	int GetBPartLootID() const;
	void SetBPartLootID(int nID);
};
