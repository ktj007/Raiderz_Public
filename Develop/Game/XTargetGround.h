#ifndef _XTARGET_GOUND_H
#define _XTARGET_GOUND_H

#include "XTarget.h"

struct EFFECT_RESULT_DATA;

class XTargetGround : public XTarget
{
private:
	stTargetID					m_Target;

	MUID						m_uidTargetEffect;
	vector<EFFECT_RESULT_DATA>	m_vecDecalEffectData;
	float						m_fSize;

private:
	bool						CheckTerrainTarget(vec3 vStart, vec3 vDir, vec3& vColPos);
	bool						CheckTargetObject(XObject* pObject, vec3& vPos);

	void						ColTerrainPos(vec3 vDir, float fRange, vec3& vColPos);

	void						UpdateGroundTargetEffect(vec3 vPos);
	void						UpdateGroundTargetObjectGlowEffect(vec3 vPos);

	void						SetCameraForTargetGround();
	void						ReleaseCameraForTargetGround();

	bool						GetTerrainPosFromCamera(vec3& vTerrainPos);
	bool						GetTerrainPosFromMouse(vec3& vTerrainPos);

	void						EnableGlowEffect( XObject* pOwner );
	void						DisableGlowEffect( XObject* pOwner );

	void						SetGroundTargetPartyMiniHpbar(XObject* pOwner, bool bShow);
	void						ReleaseGroundTargetPartyMiniHpbar();

public:
	XTargetGround(XObject* pObject);
	virtual ~XTargetGround();

	void						CreateGroundTarget(float fSize);

	virtual void				UpdateTarget();

	virtual stTargetID*			GetTargetInfo()									{ return &m_Target; }

	virtual TARGET_TYPE			GetTargetType()		{ return XTT_GROUND; }
};

#endif // _XTARGET_STICKY_H