#pragma once

#include "CSChrInfo.h"

class GEntityPlayer;

class GPlayerDoing : public MTestMemPool<GPlayerDoing>
{
public:
	GPlayerDoing(GEntityPlayer*	pOwnerPlayer);
	
	bool			IsBusy();
	bool			IsEndableBusy();

	void			ChangeDoing(CHAR_DOING eDoing);

	void			EndDoing();

	CHAR_DOING		GetCurDoing()				{ return m_eDoing; }
	
	bool			IsNowInteracting()	{ return CD_INTERACTING == m_eDoing; }
	bool			IsNowTrading()		{ return CD_TRADING == m_eDoing; }
	bool			IsNowLooting()		{ return CD_LOOTING == m_eDoing; }
	bool			IsNowCutscening()	{ return CD_CUTSCENING == m_eDoing; }
	bool			IsNowEnchanting()	{ return CD_ENCHANTING == m_eDoing; }
	bool			IsNowDyeing()		{ return CD_DYEING == m_eDoing; }

protected:
	GEntityPlayer*	m_pOwnerPlayer;
	CHAR_DOING		m_eDoing;
};
