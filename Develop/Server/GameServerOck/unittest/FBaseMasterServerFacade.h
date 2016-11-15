#pragma once

#include "GTestWrapper.h"
#include "GMasterServerFacade.h"
#include "MockMasterServerFacade.h"

struct FBaseMasterServerFacade
{
	FBaseMasterServerFacade();
	virtual ~FBaseMasterServerFacade();

	GTestSysWrapper2<GMasterServerFacade, MockMasterServerFacade>	m_MasterServerFacadeWrapper;
	GMasterServerFacade*											m_pMasterServerFacade;

private:
	bool m_bOldStandAlone;
};
