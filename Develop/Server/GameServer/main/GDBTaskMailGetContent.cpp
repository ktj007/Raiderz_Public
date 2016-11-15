#include "stdafx.h"
#include "GDBTaskMailGetContent.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GDBManager.h"
#include "GEntityPlayer.h"
#include "GPlayerMailBox.h"
#include "GItemManager.h"
#include "GMail.h"
#include "GClientMailRouter.h"

GDBTaskMailGetContent::GDBTaskMailGetContent(const MUID& uidReqPlayer)
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_MAIL_GET_CONTENT)
{
	m_Data.m_uidPlayer	= uidReqPlayer;
}

GDBTaskMailGetContent::~GDBTaskMailGetContent()
{
}

void GDBTaskMailGetContent::Input(MUID nMUID, bool bHasText)
{
	m_Data.m_uidMail = nMUID;
	m_Data.m_bHasText = bHasText;
}

void GDBTaskMailGetContent::OnExecute(mdb::MDatabase& rfDB)
{
	_Execute_GetText(rfDB);
	_Execute_GetAppendedItem(rfDB);
}

void GDBTaskMailGetContent::_Execute_GetText(mdb::MDatabase& rfDB)
{
	if (!m_Data.m_bHasText)
		return;

	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(SQL_MAIL_GET_TEXT)))
		return;

	m_Result.m_strText = rs.Field("TEXT").AsWString();
}

void GDBTaskMailGetContent::_Execute_GetAppendedItem(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(SQL_MAIL_GET_APPENDED_ITEM_LIST)))
		return;

	if (0 == rs.GetFetchedCount())
		return;

	static const size_t nHash_IUID			= rs.MakeHashValueW(L"IUID");
	static const size_t nHash_SlotID		= rs.MakeHashValueW(L"SLOT_ID");
	static const size_t nHash_ItemID		= rs.MakeHashValueW(L"ITEM_ID");
	static const size_t nHash_StackAmt		= rs.MakeHashValueW(L"STACK_AMT");
	static const size_t nHash_Dura			= rs.MakeHashValueW(L"DURA");
	static const size_t nHash_Color			= rs.MakeHashValueW(L"COLOR");
	static const size_t nHash_EnchCount		= 0;
	static const size_t nHash_EnchItemID1	= rs.MakeHashValueW(L"ENCH_ITEM_ID_1");
	static const size_t nHash_EnchItemID2	= rs.MakeHashValueW(L"ENCH_ITEM_ID_2");
	static const size_t nHash_EnchItemID3	= rs.MakeHashValueW(L"ENCH_ITEM_ID_3");
	static const size_t nHash_EnchItemID4	= rs.MakeHashValueW(L"ENCH_ITEM_ID_4");
	static const size_t nHash_EnchItemID5	= rs.MakeHashValueW(L"ENCH_ITEM_ID_5");
	static const size_t nHash_EnchItemID6	= rs.MakeHashValueW(L"ENCH_ITEM_ID_6");

	for (; !rs.IsEOF(); rs.MoveNext())
	{
		_RESULT_MAIL_ITEM item;

		item.m_nIUID		= rs.FieldHash(nHash_IUID).AsInt64();
		item.m_nSlotIndex	= rs.FieldHash(nHash_SlotID).AsInt();
		item.m_nItemID		= rs.FieldHash(nHash_ItemID).AsInt();
		item.m_nStackAmt	= rs.FieldHash(nHash_StackAmt).AsShort();
		item.m_nDura		= rs.FieldHash(nHash_Dura).AsByte();
		item.m_nColor		= rs.FieldHash(nHash_Color).AsInt();
		item.m_nEnchCount	= 0;
		item.m_nEnchBufID1	= rs.FieldHash(nHash_EnchItemID1).AsInt();
		item.m_nEnchBufID2	= rs.FieldHash(nHash_EnchItemID2).AsInt();
		item.m_nEnchBufID3	= rs.FieldHash(nHash_EnchItemID3).AsInt();
		item.m_nEnchBufID4	= rs.FieldHash(nHash_EnchItemID4).AsInt();
		item.m_nEnchBufID5	= rs.FieldHash(nHash_EnchItemID5).AsInt();
		item.m_nEnchBufID6	= rs.FieldHash(nHash_EnchItemID6).AsInt();

		m_Result.m_vecAppendedItem.push_back(item);
	}

	rs.Close();
}

