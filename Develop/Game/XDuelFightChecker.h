#pragma once


class XDuelFightChecker
{
private:
	XPlayer*				m_pPlayer;

public:
	XDuelFightChecker() : m_pPlayer(NULL) {}
	virtual ~XDuelFightChecker() {}

	void					Init(XPlayer* pPlayer);
	void					Update();

	bool					SetDuel(XObject* pAttacker);
	void					ReSetDuel();
};
