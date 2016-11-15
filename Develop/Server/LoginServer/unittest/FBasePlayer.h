#pragma once

#include "LTestWrapper.h"
#include "LPlayerObjectManager.h"

class LPlayerObject;

class FBasePlayer
{
public:
	FBasePlayer()			{}
	virtual ~FBasePlayer()	{}

	LPlayerObject* NewTestPlayerObject(MUID uidPlayer=MUID::ZERO);
	LPlayerObject* NewLoginedTestPlayerObject(MUID uidPlayer=MUID::ZERO, AID nAID=0);

	LTestMgrWrapper<LPlayerObjectManager> m_PlayerObjectManager;
};
