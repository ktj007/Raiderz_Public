#pragma once

#include "MCommandTable.h"
#include "CCommandTable.h"
#include "CCommandTable_Master.h"
#include "CCommandTable_App.h"
#include "SMinetInitScope.h"

void PAddCommandTable();

class PMinetInitScope : public SMinetInitScope
{
public:
	PMinetInitScope() : SMinetInitScope()
	{
		PAddCommandTable();
	}
};
