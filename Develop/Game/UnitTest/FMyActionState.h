#pragma once

#include "FTestGame.h"
#include "FBaseTalentInfo.h"
#include "XMyActionState.h"

class XMyPlayer;

class FMyActionState : public FTestGame, public FBaseTalentInfo
{
public:
	FMyActionState();
	virtual ~FMyActionState();

	XMyPlayer*	m_pMyPlayer;
};