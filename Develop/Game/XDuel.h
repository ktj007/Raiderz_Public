#pragma once

class XDuel
{
protected:
	vector<MUID> m_vecOpponentUID;
	vector<MUID> m_vecAllyUID;

	void RemoveUID(vector<MUID>& vecUID, MUID OpponentUID);

public:
	XDuel(void);
	virtual ~XDuel(void);

	void DuelClear();

	void SetOpponentUID( MUID OpponentUID );
	void SetAllyUID( MUID AllyUID );

	bool IsOpponent(XObject* pObject);
	bool IsOpponent(MUID OpponentUID);
	bool IsAlly(XObject* pObject);

	bool IsDuel()	{ return m_vecOpponentUID.empty() ? false : true; }

	void RemoveOpponentUID(MUID OpponentUID);
};
