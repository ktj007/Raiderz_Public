#ifndef _XENGINE_DEBUGGER_H
#define _XENGINE_DEBUGGER_H

#include "MSingleton.h"
#include "MMockNetClient.h"

class XConsoleCmdsModifier;
class TestNetAgent;
class XEngineDebugContext;

class XEngineDebugger : public MSingleton<XEngineDebugger>
{
public:
	XEngineDebugger();
	virtual ~XEngineDebugger();

	void					OnConsoleCmd(const wchar_t* line, const int argc, wchar_t **const argv);
	void					Update(float fDelta);

	static void				OnConsoleCmd(const wchar_t* _realspace_debug_str);
	static void				UpdateBeforeGame(float fDelta);

	minet::MTestNetAgent*	GetNetAgent(){ return m_pNetAgent; }
	void					PrintToConsole(const wchar_t* pSzText);
	

private:
	XConsoleCmdsModifier*	m_pConsoleModifier;
	minet::MTestNetAgent*	m_pNetAgent;

	template< typename T > void AddDebugContext(const std::wstring _refStrSubCommand, const std::wstring _refStrHelpText)
	{
		MAP_DEB_CONTEXT::iterator itr = m_mapDebugContexts.find(_refStrSubCommand);
		if (itr == m_mapDebugContexts.end())
		{
			XEngineDebugContext* pContext = new T;
			pContext->m_pEngineDebugger = this;
			m_mapDebugContexts.insert( MAP_DEB_CONTEXT::value_type(_refStrSubCommand, pContext) );

			m_vecHelpText.push_back( _refStrSubCommand + L" : " + _refStrHelpText );
		}
	}

	typedef map< wstring, XEngineDebugContext*> MAP_DEB_CONTEXT;
	MAP_DEB_CONTEXT			m_mapDebugContexts;
	vector<wstring>			m_vecHelpText;
};

#endif // _XENGINE_DEBUGGER_H