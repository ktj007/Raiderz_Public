#include "stdafx.h"
#include "LDBTaskCharInsert.h"
#include "LDBTaskGlobal.h"
#include "LPlayerObject.h"
#include "LPlayerObjectManager.h"
#include "CCommandResultTable.h"
#include "LCommandCenter.h"
#include "CSMsgCommandHelper.h"
#include "SCmdRouter_Login.h"
#include "LDBManager.h"


LDBTaskCharInsert::LDBTaskCharInsert() 
: LDBAsyncTask(SDBT_DBTYPE_GAMEDB, SDBTID_CHARINSERT, 2)
{
	m_Data.bIsCharNameDuplicated = false;
}


LDBTaskCharInsert::~LDBTaskCharInsert()
{

}


void LDBTaskCharInsert::Input(const MUID& uidPlayer, const int64 nAID)
{
	m_Data.uidPlayer	= uidPlayer;
	m_Data.nAID			= nAID;
}


void LDBTaskCharInsert::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(INSERT_CHAR)))
		return;

	if (0 == rs.GetFetchedCount())
		return;

	if (rs.FieldW(L"CHAR_ID").IsNull())
		return;

	int64 nNewCID = rs.FieldW(L"CHAR_ID").AsInt64();
	if (0 > nNewCID)
	{
		m_Data.bIsCharNameDuplicated = true;
		return;
	}
}


mdb::MDB_THRTASK_RESULT LDBTaskCharInsert::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

LDBTaskCharInsert::Completer::Completer( LDBTaskCharInsert::_DATA& Data ) : m_Data(Data)
{

}

void LDBTaskCharInsert::Completer::Do()
{
	LPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_Data.uidPlayer);
	if (NULL == pPlayerObject)
		return;

	if (PreProcessDuplicatedName(pPlayerObject))
		return;

	RefreshCharacter(pPlayerObject);

	//
	if (pPlayerObject->GetAccountInfo().bNewAcc)
	{
		gsys.pDBManager->UpdaateCreateDt(pPlayerObject->GetAccountInfo().nAID);	
		pPlayerObject->GetAccountInfo().bNewAcc = false;
	}
}

bool LDBTaskCharInsert::Completer::PreProcessDuplicatedName( LPlayerObject* pPlayerObject )
{
	if (!m_Data.bIsCharNameDuplicated)
		return false;
	
	RouteResult(CR_FAIL_CHAR_EXIST_CHAR_NAME);

	return true;
}

void LDBTaskCharInsert::Completer::RefreshCharacter( LPlayerObject* pPlayerObject )
{
	// 결과 통보
	RouteResult(CR_SUCCESS);

	// 캐릭터 목록 갱신
	gsys.pDBManager->GetAccountCharList(m_Data.uidPlayer, (int)m_Data.nAID);
}

void LDBTaskCharInsert::Completer::RouteResult(CCommandResultTable nResult)
{
	if (gsys.pCommandCenter == NULL) return;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponseInsertChar(m_Data.uidPlayer, nResult);
}
