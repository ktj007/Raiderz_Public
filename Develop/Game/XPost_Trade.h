#ifndef _XPOST_TRADE_H
#define _XPOST_TRADE_H

inline void XPostTrade_Start(MUID uidPlayer)
{
	XPOSTCMD1(MC_TRADE_START_REQ, MCommandParameterUID(uidPlayer));
}

inline void XPostTrade_StartAccept(bool bAccept)
{
	XPOSTCMD1(MC_TRADE_START_ACCEPT_REQ, MCommandParameterBool(bAccept));
}

inline void XPostTrade_PutUpItem(int nSlotID, int nAmount)
{
	XPOSTCMD1(MC_TRADE_PUTUP_ITEM_REQ, MCommandParameterSingleBlob(TD_TRADE_ITEM_PUTUP_REQ(nSlotID, nAmount)));
}

inline void XPostTrade_PutDownItem(int nSlotID, int nAmount)
{
	XPOSTCMD1(MC_TRADE_PUTDOWN_ITEM_REQ, MCommandParameterSingleBlob(TD_TRADE_ITEM_PUTDOWN(nSlotID, nAmount)));
}

inline void XPostTrade_PutUpSilver(int nPutUpSilver)
{
	XPOSTCMD1(MC_TRADE_PUTUP_MONEY_REQ, MCommandParameterInt(nPutUpSilver));
}

inline void XPostTrade_Cancel()
{
	XPOSTCMD0(MC_TRADE_CANCEL_REQ);
}

inline void XPostTrade_Confirm()
{
	XPOSTCMD0(MC_TRADE_CONFIRM_REQ);
}

#endif