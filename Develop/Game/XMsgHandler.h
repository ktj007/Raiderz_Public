#pragma once

#include "CSMsgHandler.h"

class XMsgHandler: public CSMsgHandler
{
public:
	XMsgHandler();
	virtual ~XMsgHandler();

	virtual bool OnRequest(const minet::MCommand* pCmd) sealed;	
	virtual bool OnPreResponse(const minet::MCommand* pCmd);

	bool FilterPvPArea(wstring& strSenderName, wstring& strMSg);
};
