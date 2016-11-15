#pragma once

#include "XStrCmd.h"


class XChatCmdLoader
{
public:
	struct _CmdStr
	{
		wchar_t			szName[XSTRCMD_TEXSIZE];
		wchar_t			szUsage[XSTRCMD_TEXSIZE];
		wchar_t			szHelp[XSTRCMD_TEXSIZE];
		vector<wstring>	vecAliases;
		_CmdStr()
		{
			szName[0] = szUsage[0] = szHelp[0] = 0;
		}
	};
private:
	map<int, _CmdStr*>		m_CmdMap;
	void ParseCmd(MXmlElement* pElement, MXml& xml);
public:
	XChatCmdLoader() {}
	~XChatCmdLoader();
	bool Load();
	_CmdStr* GetStr(int nID);
};
