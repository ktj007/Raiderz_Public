#pragma once

class PLogicChecker;

class PCheckerFactory
{
public:
	static PLogicChecker* NewCommandVersionChecker(MUID uidClient, int nCommandVersion);
	static PLogicChecker* NewWorldIDChecker(MUID uidClient, int nWorldID);
};
