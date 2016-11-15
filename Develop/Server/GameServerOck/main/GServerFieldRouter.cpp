#include "stdafx.h"
#include "GServerFieldRouter.h"
#include "GCommand.h"
#include "GGlobal.h"
#include "GMasterServerFacade.h"
#include "CCommandTable_Master.h"
#include "GConfig.h"

GServerFieldRouter::GServerFieldRouter()
{
	// do nothing
}

GServerFieldRouter::~GServerFieldRouter()
{
	// do nothing
}

void GServerFieldRouter::RequestAllFieldInfo(void)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_FIELD_REQUEST_SHARED_FIELD_INFO_ALL);
	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GServerFieldRouter::RequestAllFieldGroupInfo(void)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_FIELD_REQUEST_DYNAMIC_FIELD_GROUP_INFO_ALL);
	gsys.pMasterServerFacade->Route(pNewCmd);
}

void GServerFieldRouter::RequestDelDynamicFieldGroup(MUID uidFieldGroup)
{
	MCommand* pNewCmd = gsys.pMasterServerFacade->MakeNewCommand(MMC_FIELD_REQUEST_DEL_DYNAMIC_FIELD_GROUP,
													1,
													NEW_UID(uidFieldGroup)
													);

	gsys.pMasterServerFacade->Route(pNewCmd);
}
