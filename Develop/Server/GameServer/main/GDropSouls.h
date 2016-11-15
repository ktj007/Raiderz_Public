#pragma once

class GDropSoul;
class GEntityPlayer;
class GEntityNPC;
class GLootInfo;

class GDropSouls : public MTestMemPool<GDropSouls>
{
public:
	GDropSouls(GEntityNPC* pOwner);
	~GDropSouls(void);

	void Clear();
	bool IsEmpty();
	bool IsAuthorizedCID(int nCID);

	void Drop(int nBeneficiaryCID, GLootInfo* pLootInfo);
	void Drop(const vector<int>& vecBeneficiaryCID, GLootInfo* pLootInfo);

	int GetQuantity(int nCID, SOUL_TYPE nSoulType);	

private:
	GEntityNPC*			m_pOwner;
	vector<GDropSoul*>	m_vecDropSoul;
};
