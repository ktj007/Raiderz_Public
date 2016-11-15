#ifndef _XPOST_GUILD_H
#define _XPOST_GUILD_H

#include "XNetClient.h"
#include "XCommandTable.h"

// ±æµå »ý¼º
inline void XPostGuild_Create(const wchar_t* szName)
{
	XPOSTCMD1(MC_GUILD_CREATE_REQ, MCommandParameterWideString(szName));
}

// ±æµå ÃÊ´ë ¿äÃ»
inline void XPostGuild_Invite_Req(const MUID& uidGuest)
{
	XPOSTCMD1(MC_GUILD_INVITE_REQ, MCommandParameterUID(uidGuest));
}

// ±æµå ÃÊ´ë ÀÀ´ä
inline void XPostGuild_Invite_Res(bool bAccept)
{
	XPOSTCMD1(MC_GUILD_INVITE_RES, MCommandParameterBool(bAccept));
}

// ±æµå Å»Åð
inline void XPostGuild_Leave()
{
	XPOSTCMD0(MC_GUILD_LEAVE_REQ);
}

// ±æµå °­Åð
inline void XPostGuild_Kick(const wchar_t* szTargetName)
{
	XPOSTCMD1(MC_GUILD_KICK_REQ, MCommandParameterWideString(szTargetName));
}

// ±æµå ÇØÃ¼
inline void XPostGuild_Destroy()
{
	XPOSTCMD0(MC_GUILD_DESTROY_REQ);
}

// ±æ¸¶ º¯°æ
inline void XPostGuild_ChangeMaster(const wchar_t* szNewMasterName)
{
	XPOSTCMD1(MC_GUILD_CHANGE_MASTER_REQ, MCommandParameterWideString(szNewMasterName));
}

// ±æµå º¸°üÇÔ ÀÔ±Ý ¿äÃ»
inline void XPostGuild_DepositStorageMoney(int nMoney)
{
	XPOSTCMD1(MC_GUILD_DEPOSIT_STORAGEMONEY_REQ, MCommandParameterInt(nMoney));
}

// ±æµå º¸°üÇÔ ­ˆ±Ý ¿äÃ»
inline void XPostGuild_WithdrawStorageMoney(int nMoney)
{
	XPOSTCMD1(MC_GUILD_WITHDRAW_STORAGEMONEY_REQ, MCommandParameterInt(nMoney));
}

#endif