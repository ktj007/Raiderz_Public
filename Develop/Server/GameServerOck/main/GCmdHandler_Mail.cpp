#include "StdAfx.h"
#include "GCmdHandler_Mail.h"
#include "CCommandTable.h"
#include "CTransData.h"
#include "GGlobal.h"
#include "GMailSystem.h"
#include "GMailItemTaker.h"
#include "GMailMoneyTaker.h"
#include "CCommandResultTable.h"
#include "GCommandCenter.h"
#include "GCommand.h"
#include "GPlayerObjectManager.h"
#include "GPlayerMailReserver.h"
#include "GEntityPlayer.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "GNPCInteractionInfo.h"


GCmdHandler_Mail::GCmdHandler_Mail(MCommandCommunicator* pCC) : MCommandHandler(pCC)
{
	SetCmdHandler(MC_MAIL_POST_REQ,						OnPostReq);
	SetCmdHandler(MC_MAIL_READ_REQ,						OnReadReq);
	SetCmdHandler(MC_MAIL_DELETE_REQ,					OnDeleteReq);
	SetCmdHandler(MC_MAIL_TAKE_APPENDED_ITEM_REQ,		OnTakeAppendedItemReq);
	SetCmdHandler(MC_MAIL_TAKE_APPENDED_MONEY_REQ,		OnTakeAppendedMoneyReq);
	SetCmdHandler(MC_MAIL_APPEND_PUTUP_ITEM_REQ,		OnPutUpItemReq);
	SetCmdHandler(MC_MAIL_APPEND_PUTDOWN_ITEM_REQ,		OnPutDownItemReq);
	SetCmdHandler(MC_MAIL_APPEND_RESET_ITEM_REQ,		OnResetAppendedItem);
}

MCommandResult GCmdHandler_Mail::OnPostReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 인자받기
	TD_MAIL_POST_REQ_INFO* pInfo;
	wstring strMailText;

	if (!pCmd->GetSingleBlob(pInfo, 0))					return CR_ERROR;
	if (!pCmd->GetParameter(strMailText, 1, MPT_WSTR))	return CR_ERROR;


	// 요청자 확인
	MUID uidRequester = pCmd->GetSenderUID();
	GEntityPlayer* pRequestPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidRequester);

	if (pRequestPlayer == NULL)
	{
		return CR_TRUE;
	}

	/// 인터렉션 조건 체크.
	if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pRequestPlayer, IT_MAILBOX))	return CR_FALSE;


	// 편지 보내기 요청
	gsys.pMailSystem->PostMail(pRequestPlayer, pInfo, strMailText);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Mail::OnReadReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 인자 받기
	MUID uidMail;	
	if (!pCmd->GetParameter(&uidMail, 0, MPT_UID))	return CR_ERROR;
		

	// 유저 확인
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);

	if (pPlayer == NULL)
	{
		return CR_TRUE;
	}

	/// 인터렉션 조건 체크.
	if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_MAILBOX))	return CR_FALSE;


	// 편지 보기
	gsys.pMailSystem->ReadMail(pPlayer, uidMail);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Mail::OnDeleteReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	MUID uidMail;

	if (!pCmd->GetParameter(&uidMail, 0, MPT_UID))	return CR_ERROR;

	// 요청자 확인
	MUID uidRequester = pCmd->GetSenderUID();
	GEntityPlayer* pRequestPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidRequester);
	if (pRequestPlayer == NULL)
		return CR_TRUE;

	/// 인터렉션 조건 체크.
	if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pRequestPlayer, IT_MAILBOX))	return CR_FALSE;

	gsys.pMailSystem->DeleteMail(pRequestPlayer, uidMail);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Mail::OnTakeAppendedItemReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	MUID uidMail;
	UCHAR nSlotIndex;
	if (false == pCmd->GetParameter(&uidMail, 0, MPT_UID)) return CR_FALSE;
	if (false == pCmd->GetParameter(&nSlotIndex, 1, MPT_UCHAR)) return CR_FALSE;

	/// 인터렉션 조건 체크.
	if (!gsys.pInteractionSystem->GetNPCInteractor().CheckIProgress(pPlayer, IT_MAILBOX))	return CR_FALSE;

	gsys.pMailSystem->GetItemTaker()->Take(pPlayer, uidMail, static_cast<int>(nSlotIndex));

	return CR_TRUE;
}

MCommandResult GCmdHandler_Mail::OnTakeAppendedMoneyReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(pCmd->GetSenderUID());
	if (NULL == pPlayer) return CR_FALSE;

	MUID uidMail;
	if (false == pCmd->GetParameter(&uidMail, 0, MPT_UID)) return CR_FALSE;

	gsys.pMailSystem->GetMoneyTaker()->TakeAppendedMoney(pPlayer, uidMail);

	return CR_TRUE;
}

MCommandResult GCmdHandler_Mail::OnPutUpItemReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 인자 확인
	TD_MAIL_RESERVE_ITEM* pItem;
	if (!pCmd->GetSingleBlob(pItem, 0))	return CR_ERROR;

	
	// 요청자 확인
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);

	if (pPlayer == NULL)
	{
		return CR_TRUE;
	}


	// 첨부 아이템 추가
	gsys.pMailSystem->PutUpItem(pPlayer, pItem);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Mail::OnPutDownItemReq(MCommand* pCmd, MCommandHandler* pHandler)
{
	// 인자 확인
	TD_MAIL_RESERVE_ITEM* pItem;
	if (!pCmd->GetSingleBlob(pItem, 0))	return CR_ERROR;


	// 요청자 확인
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);

	if (pPlayer == NULL)
	{
		return CR_TRUE;
	}


	// 첨부 아이템 제거
	gsys.pMailSystem->PutDownItem(pPlayer, pItem);


	return CR_TRUE;
}

MCommandResult GCmdHandler_Mail::OnResetAppendedItem(MCommand* pCmd, MCommandHandler* pHandler)
{
	/*	인자없는 커멘드입니다.	*/

	// 요청 플레이어의 첨부아이템 예약정보를 초기화합니다.
	MUID uidPlayer = pCmd->GetSenderUID();
	GEntityPlayer* pPlayer = gmgr.pPlayerObjectManager->GetEntityInWorld(uidPlayer);

	if (pPlayer == NULL)
	{
		return CR_TRUE;
	}

	GPlayerMailReserver& reserver = pPlayer->GetMailReserver();
	reserver.Clear();

	
	return CR_TRUE;
}
