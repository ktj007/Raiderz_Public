#ifndef _XGAME_STATE_H
#define _XGAME_STATE_H

#pragma once
#include "XWorld.h"
#include "XControlNetCmdState.h"

class XGameStateMgr;
class XNPCInfo;
class XBattleArena;
class XPvPAreaLogic;
class XMagicArea;
class XSensorManager;
class XScreenFader;

enum GAME_PLAY_STATE
{
	GPS_NONE,
	GPS_GAMEPLAY,
	GPS_CUTSCENE,
};

class XGameState
{
protected:
	XWorld*						m_pWorld;
	XController*				m_pController;
	XObjectManager*				m_pObjectManager;
	XGameStateMgr *				m_pOwnerStateMgr;
	XSensorManager*				m_pSensorManager;
	XScreenFader*				m_pScreenFader;

	bool						m_bRealGameNotTest;

	XControlNetCmdState			m_ControlNetCmdState;

public:
	XGameState(XGameStateMgr * pStateMgr);
	virtual ~XGameState();

	virtual bool				Create();
	virtual bool				Ready(XGameState* pPreState, int nParam = 0);
	virtual void				LoadResource()				{ return; }
	virtual void				Enter()						{ return; }
	virtual void				Update(float fDelta);
	virtual void				Render() {}
	virtual void				Exit();
	virtual void				Destroy();

	virtual GAME_PLAY_STATE		GetPlayState()		{ return GPS_NONE; }
	virtual void				SetControlNetCmd()	{}

// XGamePlayState 쓰이는 함수
private:
	virtual XNonPlayer*			InNPC(XNPCInfo* pNPCInfo, TD_UPDATE_CACHE_NPC* pMonsterInfo, bool bAppearEffect = true, bool bLoadingAsync = true)		{ return NULL; }
	virtual XBPartNonPlayer*	InBPartNPC(XNPCInfo* pNPCInfo, TD_UPDATE_CACHE_PBART* pBPartInfo, bool bAppearEffect = true, bool bLoadingAsync = true)		{ return NULL; }

protected:
	bool						CheckModelLoadingAsync(SEX nSex);
	bool						CheckModelLoadingAsync(const wstring& strResourceName);

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 캐릭터 / 오브젝트 / 이팩트
	//
	virtual XPlayer*			InPlayer(MUID& uid, TD_UPDATE_CACHE_PLAYER* pPlayerInfo, TD_PLAYER_FEATURE_TATTOO* pTattooInfo = NULL, bool bAppearEffect = true, bool bLoadingAsync = true)	{ return NULL; }
	virtual void				OutPlayer(MUID uid)																					{}
	virtual void				OutPlayer( UIID nPlayerUIID )																		{}

	virtual void				InNPCs(TD_UPDATE_CACHE_NPC* pNpcNode, int nNPCCount, bool bAppearEffect = true, bool bLoadingAsync = true)				{}
	virtual void				OutNPC(UIID nNPCUIID)																						{}
	virtual void				OutNPC(MUID uid) {}

	virtual void				InBPartNPCs(TD_UPDATE_CACHE_PBART* pBPartNpcNode, int BPartNpcCount, bool bAppearEffect = true, bool bLoadingAsync = true)	{}
	virtual void				InBPartNPC(TD_UPDATE_CACHE_PBART* pBPartInfo, MMatrix matWorld, vec3 vOrgPos)												{}

	virtual XMagicArea*			InMagicArea(TD_UPDATE_CACHE_BUFFENTITY* pMagicArea, bool bStarted = false)								{ return NULL; }
	virtual void				InMagicAreas(TD_UPDATE_CACHE_BUFFENTITY* pMagicArea, int nCount)											{}
	virtual void				OutMagicArea(MUID& uid)																					{}
	virtual void				TrapTriggered(MUID& uid)																				{}

	virtual XObject*			CreateObject(XObjectID nObjectID, MUID uid, UIID nUIID=0)														{ return NULL; }
	XObjectManager*				GetObjectManager()																						{ return m_pObjectManager; }
	virtual void				EnableObjectUpdateCache(bool bEnabled)																	{}

	virtual void				CreateSoulObject( const MUID& uidOwnerNPC, const MUID& uidTarget, int nType, unsigned int nWaitTime =0)	{}
	virtual MUID				CreateFeedbackObject(vec3 vPos, vec3 vDir, wstring strName, wstring strComment)							{ return MUID::ZERO; }

	virtual XSensorManager*		GetSensorManager()	{ return m_pSensorManager; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 맵 / 월드
	//
	XWorld*						GetWorld()																								{ return m_pWorld; }
	virtual XBattleArena*		GetBattleArena()																						{ return NULL; }
	virtual XPvPAreaLogic*		GetPvPArea()																							{ return NULL; }

	virtual void				ChangeField(int nNewFieldID, vec3 vMyPos, vec3 vMyDir)													{}
	virtual void				OnWorldLoadingComplete();

	virtual void				PlayFieldBGMSound()																						{}
	virtual void				UpdateEnvSound()																						{}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 컨트롤
	//
	XController*				GetController()																							{ return m_pController; }

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// 
	virtual void				GetDataForSoundListener(vec3& vPos, vec3& vDir)															{}
	bool						GetRealGameNotTest() { return m_bRealGameNotTest; }

	virtual void				FieldStartMessage()																						{}
	
	void						ChangeTimeWeather(GAME_TIME_TYPE nGameTime, GAME_WEATHER_TYPE nWeather);

	virtual XScreenFader*		GetFader()	{ return m_pScreenFader; }
	virtual	void				FadeUpdate( float fFadingAlpha )	{}
	virtual	void				FadeEnd()							{}
};

#endif