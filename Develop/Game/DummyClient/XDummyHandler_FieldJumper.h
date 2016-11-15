#pragma once

#include "XDummyHandler.h"

class XDummyHandler_FieldJumper: public XDummyHandler
{
public:
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand);
	virtual const wchar_t* GetName()	{ return L"FieldJumper";	}

};
