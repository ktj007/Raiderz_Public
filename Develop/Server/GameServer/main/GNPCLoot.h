#pragma once

class GEntityNPC;
class GRewarderSelector;
class GDropList;
class GDropList_InteractNPC;

class GNPCLoot : public MTestMemPool<GNPCLoot>
{
TEST_FRIEND_NPCSETTER;

private:
	set<CID>				m_setLootingPlayerCID;		///< 루팅중인 플레이어의 CID
	GDropList*				m_pDropList;
	int						m_nBPartLootID;

public:
	GNPCLoot(GEntityNPC* pOwner);
	~GNPCLoot();

	void Begin(CID nCID);
	void End(CID nCID);
	set<CID> GetLootingPlayerCID();
	bool IsLootingPlayer(CID nCID);
	bool IsLootingPlayerEmpty();

	GDropList&	GetDropList() { _ASSERT(m_pDropList); return (*m_pDropList); }

	int GetBPartLootID() const;
	void SetBPartLootID(int nID);
};
