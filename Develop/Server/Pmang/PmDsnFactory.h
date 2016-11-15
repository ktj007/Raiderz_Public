#ifndef _PM_DSNFACTORY_H
#define _PM_DSNFACTORY_H


#include "SBaseDsnFactory.h"
#include "MTstring.h"

class PmDsnFactory : public SBaseDsnFactory
{
public :
	PmDsnFactory(const mdb::MDatabaseDesc& AccDesc
		, const mdb::MDatabaseDesc& GameDesc
		, const mdb::MDatabaseDesc& LogDesc);
	virtual ~PmDsnFactory();

	SBaseDsnFactory::DSN GetDsnID() { return NEOWIZ; }

	mdb::MDatabaseDesc GetAccountDSN();
	mdb::MDatabaseDesc GetGameDSN();
	mdb::MDatabaseDesc GetLogDSN();


protected :
	std::wstring Decrypt(const std::wstring& str);	
};



#endif