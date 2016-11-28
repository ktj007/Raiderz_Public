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

	bool DropByKill(CID nBeneficiaryCID);
	TEST_VIRTUAL bool DropByKill(const vector<CID>& vecBeneficiaryCID, MUID nPartyUID);
	bool DropByInteract_Mortal(CID nBeneficiaryCID, int nLootID);
	bool DropByInteract_Immortal(CID nBeneficiaryCID,int nLootID);
	TEST_VIRTUAL bool DropByGather(CID nGatherCID, int nLootID);

public : // helper
	bool IsEmpty();
	bool IsAuthorizedCID(CID nCID);
	bool IsViewableCID(CID nCID);
	bool IsDropByKill();
	bool IsDropByGather();
	bool IsDropByInteract_Mortal();
	bool IsDropByInteract_Immortal();
	bool IsBeneficiaryCID(CID nCID);

	void GetViewableCID(set<CID>& outsetViewableCID);
	const set<CID>& GetBeneficiaryCID();
	CID GetMasterCID();
	MUID GetPartyUID();
	
	GDropItems& GetDropItems();
	GDropMoney& GetDropMoney();
	GDropSouls& GetDropSouls();

	void SetDropCause(DROP_CAULE nDropCause);

	void InsertBeneficiaryCID(CID nCID);

private:
	bool DropByInteract(CID nBeneficiaryCID, int nLootID, bool bImmortal);

private:
	GEntityNPC*				m_pOwner;
	GDropItems*				m_pDropItems;
	GDropMoney*				m_pDropMoney;	
	GDropSouls*				m_pDropSouls;
	DROP_CAULE				m_nDropCause;

	set<CID>				m_setBeneficiaryCID;
	CID						m_nMasterCID;
	MUID					m_nPartyUID;
};