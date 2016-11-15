#pragma once

class GEntityNPC;
class GEntityPlayer;
class GLootInfo;
class GDropItems;
class GDropMoney;
class GDropSouls;

enum DROP_CAULE;

struct LOOTING_RULE_DATA;

// 실버와 아이템 드랍
// 드랍된 아이템과 실버 관리

class GDropList : public MTestMemPool<GDropList>
{
public:
	GDropList(GEntityNPC* pOwner);
	TEST_VIRTUAL ~GDropList(void);

	void Clear();

	bool DropByKill(int nBeneficiaryCID);
	TEST_VIRTUAL bool DropByKill(const vector<int>& vecBeneficiaryCID, MUID nPartyUID);
	bool DropByInteract_Mortal(int nBeneficiaryCID, int nLootID);
	bool DropByInteract_Immortal(int nBeneficiaryCID,int nLootID);
	TEST_VIRTUAL bool DropByGather(int nGatherCID, int nLootID);

public : // helper
	bool IsEmpty();
	bool IsAuthorizedCID(int nCID);
	bool IsViewableCID(int nCID);
	bool IsDropByKill();
	bool IsDropByGather();
	bool IsDropByInteract_Mortal();
	bool IsDropByInteract_Immortal();
	bool IsBeneficiaryCID(int nCID);

	void GetViewableCID(set<int>& outsetViewableCID);
	const set<int>& GetBeneficiaryCID();
	int GetMasterCID();
	MUID GetPartyUID();
	
	GDropItems& GetDropItems();
	GDropMoney& GetDropMoney();
	GDropSouls& GetDropSouls();

	void SetDropCause(DROP_CAULE nDropCause);

	void InsertBeneficiaryCID(int nCID);

private:
	bool DropByInteract(int nBeneficiaryCID, int nLootID, bool bImmortal);

private:
	GEntityNPC*				m_pOwner;
	GDropItems*				m_pDropItems;
	GDropMoney*				m_pDropMoney;	
	GDropSouls*				m_pDropSouls;
	DROP_CAULE				m_nDropCause;

	set<int>				m_setBeneficiaryCID;
	int						m_nMasterCID;
	MUID					m_nPartyUID;
};