#include "stdafx.h"
#include "GDBTaskCharInsert.h"
#include "GDBTaskGlobal.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "CCommandResultTable.h"
#include "GCommand.h"
#include "GCommandCenter.h"
#include "SCmdRouter_Login.h"
#include "GDBManager.h"


GDBTaskCharInsert::GDBTaskCharInsert(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_CHARINSERT)
{
	m_Data.uidPlayer				= uidReqPlayer;
	m_Data.bIsCharNameDuplicated	= false;
}


GDBTaskCharInsert::~GDBTaskCharInsert()
{

}


void GDBTaskCharInsert::Input(const int64 nAID)
{
	m_Data.nAID = nAID;
}


void GDBTaskCharInsert::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(INSERT_CHAR)))
		return;

	if (0 == rs.GetFetchedCount())
		return;

	if (rs.FieldW(L"CHAR_ID").IsNull())
		return;

	int64 nNewCID = rs.FieldW(L"CHAR_ID").AsInt64();
	if (-1 == nNewCID)
	{
		m_Data.bIsCharNameDuplicated = true;
		return;
	}
}

mdb::MDB_THRTASK_RESULT GDBTaskCharInsert::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

GDBTaskCharInsert::Completer::Completer( GDBTaskCharInsert::_DATA& Data ) : m_Data(Data)
{

}

void GDBTaskCharInsert::Completer::Do()
{
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(m_Data.uidPlayer);
	if (NULL == pPlayerObject)
		return;

	if (PreProcessDuplicatedName(pPlayerObject))
		return;

	RefreshCharacter(pPlayerObject);
}

bool GDBTaskCharInsert::Completer::PreProcessDuplicatedName( GPlayerObject* pPlayerObject )
{
	if (!m_Data.bIsCharNameDuplicated)
		return false;

	GEntityPlayer* pEntityPlayer = pPlayerObject->GetEntity();
	_ASSERT(NULL != pEntityPlayer);
	if (NULL == pEntityPlayer)
		return true;

	RouteResult(CR_FAIL_CHAR_EXIST_CHAR_NAME);

	return true;
}

void GDBTaskCharInsert::Completer::RefreshCharacter( GPlayerObject* pPlayerObject )
{
	// 결과 통보
	RouteResult(CR_SUCCESS);

	// 캐릭터 목록 갱신
	gsys.pDBManager->CharGetLookList(m_Data.uidPlayer, m_Data.nAID);
}

void GDBTaskCharInsert::Completer::RouteResult(CCommandResultTable nResult)
{
	if (gsys.pCommandCenter == NULL) return;

	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponseInsertChar(m_Data.uidPlayer, nResult);
}
