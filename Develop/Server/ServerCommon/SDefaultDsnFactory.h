#ifndef _GDEFAULT_DSNFACTORY_H
#define _GDEFAULT_DSNFACTORY_H


#include "SBaseDsnFactory.h"


class SDefaultDsnFactory : public SBaseDsnFactory, public MTestMemPool<SDefaultDsnFactory>
{
public :
	SDefaultDsnFactory(const mdb::MDatabaseDesc& AccDesc
		, const mdb::MDatabaseDesc& GameDesc
		, const mdb::MDatabaseDesc& LogDesc) 
		: SBaseDsnFactory(SBaseDsnFactory::DEFAULT, AccDesc, GameDesc, LogDesc) 
	{}	

	mdb::MDatabaseDesc GetAccountDSN();
	mdb::MDatabaseDesc GetGameDSN();
	mdb::MDatabaseDesc GetLogDSN();
};


#endif