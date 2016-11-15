#pragma once

#include "MTypes.h"

class XObject;

class XMouseBase
{
protected:
	XObject*	m_pOwner;

protected:
	bool GetMouseLine(const LONG sx, const LONG sy, RVector& origin, RVector& to ) const;
	bool GetTrans3DCoordinates( int x, int y, vec3 &vPos, vec3 &vDir );

public:
	XMouseBase(XObject* pOwner) { m_pOwner = pOwner; }
	virtual ~XMouseBase() {}

};