#pragma once

#include "GTestWrapper.h"
#include "GAppServerFacade.h"
#include "MockAppServerFacade.h"

struct FBaseAppServerFacade
{
	FBaseAppServerFacade();
	virtual ~FBaseAppServerFacade();

	GTestSysWrapper2<GAppServerFacade, MockAppServerFacade>			m_AppServerFacadeWrapper;
	GAppServerFacade*												m_pAppServerFacade;
};
