#pragma  once
#include "XMovableFactor.h"

class XMovableProcObjectChecker
{
public:
	XMovableProcObjectChecker(){}
	virtual ~XMovableProcObjectChecker(){}

	bool		CheckObjectCollision(const XMovableFactor& _rMovableFactor, XObject* pObejct);
	bool		CheckCanRub(XMovableFactor& _rMovableFactor, XObject* pObejct, int nCurrentTalentID);
	bool		CheckCapsuleRadiusOffset(const XMovableFactor& _rMovableFactor, XObject* pObejct);
};
