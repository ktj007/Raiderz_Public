#pragma once

#include "XDummyHandler.h"

class XDummyHandler_InsertChar : public XDummyHandler
{
protected:
	minet::MCommandResult OnResponseLogin(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	minet::MCommandResult OnResponseAccountCharList(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	minet::MCommandResult OnResponseInsertChar(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
public:
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd);
	virtual const wchar_t* GetName() { return L"InsertChar"; }
};

