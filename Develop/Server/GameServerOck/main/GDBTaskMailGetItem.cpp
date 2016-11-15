#include "StdAfx.h"
#include "GDBTaskMailGetItem.h"
#include "GEntityPlayer.h"
#include "GGlobal.h"
#include "GPlayerObjectManager.h"
#include "GCharacterDBCache.h"
#include "GDBCacheSystem.h"
#include "GMailSystem.h"
#include "GMailItemTaker.h"
#include "GItemHolder.h"
#include "GPlayerMailBox.h"
#include "GMail.h"
#include "GItemAdder.h"
#include "GDBManager.h"


GDBTaskMailGetItem::GDBTaskMailGetItem(const MUID& uidReqPlayer)
: GDBAsyncTask(uidReqPlayer, SDBT_DBTYPE_GAMEDB, SDBTID_MAIL_GET_ITEM)
, m_SyncingList(uidReqPlayer)
{
	
}

GDBTaskMailGetItem::~GDBTaskMailGetItem(void)
{
}

bool GDBTaskMailGetItem::Input(const GDBT_MAIL_GET_ITEM& data)
{
	m_Data.m_MailItem = data;

	// DB Ä³½Ì
	if (!BeginCacheSync())
	{
		return false;
	}

	return true;
}

bool GDBTaskMailGetItem::BeginCacheSync()
{
	if (!CacheSyncCharacter())
	{
		EndCacheSync(false);
		return false;
	}
	return true;
}
bool GDBTaskMailGetItem::CacheSyncCharacter()
{
	GCharacterDBCache* cc = gsys.pDBCacheSystem->FindCharCache(m_Data.m_MailItem.m_uidPlayer);
	if (NULL == cc)
		return true;

	if (!cc->BeginCacheSync())
		return false;

	m_SyncingList.SetPlayer(m_Data.m_MailItem.m_uidPlayer);

	if (NULL == m_SyncingList.PushChar())
	{
		cc->EndCacheSync(false);
		return false;
	}

	return true;
}
void GDBTaskMailGetItem::EndCacheSync(bool bSuccess)
{
	m_SyncingList.EndCacheSync(bSuccess);
}

void GDBTaskMailGetItem::OnExecute(mdb::MDatabase& rfDB)
{
	mdb::MDBRecordSet rs(&rfDB);
	if (!ExecuteW(rs, GetSQLW(SQL_MAIL_GET_ITEM)))
	{
		return;
	}

	rs.Close();

	return;
}

void GDBTaskMailGetItem::_OnPreCompleted()
{
	__super::_OnPreCompleted();

	EndCacheSync(IsSuccess());
}

mdb::MDB_THRTASK_RESULT GDBTaskMailGetItem::_OnCompleted()
{
	Completer completer(m_Data);
	completer.Do();

	return mdb::MDBTR_SUCESS;
}

mdb::MDB_THRTASK_RESULT GDBTaskMailGetItem::_OnFailed()
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_MailItem.m_uidPlayer);
	if (pPlayer == NULL)
		return mdb::MDBTR_SUCESS;

	// Invectory ½½·Ô ¿¹¾à ÇØÁ¦.
	GItemHolder* pItemHolder = pPlayer->GetItemHolder();
	pItemHolder->CancelReserveSlot(SLOTTYPE_INVENTORY, m_Data.m_MailItem.m_InvenSlotID);

	return mdb::MDBTR_SUCESS;
}

GDBTaskMailGetItem::Completer::Completer( _DATA& Data ) : m_Data(Data)
{
}

void GDBTaskMailGetItem::Completer::Do()
{
	gsys.pDBManager->MailGetAppendedItemLog(m_Data.m_MailItem);

	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntity(m_Data.m_MailItem.m_uidPlayer);
	if (pPlayer == NULL) return;

	AddItemToPlayer(pPlayer);

	DelItemFromMail(pPlayer);
}

void GDBTaskMailGetItem::Completer::AddItemToPlayer(GEntityPlayer* pPlayer)
{
	VALID(pPlayer != NULL);

	GPlayerMailBox& refMailbox = pPlayer->GetMailbox();
	GMail* pMail = refMailbox.GetMail(m_Data.m_MailItem.m_nMailUID);
	if (pMail == NULL)
		return;

	GItem* pMailItem = pMail->GetAppendedItem(m_Data.m_MailItem.m_nMailSlotID);
	if (pMailItem == NULL)
		return;

	if (0 >= pMailItem->GetAmount() || 0 == pMailItem->m_nIUID)
		return;

	ADD_ITEM addItem = ADD_ITEM(SLOTTYPE_INVENTORY, m_Data.m_MailItem.m_InvenSlotID, pMailItem->GetAmount(), pMailItem->GetAmount(),
		pMailItem->m_nIUID, pMailItem->m_pItemData->m_nID, -1, 0, pMailItem->m_nDurability, pMailItem->m_nDyedColor);
	gsys.pItemSystem->GetAdder().AddForDBTask(pPlayer->GetUID(), addItem);
}

void GDBTaskMailGetItem::Completer::DelItemFromMail(GEntityPlayer* pPlayer)
{
	VALID(pPlayer != NULL);

	gsys.pMailSystem->GetItemTaker()->DelTakenItemForDBTask(
		pPlayer
		, m_Data.m_MailItem.m_nMailUID
		, m_Data.m_MailItem.m_nMailSlotID
		, m_Data.m_MailItem.m_nNewMailDefaultItemID);
}
