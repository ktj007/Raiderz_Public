#include "stdafx.h"
#include "GDBTaskMailWrite.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GMailSystem.h"
#include "CTransData.h"
#include "SProxyPlayerManager.h"
#include "GMasterServerFacade.h"
#include "SProxyPlayer.h"
#include "GCharacterDBCache.h"
#include "GDBCacheSystem.h"
#include "GDBManager.h"
#include "GItemDBCache.h"
#include "GMoneySystem.h"
#include "GItemSystem.h"
#include "GItemRemover.h"
#include "GMsgCommandFacade.h"
#include "GEntityPlayer.h"
#include "GItemHolder.h"

GDBTaskMailWrite::GDBTaskMailWrite(const MUID& uidReqPlayer)
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_MAIL_SEND)
{
	m_Data.m_uidPlayer = uidReqPlayer;
}

GDBTaskMailWrite::~GDBTaskMailWrite()
{
}

bool GDBTaskMailWrite::Input(const GDBT_MAIL_WRITE& data)
{
	// 입력값 저장
	m_Data.m_mailWrite = data;

	// DB 캐싱
	if (!BeginCacheSync(data))
	{
		return false;
	}

	return true;
}

mdb::MDB_THRTASK_RESULT GDBTaskMailWrite::RouteResult(CCommandResultTable nResult)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);

	if (pPlayer != NULL)
	{
		gsys.pMailSystem->RouteMailPostRes(pPlayer, nResult);
	}

	return mdb::MDBTR_SUCESS;
}

void GDBTaskMailWrite::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(SQL_MAIL_WRITE)))
		return;
	
	
	// 공통 결과
	m_Result.m_mailResult.m_nAID = rs.Field("RCVR_ACCN_ID").AsInt64();
	m_Result.m_mailResult.m_nCID = rs.Field("RCVR_CHAR_ID").AsInt64();
	m_Result.m_mailResult.m_nMailUID = rs.Field("MAIL_ID").AsInt64();


	// 첨부 아이템 각각의 결과
	int nCount = 0;
	while(!rs.IsEOF() && nCount < MAX_MAIL_APPENDED_ITEM_COUNT)
	{
		m_Result.m_mailResult.m_nItemUID[nCount] = rs.Field("IUID").AsInt64();	
		m_Result.m_mailResult.m_nNewItemUID[nCount] = rs.Field("NEW_IUID").AsInt64();			

		nCount++;
		rs.MoveNext();
	}

	m_Result.m_mailResult.m_nResultItemCount = nCount;
}

void GDBTaskMailWrite::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	EndSync(IsSuccess());
}

