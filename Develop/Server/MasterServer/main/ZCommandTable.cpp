#include "stdafx.h"
#include "MCommandTable.h"
#include "ZCommandTable.h"
#include "MCommandDesc.h"
#include "MCommand.h"
#include "SCommandTable.h"

using namespace minet;

void ZAddCommandTable()
{
	AddCommonCommandTable();
	AddServerCommonCommandTable();
	AddMaster2GameCommandTable();
	AddMaster2LoginCommandTable();
	AddMaster2AppCommandTable();
}