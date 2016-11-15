#include "stdafx.h"
#include "PCheckerFactory.h"
#include "PCommandVersionChecker.h"
#include "PWorldIDChecker.h"

PLogicChecker* PCheckerFactory::NewCommandVersionChecker(MUID uidClient, int nCommandVersion)
{	
	return new PCommandVersionChecker(uidClient, nCommandVersion);
}

PLogicChecker* PCheckerFactory::NewWorldIDChecker(MUID uidClient, int nWorldID)
{
	return new PWorldIDChecker(uidClient, nWorldID);
}
