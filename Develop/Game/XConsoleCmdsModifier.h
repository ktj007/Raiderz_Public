#ifndef _XCONSOLE_CMDS_MODIFIER_H
#define _XCONSOLE_CMDS_MODIFIER_H

#include "XConsoleCmds.h"

class XConsoleCmdsModifier
{
public:
	XConsoleCmdsModifier(){}
	~XConsoleCmdsModifier(){}

	void ClearCmds()
	{
		XConsoleCmds::m_CmdManager.Clear();
	}

	void PrintToConsole(const wchar_t* pSzText)
	{
		XConsoleCmds::Output(pSzText);
	}
};

#endif