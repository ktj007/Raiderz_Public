#pragma once

#include "RTypes.h"
#include "RViewFrustum.h"

class CEntityObject;
class CControlMediator;

class CLogicObject_ForIconSelect
{
private:
	bool				m_bUse;
	bool				m_bReady;

public:
	float				m_fSize;
	float				m_fOffset;

private:
	void				UpdateSizeForIconSelection(CEntityObject* pObject, CControlMediator* pControlMediator_);

public:
	CLogicObject_ForIconSelect();
	virtual ~CLogicObject_ForIconSelect();


	void				Update(CEntityObject* pObject, CControlMediator* pControlMediator_);
	void				UpdateSmartOffsetHeightForIconSelection(CEntityObject* pObject, CControlMediator* pControlMediator_);

	bool				PickForSelection(CEntityObject* pObject, CControlMediator* pControlMediator_, float& _rPrevPickedSqLength, const rs3::RVector &origin, const rs3::RVector &to);
	bool				PickForSelection(CEntityObject* pObject, CControlMediator* pControlMediator_, rs3::RViewFrustum& _refFrustum);
	bool				PickZone(CControlMediator* pControlMediator_, vec3 vOrg, vec3 vTo, float& fOutDist, vec3& vOutColPos);

	void				DoUse()	{ m_bUse = true; }
};