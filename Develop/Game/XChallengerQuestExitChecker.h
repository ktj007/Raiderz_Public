#pragma once

class XChallengerQuestExitChecker
{
	list<int>	m_vecCheckTime;
	MRegulator	m_Regulator;
	bool		m_bHide;

	bool			CheckTime(int nTickTime);
	void			UIVictoryHide();

public:
	XChallengerQuestExitChecker(void);
	virtual ~XChallengerQuestExitChecker(void);

	void			Update(float fDelta);
	void			Start();
	void			Stop();
};
