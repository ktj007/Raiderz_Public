#pragma once

#include "CSDef.h"
#include "CSMsgCommandHelper.h"

class CSMsgHandler
{
public:
	CSMsgHandler();
	virtual ~CSMsgHandler();

	bool Request(const minet::MCommand* pCmd);
	bool Response(const minet::MCommand* pCmd);
	bool GetCommandParameter(const minet::MCommand* pCmd);

	virtual bool OnPreRequest(const minet::MCommand* pCmd);		// 욕설필터링 등 메시지 선처리용
	virtual bool OnPreResponse(const minet::MCommand* pCmd);	// 받을때 처리

	virtual bool OnRequest(const minet::MCommand* pCmd) = 0;
	virtual bool OnResponse(const minet::MCommand* pCmd) = 0;	
	virtual CSMsgType GetHandlerType(void) = 0;

protected:
	CSMsgCommandHelper m_msgHelper;
	
	MUID m_uidCommandSender;
	CSMsgType m_eType;
	tstring m_strMsg;
	int m_nAdditional;	
};
