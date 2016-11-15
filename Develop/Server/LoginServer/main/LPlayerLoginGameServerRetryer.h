#pragma once

#include "MTime.h"

class LPlayerLogic;

class LPlayerLoginGameServerRetryer
{
	struct RETRYER_NODE
	{
		MUID uidPlayer;
		int nCharIndex;
	};
	typedef std::list<RETRYER_NODE> LRetryerList;
public:
	static const float TIME_TO_UPDATE_SEC;

public:
	LPlayerLoginGameServerRetryer();
	~LPlayerLoginGameServerRetryer();

	void Update(float fDelta);
	void AddRetryer(MUID uidPlayer, int nCharIndex);
	void DelRetryer(MUID uidPlayer);

protected:
	LRetryerList	m_listRetryer;

private:
	MRegulator		m_rgltRetry;

	LPlayerLogic*	m_pPlayerLogic;

};
