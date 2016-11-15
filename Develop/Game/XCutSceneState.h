#ifndef _XCUT_SCENE_STATE_H
#define _XCUT_SCENE_STATE_H

#pragma once
#include "XGameState.h"
#include "XCutSceneWorldResourceMgr.h"

class XCutSceneContent;
class XCutScenePlayer;

class XCutSceneState : public XGameState
{
private:
	XCutSceneWorldResourceMgr	m_WorldResourceMgr;
	XCutSceneContent*			m_pCutSceneContent;

	vec3						m_vStartPos;
	vec3						m_vStartDir;
	vec3						m_vStartUp;

	bool						m_bUseGameWorld;

	int							m_nCutSceneID;

private:
	bool						_CreateWorld();
	void						_DestroyWorld();

	bool						CheckCutSceneObjectLoadingComplete();

	void						SetCameraStartPos();

	void						EndCutScene();

protected:
	virtual XCutScenePlayer*	InCutScenePlayer(MUID& uid);

public:
	XCutSceneState(XGameStateMgr * pStateMgr, bool bRealGameNotTest = true);
	virtual ~XCutSceneState();

	virtual bool				Create();
	virtual bool				Ready(XGameState* pPreState, int nParam = 0);

	virtual void				LoadResource();
	virtual void				Enter();
	virtual void				Update(float fDelta);
	virtual void				Render();
	virtual void				Exit();
	virtual void				Destroy();

	virtual void				OnWorldLoadingComplete();

	virtual void				GetDataForSoundListener(vec3& vPos, vec3& vDir);
	virtual GAME_PLAY_STATE		GetPlayState()		{ return GPS_CUTSCENE; }


	//////////////////////////////////////////////////////////////////////////

	void						SetWorld(XWorld* pWorld){ m_pWorld = pWorld; }
	virtual void				SetControlNetCmd();

	virtual	void				FadeUpdate( float fFadingAlpha );
	virtual	void				FadeEnd();

	bool						OnEvent(MEvent& e);
};

#endif