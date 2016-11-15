#ifndef _WS_EFFECT_H
#define _WS_EFFECT_H

#pragma once

#include "../../../Game/GameCommon/XModelEffect.h"

class CORETOOL_SOULHUNT_API WS_Effect : public XModelEffect
{
public:
	WS_Effect();
	virtual ~WS_Effect();

	bool				Create(MUID& uid, const string& str);
	void				UpdateXEffect(float fDelta);
};



#endif