mdb::MDB_THRTASK_RESULT GDBTaskMailWrite::_OnCompleted()
{
	// 받을 사람이 없거나, 받을 수 있는 상태가 아니다.
	if (m_Result.m_mailResult.m_nCID == -1)
	{		
		return RouteResult(CR_FAIL_MAIL_POST_DB_FAIL);		
	}

	gsys.pDBManager->MailWriteLog(m_Data.m_mailWrite, m_Result.m_mailResult);

	// 첨부 아이템, 게임머니 정리
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_uidPlayer);
	
	if (pPlayer != NULL)
	{
		// 게임머니 감소
		int nPostage = gsys.pMailSystem->CalcPostage(m_Data.m_mailWrite.m_nAppendedItemCount);
		int nDecreaseMoney = m_Data.m_mailWrite.m_nDeltaMoney + nPostage;

		gsys.pMoneySystem->DecreaseForDBTask(pPlayer, nDecreaseMoney);

		// 아이템 제거
		vector<REMOVE_ITEM> vecRemoveItem;
		for(int i = 0; i < m_Data.m_mailWrite.m_nAppendedItemCount && i < MAX_MAIL_APPENDED_ITEM_COUNT; i++)
		{
			IUID nItemUID = m_Data.m_mailWrite.m_appendedItemSlot[i].m_nItemUID;
			int nStackAmount = m_Data.m_mailWrite.m_appendedItemSlot[i].m_nStackAmount;
			int nAppendedAmount = m_Data.m_mailWrite.m_appendedItemSlot[i].m_nDeltaStackAmount;

			GItemHolder* pItemHolder = pPlayer->GetItemHolder();
			GItem* pItem = pItemHolder->GetItem(SLOTTYPE_INVENTORY, nItemUID);

			if (pItem == NULL)
			{
				continue;
			}		

			vecRemoveItem.push_back(REMOVE_ITEM(SLOTTYPE_INVENTORY, pItem->m_nSlotID, nStackAmount, nAppendedAmount));
		}		
		gsys.pItemSystem->GetRemover().RemoveForDBTask(pPlayer->GetUID(), vecRemoveItem);
	}


	// 편지쓰기 완료 통보
	RouteResult(CR_SUCCESS);


	// AppServer 전달할 정보 설정
	CID nCID = (CID)m_Result.m_mailResult.m_nCID;

	TD_MAIL_MAILBOX_MAIL_INFO info;
	info.uidMail = m_Result.m_mailResult.m_nMailUID;
	info.nType = m_Data.m_mailWrite.m_nMailType;
	wcsncpy_s(info.strSenderName, m_Data.m_mailWrite.m_strSenderName.c_str(), _TRUNCATE);
	wcsncpy_s(info.strMailTitle, m_Data.m_mailWrite.m_strTitle.c_str(), _TRUNCATE);
	info.nAppendedItemID = m_Data.m_mailWrite.m_nDefaultItemID;
	info.bIsRead = false;
	info.nExpiringTimeSec = m_Data.m_mailWrite.m_nRemainDeleteSeconds;
	info.bHasText = m_Data.m_mailWrite.m_hasText;	
	info.nAppendedMoney = m_Data.m_mailWrite.m_nDeltaMoney;


	// AppServer에 전달
	gsys.pMailSystem->RouteNewMailToReceiver(nCID, &info);

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT	GDBTaskMailWrite::_OnFailed()
{
	// 클라이언트에 실패 통보
	return RouteResult(CR_FAIL_MAIL_POST_DB_FAIL);
}

bool GDBTaskMailWrite::BeginCacheSync(const GDBT_MAIL_WRITE& data)
{
	if (!SyncCharacter() ||
		!SyncItem(data))
	{
		EndSync(false);
		return false;
	}

	return true;
}

bool GDBTaskMailWrite::SyncCharacter(void)
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(m_Data.m_uidPlayer);
	if (NULL == cc)
		return true;

	if (!cc->BeginCacheSync())
		return false;

	m_SyncingList.SetPlayer(m_Data.m_uidPlayer);

	if (NULL == m_SyncingList.PushChar())
	{
		cc->EndCacheSync(false);
		return false;
	}

	return true;
}

bool GDBTaskMailWrite::SyncItem(const GDBT_MAIL_WRITE& data)
{
	GItemDBCache* pi= NULL;

	for(int i = 0; i < data.m_nAppendedItemCount && i < MAX_MAIL_APPENDED_ITEM_COUNT; i++)
	{
		IUID nItemUID = data.m_appendedItemSlot[i].m_nItemUID;
		int nSlotID = data.m_appendedItemSlot[i].m_nSlotID;
		int nSlotType = SLOTTYPE_INVENTORY;


		if (nItemUID <= 0)
			continue;

		pi = gsys.pDBCacheSystem->FindItemCache(m_Data.m_uidPlayer, nSlotType, nSlotID, nItemUID);
		if (NULL == pi)
			return false;

		if (!pi->BeginCacheSync())
			return false;

		if (NULL == m_SyncingList.PushItem(nItemUID))
		{
			pi->EndCacheSync(false);
			return false;
		}
	}

	return true;
}

void GDBTaskMailWrite::EndSync( bool bZero)
{
	m_SyncingList.EndCacheSync(bZero);
}
