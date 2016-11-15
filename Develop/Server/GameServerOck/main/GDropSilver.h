#pragma once

class GLootInfo;
class GEntityPlayer;

// 드랍된 실버의 보상자관리

class GDropMoney : public MTestMemPool<GDropMoney>
{
public:
	GDropMoney();		
	void Clear();

	void Drop(int nBeneficiaryCID, GLootInfo* pLootInfo);
	void Drop(const vector<int>& vecBeneficiaryCID, GLootInfo* pLootInfo);

	bool IsAuthorizedCID(int nCID);
	bool IsEmpty();
	int	 GetQuantity();

	void GetViewableCID(set<int>& outsetViewableCID);
	void GetAuthorizedCID(set<int>& outsetAuthorizedCID);

private:
	int			m_nMoney;
	set<int>	m_setAuhorizedCID;	// 획득 권한이 있는 사람의 CID
};
