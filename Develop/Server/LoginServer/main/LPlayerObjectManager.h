#pragma once

#include "SCommObjectManager.h"
#include "LDef.h"
#include "MSync.h"

class LPlayerObject;

/// Client °ü¸®ÀÚ
class LPlayerObjectManager : public SCommObjectManager
{
public:
	friend class LServer;
public:
	LPlayerObjectManager();
	virtual ~LPlayerObjectManager();
	virtual SCommObject* NewObject(MUID& uid);
	LPlayerObject* GetPlayer( const MUID& uidPlayer );
};
