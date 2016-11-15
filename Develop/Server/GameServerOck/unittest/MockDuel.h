#pragma once

#include "GDuel.h"

class MockDuel : public GDuel
{
public:
	MockDuel(MUID uid, GField* pDuelField, GEntityPlayer* pFighter1, GEntityPlayer* pFighter2);
	virtual ~MockDuel(void);

	void SetState(DUEL_STATE nState);
};
