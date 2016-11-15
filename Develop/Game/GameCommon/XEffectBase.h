#ifndef _XEFFECT_BASE_H
#define _XEFFECT_BASE_H

#pragma once

#include "RTypes.h"
#include "RTimer.h"
#include "RResource.h"
#include "RSceneNode.h"
#include "XFader.h"
#include "MTstring.h"

using namespace rs3;

enum EFFECT_TYPE
{
	ET_NONE,
	ET_MODEL,
	ET_DECAL,
	ET_GLOW,
	ET_LIGHT, 
	ET_ALPHATESTFADE,
	ET_WEAPON,
};

class XEffectBase
{
protected:
	MUID				m_UID;

	RSceneNode*			m_pEffect;
	tstring				m_strEffectFileName;

	XFader				m_Fader;

	float				m_fDurationTime;
	float				m_fEndVisibilityTime;

	bool				m_bUseDurationTime;
	bool				m_bDelMe;
	bool				m_bUseVisibility;
	bool				m_bCancelEnabled;

	tstring				m_strBoneName;

	RMatrix				m_matLocal;

protected:
	void				UpdateDurationTime(float fDelta);
	void				UpdateEffectFader(float fDelta);

	virtual void		StopXEffect() {}

public:
	XEffectBase();
	virtual ~XEffectBase();

	virtual bool		Create(MUID& uid, const tstring& str)	{ return false; }
	virtual void		Stop();
	virtual void		UpdateXEffect(float fDelta) {}
	
	virtual void		CheckOwerChangeMotion(MUID uidOwner) {}

	MUID				GetMUID()			{ return m_UID;}
	tstring				GetEffectFileName()	{ return m_strEffectFileName; }
	virtual bool		GetImmediatlyDelete() { return false; }
	float				GetEndVisibilityTime() { return m_fEndVisibilityTime; }
	void				InvalidateEffect()	{ m_pEffect = NULL; }

	virtual bool		IsEffectLive()		{ return false; }
	virtual bool		IsEffectLoadingComplete() { return false; }
	virtual bool		IsEffectLoop()		{ return false; }
	bool				IsEffectLiveTime();
	bool				IsCancelEnabled()	{ return m_bCancelEnabled; }

	void				SetDurationTime(float fDurationTime);
	void				SetBoneName(tstring& strName);
	void				SetUseVisibility(float fStartTime, float fEndTime);
	void				SetLocalMatrix(RMatrix& mat)		{ m_matLocal = mat; }
	void				SetCancelEnabled(bool cancelEnabled) { m_bCancelEnabled = cancelEnabled; }

	virtual EFFECT_TYPE	GetType()	{ return ET_NONE; }
};



#endif