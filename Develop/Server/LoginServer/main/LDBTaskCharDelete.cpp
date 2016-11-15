#include "stdafx.h"
#include "LDBTaskCharDelete.h"
#include "LDBTaskGlobal.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "LCommandCenter.h"
#include "SCmdRouter_Login.h"
#include "CSMsgCommandHelper.h"



LDBTaskCharDelete::LDBTaskCharDelete() 
: LDBAsyncTask(SDBT_DBTYPE_GAMEDB, SDBTID_CHARDELETE)
{
	m_Data.nReturn = SUCCESS;
}


LDBTaskCharDelete::~LDBTaskCharDelete()
{

}


void LDBTaskCharDelete::Input(const MUID& uidPlayer, const int nIndex)
{
	m_Data.uidPlayer = uidPlayer;
	m_Data.nIndex	= nIndex;
}


void LDBTaskCharDelete::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(CHAR_DELETE), m_Data.nReturn))
	{
		return;
	}

	SetTaskSuccess();
}


mdb::MDB_THRTASK_RESULT LDBTaskCharDelete::_OnCompleted()
{
	Completer completer(m_Data, m_Data.nReturn);
	completer.Do();
	
	return mdb::MDBTR_SUCESS;
}

LDBTaskCharDelete::Completer::Completer( LDBTaskCharDelete::_DATA& Data, int& nReturn) : m_Data(Data)
{

}

void LDBTaskCharDelete::Completer::Do()
{
	LPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_Data.uidPlayer);
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

void LDBTaskCharDelete::Completer::RouteResult(CCommandResultTable nCRT, int nIndex)
{
	if (gsys.pCommandCenter == NULL) return;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponseDeleteChar(m_Data.uidPlayer, nCRT, nIndex);
}
