#pragma once

#include "XNetClient.h"
#include "XCommandTable.h"


inline void XPostMail_SendMail(const wchar_t* szReceiver, const wchar_t* szTitle, const wchar_t* szContent, int nMoney)
{
	TD_MAIL_POST_REQ_INFO tdMailPost;
	_tcsncpy_s(tdMailPost.strReceiverName,	szReceiver,	_TRUNCATE);
	_tcsncpy_s(tdMailPost.strMailTitle,		szTitle,	_TRUNCATE);	
	tdMailPost.nAppendedMoney = nMoney;

	XPOSTCMD2(MC_MAIL_POST_REQ, MCommandParameterSingleBlob(tdMailPost), MCommandParameterWideString(szContent));
}

inline void XPostMail_ReadMail(MUID uidMail)
{
	XPOSTCMD1(MC_MAIL_READ_REQ, MCommandParameterUID(uidMail));
}

inline void XPostMail_DeleteMail(MUID uidMail)
{
	XPOSTCMD1(MC_MAIL_DELETE_REQ, MCommandParameterUID(uidMail));
}

inline void XPostMail_PutUpItem( int nIndex, int nQty)
{
	TD_MAIL_RESERVE_ITEM tdMailReserverItem;
	tdMailReserverItem.nInventorySlotID = nIndex;
	tdMailReserverItem.nItemAmount = nQty;

	XPOSTCMD1(MC_MAIL_APPEND_PUTUP_ITEM_REQ, MCommandParameterSingleBlob(tdMailReserverItem));
}

inline void XPostMail_PutDownItem( int nIndex, int nQty)
{
	TD_MAIL_RESERVE_ITEM tdMailReserverItem;
	tdMailReserverItem.nInventorySlotID = nIndex;
	tdMailReserverItem.nItemAmount = nQty;

	XPOSTCMD1(MC_MAIL_APPEND_PUTDOWN_ITEM_REQ, MCommandParameterSingleBlob(tdMailReserverItem));	
}

inline void XPostMail_ResetItems(void)
{
	XPOSTCMD0(MC_MAIL_APPEND_RESET_ITEM_REQ);
}

inline void XPostMail_TakeAppendedItem(MUID uidMail, int nSlotID)
{
	XPOSTCMD2(MC_MAIL_TAKE_APPENDED_ITEM_REQ, MCommandParameterUID(uidMail), MCommandParameterUChar(nSlotID));
}

inline void XPostMail_TakeAppendedMoney(MUID uidMail)
{
	XPOSTCMD1(MC_MAIL_TAKE_APPENDED_MONEY_REQ, MCommandParameterUID(uidMail));
}
