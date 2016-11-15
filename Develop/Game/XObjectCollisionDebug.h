#pragma once
#ifndef _XOBJECT_COLLISION_DEBUG_H
#define _XOBJECT_COLLISION_DEBUG_H

#include "RCollision.h"

struct stObjectCollisionInvestigateData
{

	MCapsule					capStart;
	MCapsule					capEnd;

	MCapsule					capColCapsule;
	MCapsule					capTargetColCapsule;

	vec3						vNear;
	vec3						vNear2;
	vec3						vColPos;
	vec3						vResultPos;

	float						fDist;

	bool						bCol;
	bool						bSet;

	void CapsuleInit(MCapsule& capsule)
	{
		capsule.top			= vec3::ZERO;
		capsule.bottom		= vec3::ZERO;

		capsule.height		= 0.0f;
		capsule.radius		= 0.0f;
	}

	void Init()
	{
		bCol					= false;
		bSet					= false;

		fDist					= 0.0f;

		CapsuleInit(capStart);
		CapsuleInit(capEnd);

		CapsuleInit(capColCapsule);
		CapsuleInit(capTargetColCapsule);

		vNear					= vec3::ZERO;
		vNear2					= vec3::ZERO;
		vColPos					= vec3::ZERO;
		vResultPos				= vec3::ZERO;
	}
};

class XObjectCollisionDebug
{
private:
	bool						m_bCollisionInvestigate;
	bool						m_bRenderCollisionInvestigate;

	int							m_nRenderCollisionInvestigateIndex;

	XObject*					m_pOwner;

public:
	vector<stObjectCollisionInvestigateData> m_vecObjectCollisionInvestigate;

	stObjectCollisionInvestigateData m_TempData;

private:

public:
	XObjectCollisionDebug();
	virtual ~XObjectCollisionDebug();

	void						Init(XObject* pOwner);
	void						AddData();
	void						Render();

	void						Show(bool bInvestigate, int nIndex);
	void						ShowCollisionInvestigate(bool bShow);

	void						SetCollisionInvestigateData(MCapsule colCapsule, MCapsule colTargetCapsule, vec3 vNear1, vec3 vNear2, vec3 vResultPos, float fDist);
	void						SetCollisionInvestigateData(stObjectCollisionInvestigateData& setData);
	void						SetCollisionInvestigateData_ResultPos(vec3& vStart, vec3& vTo, vec3& vImpactDir, float fImpactDist);
};

#endif