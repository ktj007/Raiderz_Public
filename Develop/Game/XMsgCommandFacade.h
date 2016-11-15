#pragma once

#include "CSMsgCommandFacade.h"
#include "CSMsgCommandHelper.h"

class XMsgCommandFacade: public CSMsgCommandFacade
{
public:
	XMsgCommandFacade();
	virtual ~XMsgCommandFacade();

	bool Post(MCommand* pCmd);

	bool PostSayMsg(wstring strMsg);
	bool PostWhisperMsg(wstring strMsg, wstring strReceiverName);	
	bool PostPartyMsg(wstring strMsg);	
	bool PostGuildMsg(wstring strMsg);
	bool PostShoutMsg(wstring strMsg);
	bool PostAuctionMsg(wstring strMsg);		
	bool PostChannelMsg(wstring strMsg, MUID uidChannel);	     
	bool PostFieldMsg(wstring strMsg);
	bool PostGlobalMsg(wstring strMsg);
	bool PostNarrationMsg(wstring strMsg);
	bool PostAnnounceMsg(wstring strMsg);
	bool PostNoticeMsg(wstring strMsg);

private:
	CSMsgCommandHelper	m_msgHelper;
};
