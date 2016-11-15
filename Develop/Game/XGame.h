#ifndef _XGAME_H
#define _XGAME_H

#include "XGlobal.h"
#include "XObjectManager.h"
#include "CTransData.h"
#include "XGameStateMgr.h"
#include "XGameState.h"

class XWorld;
class XNonPlayer;
class XPlayer;
class XNPCInfo;
class XMagicArea;
class XBattleArena;
class XPvPAreaLogic;

using namespace rs3;

/// 실제 게임 클래스(MVC의 Model에 해당함)
class XGame
{
private:
	bool					m_bCreated;

protected:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 오브젝트 / 이팩트
	//
	XObjectManager			m_ObjectManager;


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 오브젝트 캐쉬
	//
	unsigned int			m_nUpdateObjectCacheTime;
	bool					m_bEnableUpdateObjectCache;
	void					UpdateObjectCache();

	//////////////////////////////////////////////////////////////////////////
	// 게임 스테이트
	XGameStateMgr			m_GameStateMgr;
	XGameState*				m_pCurrentGameState;


	void					UpdateMyInfo( float fDelta );
	void					UpdateFeedbackObjects( float fDelta );

	void					ChangeState(GAME_PLAY_STATE eState, int nParam = 0);

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	XGame(bool bRealGameNotTest = true);
	virtual ~XGame();

	bool					Create();
	void					Destroy();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 게임 업데이트 / 렌더링
	//
	void					Update(float fDelta);
	bool					OnEvent(MEvent& e);
	bool					OnForceEvent(MEvent& e);

	void					OnPreRender();
	void					OnRender();

	void					OnActive(bool bActive);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 게임 상태 초기화
	//
	void					ChangeField(int nNewFieldID, vec3 vMyPos, vec3 vMyDir);
	void					OnWorldLoadingComplete();

	//////////////////////////////////////////////////////////////////////////
	// 게임 스테이트
	//
	void					OnChangeStart(GAME_PLAY_STATE ePlayState, int nParam = 0);
	void					OnChangeEnd();
	XGameState*				GetCurrentGameState()	{ return m_pCurrentGameState; }


	//////////////////////////////////////////////////////////////////////////
	// 기타
	bool					Check18Over();


	// 테스트로 여기다 넣음. - 조만간 다른 곳으로 옮길예정
	void SetShowFeedback(bool bShow);

};

#endif // _XGAME_H