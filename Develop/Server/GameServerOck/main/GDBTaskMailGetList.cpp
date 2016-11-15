#include "stdafx.h"
#include "GDBTaskMailGetList.h"
#include "GGlobal.h"
#include "GPlayerObject.h"
#include "GPlayerObjectManager.h"
#include "GEntityPlayer.h"
#include "GPlayerMailBox.h"
#include "GMail.h"
#include "GMailSystem.h"
#include "GPlayerDoing.h"
#include "GSystem.h"


GDBTaskMailGetList::GDBTaskMailGetList(const MUID& uidReqPlayer) 
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_MAIL_GET_LIST)
{
	m_Data.uidPlayer	= uidReqPlayer;
}

GDBTaskMailGetList::~GDBTaskMailGetList()
{

}

bool GDBTaskMailGetList::Input(int64 nReqMailUIDTop, int64 nReqMailUIDBottom, int nReqMailCount, bool bOpenMaibox/*=true*/)
{
	if (nReqMailUIDTop < nReqMailUIDBottom)
		return false;
	if (nReqMailCount == 0)
		return false;

	m_Data.nReqMailUIDTop = nReqMailUIDTop;
	m_Data.nReqMailUIDBottom = nReqMailUIDBottom;
	m_Data.nReqMailCount = nReqMailCount;
	m_Data.bOpenMaibox = bOpenMaibox;

	return true;
}

void GDBTaskMailGetList::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(SQL_MAIL_GET_LIST)))
	{
		return;
	}

	if (0 == rs.GetFetchedCount())
	{
		return;
	}

	static const size_t nHashMailUID		= rs.MakeHashValueW(L"MAIL_ID");
	static const size_t nHashMailType		= rs.MakeHashValueW(L"MAIL_TYPE");
	static const size_t nHashSenderName		= rs.MakeHashValueW(L"SNDR_NAME");
	static const size_t nHashTitle			= rs.MakeHashValueW(L"TITLE");
	static const size_t nHashHasText		= rs.MakeHashValueW(L"HAVE_TEXT");
	static const size_t nHashMoney			= rs.MakeHashValueW(L"MONEY");
	static const size_t nHashReadFlag		= rs.MakeHashValueW(L"READ_FLAG");
	static const size_t nHashRemainDelTime	= rs.MakeHashValueW(L"REMAIN_DEL_TIME");
	static const size_t nHashDefaultItemID	= rs.MakeHashValueW(L"DEF_ITEM_ID");

	for (; !rs.IsEOF(); rs.MoveNext())
	{
		_DATA_MAIL tdMail;
		tdMail.nMailUID			= rs.FieldHash(nHashMailUID).AsInt64();
		tdMail.nType			= rs.FieldHash(nHashMailType).AsByte();
		tdMail.strSender		= rs.FieldHash(nHashSenderName).AsWString();
		tdMail.strTitle			= rs.FieldHash(nHashTitle).AsWString();
		tdMail.bHasText			= rs.FieldHash(nHashHasText).AsBool();
		tdMail.nAppendedMoney	= rs.FieldHash(nHashMoney).AsInt();
		tdMail.bReadFlag		= rs.FieldHash(nHashReadFlag).AsBool();
		tdMail.nDefaultItemID	= rs.FieldHash(nHashDefaultItemID).AsInt();
		tdMail.nRemainDelTime	= rs.FieldHash(nHashRemainDelTime).AsInt();

		m_Data.vecDBMailList.push_back(tdMail);
	}

	rs.Close();

	return;
}

mdb::MDB_THRTASK_RESULT GDBTaskMailGetList::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskMailGetList::_OnFailed()
{
	GPlayerObject* pPlayer = gmgr.pPlayerObjectManager->GetPlayer(m_Data.uidPlayer);
	if (NULL == pPlayer)
		return mdb::MDBTR_SUCESS;

	GEntityPlayer* pEntityPlayer = pPlayer->GetEntity();
	if (NULL == pEntityPlayer)
		return mdb::MDBTR_SUCESS;

	pEntityPlayer->GetDoing().EndDoing();

	return mdb::MDBTR_SUCESS;
}

