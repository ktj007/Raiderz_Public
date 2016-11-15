#pragma once

struct TD_ITEM_DROP;
struct TD_LOOT_MASTERLOOT_UI;
struct TD_LOOT_MASTERLOOT_NOTIFY;
enum DROP_ITEM_TYPE;
class GEntityNPC;

// 드랍된 아이템의 보상자 관리

class GDropItem : public MTestMemPool<GDropItem>
{
public:
	GDropItem(const MUID& nDropItemUID, int nItemID, int nItemAmount);
	~GDropItem();

	void InsertAuthorizedCID(int nCID);
	void DecraseAmount(int nDecreaseAmount);

	void SetFreeAuthority();
	void SetGettable();
	void SetMasterLootable();
	void SetRollable();

	void SetType(DROP_ITEM_TYPE nDIT);

	void Visible();
	void Unvisible();

	void GetViewableCID(set<int>& outsetViewableCID) const;
	int GetAmount() const;
	const MUID& GetUID() const;
	int GetID() const;

	bool IsViewableCID(int nCID) const;
	bool IsAuthorizedCID(int nCID) const;
	bool IsGettable() const;
	bool IsMasterLootable() const;
	bool IsRollable() const;
	bool IsVisible() const;
	DROP_ITEM_TYPE GetType() const;

	TD_ITEM_DROP				MakeTDDropItem() const;
	TD_LOOT_MASTERLOOT_UI		MakeTDMasterLootUI(GEntityNPC* pOwner) const;
	TD_LOOT_MASTERLOOT_NOTIFY	MakeTDMasterLootNotify() const;

	void ChangeGettableItem(int nCID);	///< 타입을 Gettable로 바꾸고 소유자를 한사람으로 고정한다.

private:	
	MUID		m_nDropItemUID;
	int			m_nItemID;
	int			m_nItemAmount;
	set<int>	m_setAuthorizedCID;	///< 권한이 있는 사람의 CID
	bool		m_bFreeAuthority;		///< 아무나 권한이 있는지 여부

	DROP_ITEM_TYPE	m_nType;
	bool		m_bVisible;				///< 마스터룻이 준비되면 아이템은 보이지 않는다.
};