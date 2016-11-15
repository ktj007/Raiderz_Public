#include "StdAfx.h"
#include "GUTHelper_MasterServer.h"
#include "GMasterServerFacade.h"
#include "GConfig.h"
#include "GProxyFieldManager.h"
#include "STransData.h"

GUTHelper_MasterServer::GUTHelper_MasterServer()
{
}

GUTHelper_MasterServer::~GUTHelper_MasterServer()
{
}

void GUTHelper_MasterServer::AddNewSharedField(MUID uidField, int nFieldID, int nChannelID, int nGameServerID/*=0*/)
{
	TDMG_SHARED_FIELD_INFO tdSharedFieldInfo;
	tdSharedFieldInfo.uidField = uidField;
	tdSharedFieldInfo.nFieldID = nFieldID;
	tdSharedFieldInfo.nChannelID = nChannelID;

	if (0 == nGameServerID)
		tdSharedFieldInfo.nGameServerID = GConfig::m_nMyServerID;
	else
		tdSharedFieldInfo.nGameServerID = nGameServerID;

	uint32 nChecksum = gsys.pMasterServerFacade->GetProxyFieldManager()->CalcChecksumSharedFields();
	SFieldChecksum checksumSharedFields(nChecksum);
	checksumSharedFields.Add(uidField);
	nChecksum = checksumSharedFields.GetValue();

	gsys.pMasterServerFacade->AddProxySharedFields(&tdSharedFieldInfo, 1, nChecksum);
	gsys.pMasterServerFacade->AddManagedSharedFields(&tdSharedFieldInfo, 1);
}