GDBTaskMailGetList::Completer::Completer( _DATA& Data ) : m_Data(Data)
{
}

void GDBTaskMailGetList::Completer::Do()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.uidPlayer);
	if (NULL == pPlayer)
		return;

	// 읽어온 편지 목록 적용.
	AssignMailList(pPlayer);

	/// 편지 목록 전송
	RouteMailList(pPlayer);

	/// 편지함 커서 갱신
	CheckRemainMailAtDB(pPlayer);
}

void GDBTaskMailGetList::Completer::AssignMailList(GEntityPlayer* pPlayer)
{
	VALID(pPlayer != NULL);

	GPlayerMailBox& refMailbox = pPlayer->GetMailbox();

	for (size_t i = 0; i < m_Data.vecDBMailList.size(); ++i)
	{
		_DATA_MAIL& refDBMail = m_Data.vecDBMailList[i];

		if (refMailbox.IsExist(refDBMail.nMailUID))
		{
			dlog("GDBTaskMailGetList::Completer::Do(), Already Exist MailUID(%I64d)\n", refDBMail.nMailUID);
			continue;
		}

		GMail* pNewMail = refMailbox.NewMail(refDBMail.nMailUID);
		if (pNewMail == NULL) continue;

		bool bRet = pNewMail->Init(pPlayer->GetCID()
			, (MAIL_TYPE)refDBMail.nType
			, refDBMail.bReadFlag
			, refDBMail.strSender
			, refDBMail.strTitle
			, refDBMail.bHasText
			, refDBMail.nAppendedMoney
			, refDBMail.nDefaultItemID
			, refDBMail.nRemainDelTime);
		if (bRet == false)
		{
			mlog("Error! GDBTaskMailGetList, GMail::Init() - CID(%I64d), AppendedMoney(%d), RemainDelTime(%d)\n"
				, pPlayer->GetCID(), refDBMail.nAppendedMoney, refDBMail.nRemainDelTime);
			SAFE_DELETE(pNewMail);
			continue;
		}

		pNewMail->InitSystemTimeMSForExpiCheck(gsys.pSystem->GetNowTime());

		if (!refMailbox.AddMail(pNewMail))
		{
			SAFE_DELETE(pNewMail);
			continue;
		}
	}
}

void GDBTaskMailGetList::Completer::RouteMailList(GEntityPlayer* pPlayer)
{
	VALID(pPlayer != NULL);

	if (m_Data.bOpenMaibox)
	{
		gsys.pMailSystem->RouteOpenMailbox(pPlayer, m_Data.nReqMailUIDTop, m_Data.nReqMailUIDBottom);
	}
	else
	{
		bool bIsNew = false;
		vector<int64> vecMailUID;
		for (size_t i = 0; i < m_Data.vecDBMailList.size(); ++i)
		{
			vecMailUID.push_back(m_Data.vecDBMailList[i].nMailUID);
		}
		gsys.pMailSystem->RouteNotifyMail(pPlayer, bIsNew, vecMailUID);
	}
}

void GDBTaskMailGetList::Completer::CheckRemainMailAtDB(GEntityPlayer* pPlayer)
{
	VALID(pPlayer != NULL);

	GPlayerMailBox& refMailbox = pPlayer->GetMailbox();
	if (false == refMailbox.IsRemainMailAtDB()) ///< DB 에 남은 편지가 없음.
		return;	///< 갱신할 필요 없음.

	if (m_Data.nReqMailCount != (int)m_Data.vecDBMailList.size()) ///< DB에서 읽어온 개수가 요청한 개수보다 적다.
	{
		/// DB에 더이상 읽어올 편지가 없다.
		refMailbox.SetNoRemainMailAtDB();
	}
}