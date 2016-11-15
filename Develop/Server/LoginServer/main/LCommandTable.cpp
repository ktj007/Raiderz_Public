#include "stdafx.h"
#include "MCommandTable.h"
#include "LCommandTable.h"
#include "SCommandTable_App.h"
#include "MCommandDesc.h"
#include "MCommand.h"
#include "SCommandTable.h"
#include "CCommandTable_WorldLocator.h"

using namespace minet;

void LAddCommandTable()
{
	AddCommonCommandTable();
	AddServerCommonCommandTable();
	AddLoginCommandTable();	
	AddMaster2LoginCommandTable();
	AddWorldLocatorCommandTable();
}
