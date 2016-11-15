#ifndef _XMODEL_EFFECT_BASE_H
#define _XMODEL_EFFECT_BASE_H

#pragma once

#include "XEffectBase.h"
#include "REffectSceneNode.h"

using namespace rs3;

class XModelEffect : public XEffectBase
{
protected:
	vec3				m_vScale;
	vec3				m_vInverseScale;

	bool				m_bImmediatlyDelete;

protected:
	virtual void		StopXEffect();

public:
	XModelEffect();
	virtual ~XModelEffect();

	REffectSceneNode*	GetEffect()			{ return static_cast<REffectSceneNode*>(m_pEffect); }
	vec3				GetScale()			{ return m_vScale; }
	vec3				GetInverseScale()	{ return m_vInverseScale; }
	virtual bool		GetImmediatlyDelete() { return m_bImmediatlyDelete; }

	virtual bool		IsEffectLive();
	virtual bool		IsEffectLoadingComplete();
	virtual bool		IsEffectLoop();

	void				SetLastFramePause(bool bPause);
	void				SetScale(float scale);
	void				SetScale(vec3 scale);
	void				SetInverseScale(vec3 scale);
//	void				SetIgnoreParentActorNodeDir(bool bIgnoreDir);
	void				SetImmediatlyDelete(bool bDelType);

	virtual EFFECT_TYPE	GetType()	{ return ET_MODEL; }
};



#endif