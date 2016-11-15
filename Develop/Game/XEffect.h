#ifndef _XEFFECT_H
#define _XEFFECT_H

#pragma once

#include "XModelEffect.h"

class XEffect : public XModelEffect
{
protected:
	MUID				m_uidOwer;

	bool				m_bFollowTerrainAlign;
	bool				m_bFollowTerrainPos;
	bool				m_bFollowWaterPos;

	float				m_fFollowWaterHeightMin;
	float				m_fFollowWaterHeightMax;

	MMatrix				m_matLocal;

protected:
	void				UpdateFollowTerrainAndWater();
	void				UpdateActorNode();

public:
	XEffect();
	virtual ~XEffect();

	virtual bool		Create(MUID& uid, const wstring& str);
	virtual void		UpdateXEffect(float fDelta);

	virtual void		CheckOwerChangeMotion(MUID uidOwner);

	void				SetOwner(MUID uidOwner);
	void				SetFollowTerrainAlign(bool bTerrainAlign, bool bTerrainPos);
	void				SetFollowWaterPos(bool bWaterPos);
	void				SetFollowWaterHeight(float fMin, float fMax) { m_fFollowWaterHeightMin = fMin, m_fFollowWaterHeightMax = fMax; }
	void				SetLocalMatrix(MMatrix& mat) { m_matLocal = mat; }
};



#endif