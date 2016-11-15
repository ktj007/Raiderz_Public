#ifndef _XGAME_PLAY_STATE_H
#define _XGAME_PLAY_STATE_H

#pragma once
#include "XGameState.h"

class XBattleArena;
class XPvPAreaLogic;
class XWorldChanger;

class XGamePlayState : public XGameState
{
protected:
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 오브젝트 캐쉬
	//
	unsigned int				m_nUpdateObjectCacheTime;
	bool						m_bEnableUpdateObjectCache;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// BattleArena
	//
	XBattleArena*				m_pBattleArena;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 정적 필드의 PVP 지역
	//
	XPvPAreaLogic*				m_pPvPAreaLogic;

	XWorldChanger*				m_pWorldChanger;
private:
	bool						_CreateWorld();
	void						_CreateMyPlayer();

public:
	XGamePlayState(XGameStateMgr * pStateMgr, bool bRealGameNotTest = true);
	virtual ~XGamePlayState();

	virtual bool				Create();
	virtual bool				Ready(XGameState* pPreState, int nParam = 0);
	virtual void				LoadResource();
	virtual void				Enter();
	virtual void				Update(float fDelta);
	virtual void				Render();
	virtual void				Exit();
	virtual void				Destroy();

	virtual GAME_PLAY_STATE		GetPlayState()		{ return GPS_GAMEPLAY; }

//////////////////////////////////////////////////////////////////////////
private:
	virtual XNonPlayer*			InNPC(XNPCInfo* pNPCInfo, TD_UPDATE_CACHE_NPC* pMonsterInfo, bool bAppearEffect = true, bool bLoadingAsync = true);
	virtual XBPartNonPlayer*	InBPartNPC(XNPCInfo* pNPCInfo, TD_UPDATE_CACHE_PBART* pBPartInfo, bool bAppearEffect = true, bool bLoadingAsync = true);
	void						OutPlayer( XObject* pObject );
public:
	virtual XPlayer*			InPlayer(MUID& uid, TD_UPDATE_CACHE_PLAYER* pPlayerInfo, TD_PLAYER_FEATURE_TATTOO* pTattooInfo = NULL, bool bAppearEffect = true, bool bLoadingAsync = true);
	virtual void				OutPlayer(MUID uid);
	virtual void				OutPlayer(UIID nPlayerUIID);

	virtual void				InNPCs(TD_UPDATE_CACHE_NPC* pNpcNode, int nNPCCount, bool bAppearEffect = true, bool bLoadingAsync = true);
	virtual void				OutNPC(UIID nNPCUIID);
	virtual void				OutNPC(MUID uid);

	virtual void				InBPartNPCs(TD_UPDATE_CACHE_PBART* pBPartNpcNode, int BPartNpcCount, bool bAppearEffect = true, bool bLoadingAsync = true);
	virtual void				InBPartNPC(TD_UPDATE_CACHE_PBART* pBPartInfo, MMatrix matWorld, vec3 vOrgPos);

	virtual XMagicArea*			InMagicArea(TD_UPDATE_CACHE_BUFFENTITY* pMagicArea, bool bStarted = false);
	virtual void				InMagicAreas(TD_UPDATE_CACHE_BUFFENTITY* pMagicArea, int nCount);
	virtual void				OutMagicArea(MUID& uid);
	virtual void				TrapTriggered(MUID& uid);

	virtual XObject*			CreateObject(XObjectID nObjectID, MUID uid, UIID nUIID=0);
	virtual void				EnableObjectUpdateCache(bool bEnabled)	{ m_bEnableUpdateObjectCache = bEnabled; }

	virtual void				CreateSoulObject( const MUID& uidOwnerNPC, const MUID& uidTarget, int nType, unsigned int nWaitTime =0);
	virtual MUID				CreateFeedbackObject(vec3 vPos, vec3 vDir, wstring strName, wstring strComment);

	virtual XBattleArena*		GetBattleArena()	{ return m_pBattleArena; }
	virtual XPvPAreaLogic*		GetPvPArea()		{ return m_pPvPAreaLogic; }
	virtual void				GetDataForSoundListener(vec3& vPos, vec3& vDir);

	virtual void				ChangeField(int nFieldID, vec3 vMyPos, vec3 vMyDir) override;
	virtual void				OnWorldLoadingComplete();

	virtual void				PlayFieldBGMSound();
	virtual void				UpdateEnvSound();

	virtual void				FieldStartMessage();
};

#endif