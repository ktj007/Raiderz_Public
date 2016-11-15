#pragma once

#include "XDummyHandler.h"

class XDummyHandler_FieldLogin : public XDummyHandler
{
protected:
	minet::MCommandResult OnResponseLogin(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	minet::MCommandResult OnResponseAccountCharList(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	minet::MCommandResult OnResponseInsertChar(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	minet::MCommandResult OnResponseSelMyChar(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
	minet::MCommandResult OnMoveToGameServer( XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd );
	minet::MCommandResult OnChangeField(XBirdDummyAgent* pAgent, XBirdClient* pClient, MCommand* pCmd);
public:
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd);
	virtual const wchar_t* GetName() { return L"FieldLogin"; }
};


class XDummyHandler_FieldLoginDisconnect : public XDummyHandler
{
public:
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd);
	virtual const wchar_t* GetName() { return L"FieldLoginDisconnect"; }
};
