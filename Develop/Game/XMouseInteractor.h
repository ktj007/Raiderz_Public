#pragma once

#include "XMouseBase.h"
#include "MTypes.h"

class XObject;

class XMouseInteractor : public XMouseBase
{
private:
	void InteractWith(XObject* pTarget);

	bool CheckLineTarget( XObject* pObject, vec3& pos, vec3& dir, float& fOutDist );
	bool CheckInteractionObject( vec3 vPos, vec3 vDir );
	bool CheckMyCharacterAction(XModuleMyControl* pModuleMyControl);
	bool CheckInteractionDistance(vec3 vPos);
	bool CheckInteractionableObject(XObject* pObject);
	XObject* CheckObject( vec3 vPos, vec3 vDir );

	void SetDefaultCursor();

public:
	XMouseInteractor(XObject* pOwner):XMouseBase(pOwner) {}
	virtual ~XMouseInteractor() {}

	bool OnClick(int x, int y);
	XObject* OnOver( int x, int y );

	void	Update();
};