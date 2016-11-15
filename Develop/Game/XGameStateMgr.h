#ifndef _XGAME_STATE_MANAGER_H
#define _XGAME_STATE_MANAGER_H

#pragma once

#include "XGameStateSwitchResourceMgr.h"

enum GAME_PLAY_STATE;
class XGameState;

class XGameStateMgr
{
private:
	GAME_PLAY_STATE					m_eCurrentGamePlayState;


public:
	vector<XGameState*>				m_vecGameState;
	XGameStateSwitchResourceMgr		m_ResourceMgr;
	XGameStateSwitchResourceMgr		m_TempResourceMgr;

private:
	XGameState*						GetGameState(GAME_PLAY_STATE eState);

public: 
	XGameStateMgr();
	virtual ~XGameStateMgr();

	XGameState*						CreateGameState(bool bRealGameNotTest);
	void							DestroyGameState();

	XGameState*						ChangeGameStateStart(GAME_PLAY_STATE eState, int nParam = 0);
	void							ChangeGameStateEnd();

	XGameState*						GetCurrentGameState();
	GAME_PLAY_STATE					GetCurrentPlayState()		{ return m_eCurrentGamePlayState; }
};

#endif