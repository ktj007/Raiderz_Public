#ifndef _XCUTSCENE_PLAYER_H
#define _XCUTSCENE_PLAYER_H

#pragma once
#include "XActor.h"
#include "XNPCInfo.h"

class XModuleCutSceneControl;

class XCutScenePlayer : public XActor, public MMemPool<XCutScenePlayer>
{
	MDeclareRTTI;

protected:
	XModuleCutSceneControl*			m_pModuleCutSceneControl;

	XNPCInfo*						m_pNPCInfo;
	int								m_nMode;

protected:
	virtual void					RegisterModules();
	virtual void					UnregisterModules();
	virtual bool					OnCreate();

public:
	XCutScenePlayer(MUID uid, const std::wstring& _rStrActorType);
	virtual ~XCutScenePlayer();
	virtual XObjectID				GetType() { return XOBJ_CUTSCENE_PLAYER; }
	
	virtual bool					IsPlayer() { return (m_nNpcId == -1); }
	virtual bool					IsNPC(){ return (m_nNpcId != -1); }
	int								GetMode()		{ return m_nMode; }

	XNPCInfo*						GetInfo(bool bCurrentMode=false);

	//////////////////////////////////////////////////////////////////////////
	virtual WEAPON_TYPE				GetCurrWeaponType() { return gvar.MyInfo.EquipmentSlot.GetCurrWeaponType(); }
	CHAR_STANCE						GetStance()			{ return CS_BATTLE; }

	void							SerializeCutScenePlayer();
	void							SetTransform( const vec3& vPos, const vec3& vDir, bool bUsingCollision );
	void							SetAnimation( const std::wstring& _rAnimationName);
	void							OnUseTalent( const int nTalent);

private:
	int		m_nNpcId;
};
#endif