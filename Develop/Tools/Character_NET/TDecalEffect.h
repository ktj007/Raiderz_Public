#ifndef _TDECAL_EFFECT_H
#define _TDECAL_EFFECT_H

#pragma once

#include "XEffectBase.h"
#include "RDecalSceneNode.h"
#include "XEffectInfo.h"


class TDecalEffect : public XEffectBase
{
protected:
	DECALEFFECT_TYPE				m_eType;

	MUID							m_uidOwer;

	vec3							m_vPos;
	float							m_fSize;

	std::vector<RVt_pos_tex1>		m_vVertexArray;
	RTexture*						m_hTexture;			//< ÅØ½ºÃÄ.
	DWORD							m_dwColor;

	bool							m_bFollow;

protected:
	virtual void					UpdateFollow();
	virtual void					StopXEffect();

public:
	TDecalEffect();
	virtual ~TDecalEffect();

	bool							Create(MUID& uid, const string& str, BYTE byDecalEffectType, const vec3& vPos, const float fSize);

	void							UpdateXEffect(float fDelta);
	void							RenderXEffect();

	void							SetOwner(MUID uidOwner)		{ m_uidOwer = uidOwner; }
	void							SetDecalEffectData(const vec3& vPos, const float fSize);
	void							SetDecalEffectColor(DWORD dwColor);
	void							SetFollow(bool bFollow) { m_bFollow = bFollow; }

	RDecalSceneNode*				GetEffect()					{ return static_cast<RDecalSceneNode*>(m_pEffect); }
	vec3							GetPosition()				{ return GetEffect()->GetPosition(); }

	virtual bool					IsEffectLive()		{ return true; }
	virtual bool					IsEffectLoop()		{ return true; }
	virtual bool					IsEffectLoadingComplete()	{ return true; }

	virtual EFFECT_TYPE	GetType()	{ return ET_DECAL; }
};

#endif // _TDECAL_EFFECT_H