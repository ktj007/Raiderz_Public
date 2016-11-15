#pragma once

#include "XDummyHandler.h"

// 무한 로그인
// - 
class XDummyHandler_LoginFlood : public XDummyHandler
{
public:
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd);
	virtual const wchar_t* GetName() { return L"LoginFlood"; }
};



// 로그인, 로그아웃 반복
class XDummyHandler_LoginDisconnect : public XDummyHandler
{
public:
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd);
	virtual const wchar_t* GetName() { return L"LoginDisconnect"; }
};
