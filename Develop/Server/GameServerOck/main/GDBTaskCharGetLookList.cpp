#include "stdafx.h"
#include "GDBTaskCharGetLookList.h"
#include "GDBTaskGlobal.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GFieldMgr.h"
#include "SCmdRouter_Login.h"
#include "GCommandCenter.h"


GDBTaskCharGetLookList::GDBTaskCharGetLookList(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_CHARGETLOOKLIST, 2)
{
	m_commonBody.SetPlayerUID(uidReqPlayer);
	m_commonBody.SetCharCount(0);
}


GDBTaskCharGetLookList::~GDBTaskCharGetLookList()
{

}


void GDBTaskCharGetLookList::OnExecute(mdb::MDatabase& rfDB)
{
	if (!GetSimpleInfo(rfDB))
		return;

	if (!GetEquipInfo(rfDB))
		return;
}

bool GDBTaskCharGetLookList::GetSimpleInfo( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(CHAR_SIMPLE_INFO)))
		return false;

	m_commonBody.GetCharSimpleInfo(rs);
	
	return true;
}

bool GDBTaskCharGetLookList::GetEquipInfo( mdb::MDatabase& rfDB )
{
	mdb::MDBRecordSet rs(&rfDB);

	if (!ExecuteW(rs, GetSQLW(CHAR_EQUIPMENT_INFO)))
		return false;

	return m_commonBody.GetCharEquipmentInfo(rs);
}

mdb::MDB_THRTASK_RESULT GDBTaskCharGetLookList::_OnCompleted()
{
	Completer completer(m_commonBody);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

SCommonBodyCharGetLookList& GDBTaskCharGetLookList::GetData()
{
	return m_commonBody;
}

GDBTaskCharGetLookList::Completer::Completer(SCommonBodyCharGetLookList& commonBody)
: m_commonBody(commonBody)
{

}

void GDBTaskCharGetLookList::Completer::Do()
{
	MUID uidPlayer = m_commonBody.GetPlayerUID();
	GPlayerObject* pPlayerObject = gmgr.pPlayerObjectManager->GetPlayer(uidPlayer);
	if (NULL == pPlayerObject)
	{
		return;
	}

	
	// 캐릭터 목록 정리
	vector<TD_AccountCharInfo> vecAccountCharInfo;	
	pPlayerObject->ClearCharFieldInfo();	

	for (int i = 0; i < m_commonBody.GetCharCount(); ++i)
	{
		// 룩 정보
		TD_AccountCharInfo tdAccountCharInfo;

		m_commonBody.SetAccountCharInfo(i, &tdAccountCharInfo);
		vecAccountCharInfo.push_back(tdAccountCharInfo);


		// 필드 정보
		SAccountCharFieldInfo accountCharFieldInfo;

		m_commonBody.SetAccountCharFieldInfo(i, &accountCharFieldInfo);
		pPlayerObject->AddCharFieldInfo(accountCharFieldInfo);
	}


	// 캐릭터 목록 응답
	SCmdRouter_Login cmdRouter(gsys.pCommandCenter);
	cmdRouter.ResponseAccountCharList(uidPlayer, vecAccountCharInfo);
}
