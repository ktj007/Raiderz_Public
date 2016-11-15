#pragma once

#include "XDummyPattern.h"

class XBirdDummyAgent;

class XDummyHandler
{
public:
	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCommand) = 0;
	virtual const wchar_t* GetName() = 0;
};

class XDummyHandlerMgr : public map<wstring, XDummyHandler*>
{
public:
	virtual ~XDummyHandlerMgr();
	void Clear();
	void Add(XDummyHandler* pNewHandler);
	XDummyHandler* GetHandler(const wchar_t* szName);
};




class XXMLDummyHandler : public XDummyHandler
{
private:
	XDummyPattern*			m_pPattern;
	wstring					m_strName;
public:
	XXMLDummyHandler() : m_pPattern(NULL)
	{
	}

	XXMLDummyHandler(XDummyPattern* pPattern)
	{
		m_pPattern		= pPattern;
		m_strName		= m_pPattern->GetName();
	}

	virtual MCommandResult OnCommand(XBirdDummyAgent* pAgent, MCommand* pCmd);
	virtual const wchar_t* GetName() { return m_strName.c_str(); }
};


