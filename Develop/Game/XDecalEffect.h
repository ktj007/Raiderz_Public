#ifndef _XDECAL_EFFECT_H
#define _XDECAL_EFFECT_H

#pragma once

#include "XEffectBase.h"
#include "RDecalProjectionSceneNode.h"
#include "XEffectInfo.h"


class XDecalEffect : public XEffectBase
{
protected:
	DECALEFFECT_TYPE				m_eType;

	MUID							m_uidOwer;

	float							m_fSizeX, m_fSizeY, m_fSizeZ;

protected:
	virtual void					UpdateFollow();
	virtual void					StopXEffect();

public:
	XDecalEffect();
	virtual ~XDecalEffect();

	bool							Create(MUID& uid, const wstring& str, BYTE byDecalEffectType, const vec3& vPos, const float fSizeX, const float fSizeY, const float fSizeZ);

	void							UpdateXEffect(float fDelta);

	void							SetOwner(MUID uidOwner)		{ m_uidOwer = uidOwner; }
	void							SetDecalEffectData(const vec3& vPos, const float fSizeX, const float fSizeY, const float fSizeZ);
	void							SetDecalEffectColor(DWORD dwColor);

	RDecalProjectionSceneNode*		GetEffect()					{ return static_cast<RDecalProjectionSceneNode*>(m_pEffect); }
	vec3							GetPosition()				{ return GetEffect()->GetPosition(); }

	virtual bool					IsEffectLive();
	virtual bool					IsEffectLoop()		{ return true; }
	virtual bool					IsEffectLoadingComplete()	{ return true; }

	virtual void					CheckOwerChangeMotion(MUID uidOwner);

	virtual EFFECT_TYPE	GetType()	{ return ET_DECAL; }
};

#endif // _XDECAL_EFFECT_H