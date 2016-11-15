#pragma once

#include "XInteractionEffect.h"

class MockInteractionEffect : public XInteractionEffect
{
public:

public:
	MockInteractionEffect(XObject* pObject);
	virtual ~MockInteractionEffect();

	virtual void	Init(wstring strTextureName, vec3 vPos, vec3 vDir, float fSize);
	virtual void	CreateInteractionEffect(INTERACTION_EFFECT_TYPE eType);

};
