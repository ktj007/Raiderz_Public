#ifndef _SBASE_DSNFACTORY_H
#define _SBASE_DSNFACTORY_H


#include "..\..\MDK\MAsyncDatabase\MDatabaseDesc.h"


class SBaseDsnFactory
{
public :
	enum DSN
	{
		DEFAULT = 0
		, NEOWIZ
	};

	SBaseDsnFactory(SBaseDsnFactory::DSN DsnID
		, const mdb::MDatabaseDesc& AccDesc
		, const mdb::MDatabaseDesc& GameDesc
		, const mdb::MDatabaseDesc& LogDesc) 
		: m_DsnID(DsnID), m_AccDesc(AccDesc), m_GameDesc(GameDesc), m_LogDesc(LogDesc)
	{}
	virtual ~SBaseDsnFactory() {}

	SBaseDsnFactory::DSN GetDsnID() { return m_DsnID; }

	virtual mdb::MDatabaseDesc GetAccountDSN()	= 0;
	virtual mdb::MDatabaseDesc GetGameDSN()		= 0;
	virtual mdb::MDatabaseDesc GetLogDSN()		= 0;

protected :
	DSN					m_DsnID;
	mdb::MDatabaseDesc	m_AccDesc;
	mdb::MDatabaseDesc	m_GameDesc;
	mdb::MDatabaseDesc	m_LogDesc;
};


#endif