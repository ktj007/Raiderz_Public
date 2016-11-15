#ifndef _XMAGICAREA_EFFECT_H
#define _XMAGICAREA_EFFECT_H

#include "RDecalProjectionSceneNode.h"

struct EFFECT_RESULT_DATA;

class XMagicAreaEffect
{
private:
	XObject*				m_pObject;			///< 적용되는 오브젝트

	vec3					m_vCenter;
	vec3					m_vDir;
	float					m_fSize;

	vector<EFFECT_RESULT_DATA>	m_vecDecalEffectData;

	// 디버그
	bool					m_bDebug;
	RDecalProjectionSceneNode* m_pDebugDecalEffect;

private:
	void					DebugMagicArea();

public:
	XMagicAreaEffect();
	virtual ~XMagicAreaEffect();

	void					Create(XObject* pMagicArea, XBuffInfo* pBuffInfo, vec3 vCenter, vec3 vDir);
	void					Update(float fDelta);
	void					Destroy();

	void					CheckRemainTimeEffect(float fRemainTime);
	
	void					SetDebugMagicArea(bool bSet) { m_bDebug = bSet; }
};

#endif // _XMAGICAREA_EFFECT_H
