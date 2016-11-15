#include "stdafx.h"
#include "GMasterServerFacade.h"
#include "FForCommandTest.h"
#include "GServerFieldRouter.h"

SUITE(MasterServerFacade)
{
	struct FMasterServerFacade : FForCommandTest
	{
		FMasterServerFacade()
		{
		}
		~FMasterServerFacade()
		{

		}
	};

	TEST_FIXTURE(FMasterServerFacade, TestMasterServerFacade_PostRequestDelDynamicFieldGroup)
	{
		MUID uidFieldGroup;

		GServerFieldRouter* pServerRouter = m_pMasterServerFacade->GetFieldRouter();
		pServerRouter->RequestDelDynamicFieldGroup(uidFieldGroup);

		CHECK_EQUAL(MMC_FIELD_REQUEST_DEL_DYNAMIC_FIELD_GROUP, m_pNetAgent->GetSendCommandID(0));
	}


}