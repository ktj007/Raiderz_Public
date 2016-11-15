#pragma once

class GPlayerRole : public MTestMemPool<GPlayerRole>
{
public:
	GPlayerRole(GEntityPlayer* pOwner);
	~GPlayerRole(void);

	bool IsHealer();
	int CalcHealTalentPoint(int nPlayerLevel) const;
	int GetNeedHealTalentPoint(int nPlayerLevel) const;

private:
	GEntityPlayer*	m_pOwner;

};
