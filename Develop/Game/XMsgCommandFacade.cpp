#include "stdafx.h"
#include "XMsgCommandFacade.h"
#include "XAnnounceMsgHandler.h"
#include "XSayMsgHandler.h"
#include "XWhisperMsgHandler.h"
#include "XNetwork.h"
#include "XGlobalMsgHandler.h"
#include "XShoutMsgHandler.h"
#include "XShoutFakeMsgHandler.h"
#include "XNoticeMsgHandler.h"
#include "XSystemMsgHandler.h"
#include "XAuctionMsgHandler.h"
#include "XPartyMsgHandler.h"
#include "XGuildMsgHandler.h"
#include "XFieldMsgHandler.h"
#include "XChannelMsgHandler.h"
#include "XNarrationMsgHandler.h"
#include "XAdviceMsgHandler.h"
#include "XWhisperBackMsgHandler.h"
#include "XGuideMsg.h"

XMsgCommandFacade::XMsgCommandFacade()
{
	SetHandler(MT_SAY, new XSayMsgHandler());
	SetHandler(MT_NARRATION, new XNarrationMsgHandler());
	SetHandler(MT_SHOUT, new XShoutMsgHandler());
	SetHandler(MT_SHOUT_FAKE, new XShoutFakeMsgHandler());
	SetHandler(MT_AUCTION, new XAuctionMsgHandler());
	SetHandler(MT_ANNOUNCE, new XAnnounceMsgHandler());	
	SetHandler(MT_WHISPER, new XWhisperMsgHandler());
	SetHandler(MT_WHISPER_BACK, new XWhisperBackMsgHandler());
	SetHandler(MT_PARTY, new XPartyMsgHandler());
	SetHandler(MT_GUILD, new XGuildMsgHandler());
	SetHandler(MT_CHANNEL, new XChannelMsgHandler());
	SetHandler(MT_FIELD, new XFieldMsgHandler());
	SetHandler(MT_GLOBAL, new XGlobalMsgHandler());
	SetHandler(MT_SYSTEM, new XSystemMsgHandler());
	SetHandler(MT_NOTICE, new XNoticeMsgHandler());
	SetHandler(MT_ADVICE, new XAdviceMsgHandler());
}

XMsgCommandFacade::~XMsgCommandFacade()
{
	// do nothing
}

bool XMsgCommandFacade::Post(MCommand* pCmd)
{
	_ASSERT(pCmd != NULL);

	MNetClient* pNetClient = global.net->GetClient();
	_ASSERT(pNetClient != NULL);

	MUID uidServer = pNetClient->GetServerUID();
	pCmd->SetReceiverUID(uidServer);
	
	return XPostCommand(pCmd);
}

bool XMsgCommandFacade::PostSayMsg(wstring strMsg)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandSayReq(strMsg);
	return Post(pNewCmd);
}

bool XMsgCommandFacade::PostWhisperMsg(wstring strMsg, wstring strReceiverName)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandWhisperReq(strMsg, strReceiverName);
	return Post(pNewCmd);
}

bool XMsgCommandFacade::PostPartyMsg(wstring strMsg)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandPartyReq(strMsg);
	return Post(pNewCmd);
}

bool XMsgCommandFacade::PostGuildMsg(wstring strMsg)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandGuildReq(strMsg);
	return Post(pNewCmd);
}

bool XMsgCommandFacade::PostShoutMsg(wstring strMsg)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandShoutReq(strMsg);
	return Post(pNewCmd);
}

bool XMsgCommandFacade::PostAuctionMsg(wstring strMsg)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandAuctionReq(strMsg);
	return Post(pNewCmd);
}

bool XMsgCommandFacade::PostChannelMsg(wstring strMsg, MUID uidChannel)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandChannelReq(strMsg, uidChannel);
	return Post(pNewCmd);
}

bool XMsgCommandFacade::PostFieldMsg(wstring strMsg)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandFieldReq(strMsg);
	return Post(pNewCmd);	
}

bool XMsgCommandFacade::PostGlobalMsg(wstring strMsg)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandGlobalReq(strMsg);
	return Post(pNewCmd);
}

bool XMsgCommandFacade::PostNarrationMsg(wstring strMsg)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandNarrationReq(strMsg);
	return Post(pNewCmd);
}


bool XMsgCommandFacade::PostAnnounceMsg(wstring strMsg)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandAnnounceReq(strMsg);	
	return Post(pNewCmd);
}


bool XMsgCommandFacade::PostNoticeMsg(wstring strMsg)
{
	MCommand* pNewCmd = m_msgHelper.MakeNewClientCommandNoticeReq(strMsg);	
	return Post(pNewCmd);
}