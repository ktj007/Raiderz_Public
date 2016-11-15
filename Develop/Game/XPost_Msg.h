#ifndef _XPOST_MSG_H
#define _XPOST_MSG_H

#include "XNetClient.h"
#include "XCommandTable.h"
#include "XMsgCommandFacade.h"


inline void XPostMsg_Say(const wchar_t* szMsg)
{
	global.msg->PostSayMsg(szMsg);
}

inline void XPostMsg_Tell(const wchar_t* szPlayerID, const wchar_t* szMsg)
{
	global.msg->PostWhisperMsg(szMsg, szPlayerID);
}

inline void XPostMsg_Reply(const wchar_t* szMsg)
{
	global.msg->PostWhisperMsg(szMsg, gvar.Game.strLastWhisper.c_str());	
}

inline void XPostMsg_Party(const wchar_t* szMsg)
{
	global.msg->PostPartyMsg(szMsg);
}

inline void XPostMsg_Guild(const wchar_t* szMsg)
{
	global.msg->PostGuildMsg(szMsg);
}

inline void XPostMsg_Shout(const wchar_t* szMsg)
{
	global.msg->PostShoutMsg(szMsg);
}

inline void XPostMsg_Auction(const wchar_t* szMsg)
{
	global.msg->PostAuctionMsg(szMsg);
}

inline void XPostMsg_Channel(MUID uidChannel, const wchar_t* szMsg)
{
	global.msg->PostChannelMsg(szMsg, uidChannel);
}

inline void XPostMsg_Field(const wchar_t* szMsg)
{
	global.msg->PostFieldMsg(szMsg);
}

inline void XPostMsg_Global(const wchar_t* szMsg)
{
	global.msg->PostGlobalMsg(szMsg);
}

inline void XPostMsg_Narration(const wchar_t* szMsg)
{
	global.msg->PostNarrationMsg(szMsg);
}

inline void XPostMsg_Announce(const wchar_t* szMsg)
{
	global.msg->PostAnnounceMsg(szMsg);
}

inline void XPostMsg_Notice(const wchar_t* szMsg)
{
	global.msg->PostNoticeMsg(szMsg);
}

#endif
