#pragma once

class GEntityPlayer;
enum CHAR_STANCE;

class GPlayerStance
{
public:
	GPlayerStance(GEntityPlayer* pOwner);
	~GPlayerStance(void);

	CHAR_STANCE	Get()							{ return m_nStance; }
	void		Change(CHAR_STANCE nStance);
	void		OnUnarmed();

private:
	GEntityPlayer*	m_pOwner;
	CHAR_STANCE		m_nStance;
};
