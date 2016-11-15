#include "stdafx.h"
#include "GMasterServerFacade.h"
#include "MockMasterNetClient.h"
#include "CCommandResultTable.h"
#include "GCommandTable.h"
#include "FBaseNetClient.h"


SUITE(MasterServer_OnCommand)
{
	struct FMasterServerOnCommand : public FBaseNetClient
	{
		FMasterServerOnCommand()
		{

		}
		virtual ~FMasterServerOnCommand()
		{

		}
	};

	TEST_FIXTURE(FMasterServerOnCommand, TestCmdHandler_MMC_COMM_RESPONSE_LOGIN)
	{
		int nParamResult = CR_SUCCESS;
		MUID uidParamAlloc = MUID(5, 6);
		TDMG_MANAGED_SHARED_FIELD paramBlob1;

		m_pNetAgent->OnRecv(MMC_COMM_RESPONSE_LOGIN_M2G, 3, NEW_INT(nParamResult), NEW_UID(uidParamAlloc), NEW_BLOB(&paramBlob1, sizeof(TDMG_MANAGED_SHARED_FIELD), 1) );

	}


}