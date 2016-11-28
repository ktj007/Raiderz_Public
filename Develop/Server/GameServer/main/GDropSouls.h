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
	bool IsAuthorizedCID(CID nCID);

	void Drop(CID nBeneficiaryCID, GLootInfo* pLootInfo);
	void Drop(const vector<CID>& vecBeneficiaryCID, GLootInfo* pLootInfo);

	int GetQuantity(CID nCID, SOUL_TYPE nSoulType);	

private:
	GEntityNPC*			m_pOwner;
	vector<GDropSoul*>	m_vecDropSoul;
};