mdb::MDB_THRTASK_RESULT GDBTaskMailGetContent::_OnCompleted()
{
	Completer completer(m_Data, m_Result);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT	GDBTaskMailGetContent::_OnFailed()
{
	// do nothing
	return mdb::MDBTR_SUCESS;
}

GDBTaskMailGetContent::Completer::Completer(_DATA& Data, _RESULT& Result) : m_Data(Data), m_Result(Result)
{
}

void GDBTaskMailGetContent::Completer::Do()
{
	// 플레이어 찾기
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	if (pPlayer == NULL) return;

	// 해당 편지 확인
	GPlayerMailBox& mailBox = pPlayer->GetMailbox();
	if (!mailBox.IsExist(m_Data.m_uidMail.Value))
		return;

	// 편지 읽기 표시
	if (!SetMailRead(pPlayer))
		return;
	
	// 첨부 아이템 저장
	AssignMailAppendedItems(pPlayer);

	// 편지 컨텐츠 채워짐 플레그 설정.
	GMail* pMail = mailBox.GetMail(m_Data.m_uidMail.Value);
	VALID(pMail != NULL);
	pMail->SetFilledContentForDBTask();

	// 편지 읽기 응답
	RouteMailReadRes(pPlayer);
}

bool GDBTaskMailGetContent::Completer::SetMailRead(GEntityPlayer* pPlayer)
{
	VALID_RET(pPlayer != NULL, false);

	GPlayerMailBox& mailBox = pPlayer->GetMailbox();

	// DB 요청을 못하는 상황이면, 편지 내용 읽기 실패처리.
	if (!mailBox.IsReaded(m_Data.m_uidMail.Value))
	{
		int64 nAID = pPlayer->GetAID();
		CID nCID = pPlayer->GetCID();

		if (!gsys.pDBManager->MailSetRead(m_Data.m_uidPlayer, nAID, nCID, m_Data.m_uidMail.Value))
		{
			return false;
		}

		// 읽은 상태 표시 - 재요청을 막기 위해서 미리 체크해놓는다.
		mailBox.SetRead(m_Data.m_uidMail.Value);
	}
	return true;
}

void GDBTaskMailGetContent::Completer::AssignMailAppendedItems(GEntityPlayer* pPlayer)
{
	VALID(pPlayer != NULL);

	GPlayerMailBox& mailBox = pPlayer->GetMailbox();

	GMail* pMail = mailBox.GetMail(m_Data.m_uidMail.Value);
	VALID(pMail != NULL);

	pMail->ClearAppendedItems();

	for (size_t i = 0; i < m_Result.m_vecAppendedItem.size(); ++i)
	{
		_RESULT_MAIL_ITEM& refData = m_Result.m_vecAppendedItem[i];

		GItem* pItem = gmgr.pItemManager->MakeNewItem(refData.m_nItemID, refData.m_nStackAmt);
		if (pItem == NULL)
			continue;

		pItem->m_nIUID = refData.m_nIUID;
		pItem->m_nSlotID = refData.m_nSlotIndex;
		pItem->m_nSlotType = SLOTTYPE_MAIL;
		pItem->m_nDurability = refData.m_nDura;
		pItem->m_nDyedColor = refData.m_nColor;
		
		pItem->m_nEnchants[0] = refData.m_nEnchBufID1;
		pItem->m_nEnchants[1] = refData.m_nEnchBufID2;
		pItem->m_nEnchants[2] = refData.m_nEnchBufID3;
		pItem->m_nEnchants[3] = refData.m_nEnchBufID4;
		pItem->m_nEnchants[4] = refData.m_nEnchBufID5;
		pItem->m_nEnchants[5] = refData.m_nEnchBufID6;

		if (!pMail->AddAppendedItemForDBTask(pItem))
		{
			SAFE_DELETE(pItem);
			continue;
		}
	}
}

void GDBTaskMailGetContent::Completer::RouteMailReadRes(GEntityPlayer* pPlayer)
{
	VALID(pPlayer != NULL);

	GPlayerMailBox& mailBox = pPlayer->GetMailbox();

	GMail* pMail = mailBox.GetMail(m_Data.m_uidMail.Value);
	VALID(pMail != NULL);

	// Param0 : 편지 읽기 정보
	TD_MAIL_CONTENT_INFO info;
	info.uidMail = m_Data.m_uidMail;
	info.nAppendedMoney = pMail->GetAppendedMoney();
 
	// Param1 : 편지 Text == m_Result.m_strText

	// Param2 : 편지 첨부 아이템 정보
	vector<TD_ITEM> vecAppendedItem;
	for (size_t i = 0; i < m_Result.m_vecAppendedItem.size(); ++i)
	{
		_RESULT_MAIL_ITEM& refData = m_Result.m_vecAppendedItem[i];
		GItem* pItem = pMail->GetAppendedItem(refData.m_nSlotIndex);
		if (pItem == NULL)
			continue;

		TD_ITEM data = pItem->ToTD_ITEM();
		vecAppendedItem.push_back(data);
	}

	// 전송
	GClientMailRouter router;
	router.MailReadRes(pPlayer, &info, m_Result.m_strText, vecAppendedItem);
}
