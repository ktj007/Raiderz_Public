#pragma once
#ifndef _XSWORDTRAIL_H
#define _XSWORDTRAIL_H

#include "XObject.h"
#include "XDef.h"

class XSwordTrailEffectController;
struct SWORDTRAIL_DATA;

class XSwordTrail
{
private:
	vector<XSwordTrailEffectController * > m_vecSwordTrailEffect;

private:
	SWORDTRAIL_DATA*		GetSwordTrailData(XObject * pObject);
	wstring					GetSwordTrailTexture(XObject * pObject);

public:
	XSwordTrail();
	virtual ~XSwordTrail();

	void					Play(XObject * pObject);
	void					Done();

	void					Destroy();
};

#endif