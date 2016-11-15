#pragma once

class GEntityPlayer;

class GDynamicFieldObserver
{
public:
	virtual void OnQuestSynch(GEntityPlayer* pPlayer) {}
	virtual void OnQuestComplete(GEntityPlayer* pPlayer, int nQuestId) {}
	virtual void OnQuestDrop(GEntityPlayer* pPlayer, int nQuestId) {}
	virtual void OnQuestEraseRunner(GEntityPlayer* pPlayer) {}
	virtual bool IsExpiredCounting(void) {return false;	}
};
