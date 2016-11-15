#pragma once

#include "XDummyHandler.h"

class XDummyHandler_DeleteAllCharacter : public XDummyHandler
{
protected:
	minet::MCommandResult OnResponseLogin(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	minet::MCommandResult OnResponseAccountCharList(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	minet::MCommandResult OnResponseDeleteChar(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
public:
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd);
	virtual const wchar_t* GetName() { return L"DeleteAllCharacter"; }
};
