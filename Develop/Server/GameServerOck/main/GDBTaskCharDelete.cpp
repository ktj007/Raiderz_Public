#include "stdafx.h"
#include "GDBTaskCharDelete.h"
#include "GDBTaskGlobal.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GCommand.h"
#include "GCommandCenter.h"
#include "CCommandResultTable.h"
#include "SCmdRouter_Login.h"
#include "CSMsgCommandHelper.h"


GDBTaskCharDelete::GDBTaskCharDelete(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_CHARDELETE)
{
	m_Data.uidPlayer = uidReqPlayer;
	m_Data.nReturn = SUCCESS;
}


GDBTaskCharDelete::~GDBTaskCharDelete()
{

}


void GDBTaskCharDelete::Input(const int nIndex)
{
	m_Data.nIndex = nIndex;
}


void GDBTaskCharDelete::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDatabaseQuery dbq(&rfDB);
	if (!ExecuteW(dbq, GetSQLW(CHAR_DELETE), m_Data.nReturn))
		return;	
}


mdb::MDB_THRTASK_RESULT GDBTaskCharDelete::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();
	
	return mdb::MDBTR_SUCESS;
}

GDBTaskCharDelete::Completer::Completer( GDBTaskCharDelete::_DATA& Data) : m_Data(Data)
{

}

void GDBTaskCharDelete::Completer::Do()
{
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_Data.uidPlayer);
	if (NULL == pPlayerObject)
	{
		return;
	}

	RETURN nReturn = static_cast<RETURN>(m_Data.nReturn);

	if (FAIL_GUILDMASTER == nReturn)
	{
		RouteResult(CR_FAIL_CHAR_CANNOT_DELETE_GUILDMASTER_CHAR, m_Data.nIndex);
	}
	else if (SUCCESS == nReturn)
	{
		pPlayerObject->RemoveCharFieldInfo(m_Data.nIndex);
		RouteResult(CR_SUCCESS, m_Data.nIndex);
	}	
}

void GDBTaskCharDelete::Completer::RouteResult(CCommandResultTable nCRT, int nIndex)
{
	if (gsys.pCommandCenter == NULL) return;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponseDeleteChar(m_Data.uidPlayer, nCRT, nIndex);
}
