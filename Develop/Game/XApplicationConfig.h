#pragma once

class XApplicationConfig
{
public:
	static void Load(const wchar_t* szFileName);

public:
	static vector<wstring> m_vecIPString;	
};
