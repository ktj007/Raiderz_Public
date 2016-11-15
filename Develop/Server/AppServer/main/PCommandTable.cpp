#include "stdafx.h"
#include "MCommandTable.h"
#include "PCommandTable.h"
#include "MCommandDesc.h"
#include "MCommand.h"
#include "CCommandTable_Log.h"
#include "SCommandTable_App.h"
#include "SCommandTable.h"

using namespace minet;

void PAddCommandTable()
{
	AddCommonCommandTable();
	AddServerCommonCommandTable();
	AddMaster2AppCommandTable();
	AddApp2GameCommandTable();
	AddLogCommandTable();	
}
