#pragma once

#include "SCommObjectManager.h"

/// Client °ü¸®ÀÚ
class LCommObjectManager : public SCommObjectManager
{
public:
	LCommObjectManager();
	virtual ~LCommObjectManager();
	virtual SCommObject* NewObject(MUID& uid);
};


