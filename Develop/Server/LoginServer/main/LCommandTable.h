#pragma once

#include "MCommandTable.h"
#include "CCommandTable.h"
#include "CCommandTable_Login.h"
#include "CCommandTable_Master.h"
#include "SMinetInitScope.h"

void LAddCommandTable();

class LMinetInitScope : public SMinetInitScope
{
public:
	LMinetInitScope() : SMinetInitScope()
	{
		LAddCommandTable();
	}
};
