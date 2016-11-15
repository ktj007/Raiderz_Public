#include "stdafx.h"
#include "XCmdHandler_Soul.h"
#include "XStrings.h"
#include "XGame.h"

XCmdHandler_Soul::XCmdHandler_Soul(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_SOUL_DRAIN_ME,			OnDrainMe);
	SetCmdHandler(MC_SOUL_DRAIN_OTHER,		OnDrainOther);
}

MCommandResult XCmdHandler_Soul::OnDrainMe(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidNPC;
	int nCharmItemEquipSlotID;
	uint8 nSoulQuantity;

	if (pCommand->GetParameter(&uidNPC,			0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nCharmItemEquipSlotID,	1, MPT_CHAR)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSoulQuantity,	2, MPT_UCHAR)==false) return CR_ERROR;	

	XItem* pItem = gvar.MyInfo.EquipmentSlot.GetItem(ITEMSLOT_CHARM);
	if (NULL == pItem) return CR_ERROR;
	pItem->m_nSoulCount += nSoulQuantity;

	const wchar_t* szSoulType = XStrings::SoulTypeToChatString(pItem->m_pItemData->m_nSoulType);	

	global.ui->OnSystemMsg(L"{0} 타입 영혼 {1}개를 흡수하였습니다.", FSParam(szSoulType, nSoulQuantity));

	// 소울 생성
	gg.currentgamestate->CreateSoulObject( uidNPC, XGetMyUID(), pItem->m_pItemData->m_nSoulType, 2000);

	return CR_TRUE;
}

MCommandResult XCmdHandler_Soul::OnDrainOther(MCommand* pCommand, MCommandHandler* pHandler)
{
	MUID uidNPC;
	MUID uidAbsorber;
	SOUL_TYPE nSoulType;
	uint8 nSoulQuantity;

	if (pCommand->GetParameter(&uidNPC,			0, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&uidAbsorber,	1, MPT_UID)==false) return CR_ERROR;
	if (pCommand->GetParameter(&nSoulType,		2, MPT_UCHAR)==false) return CR_ERROR;	
	if (pCommand->GetParameter(&nSoulQuantity,	3, MPT_UCHAR)==false) return CR_ERROR;	

	// 소울 생성
	gg.currentgamestate->CreateSoulObject( uidNPC, uidAbsorber, nSoulType, 2000);

	return CR_TRUE;
}