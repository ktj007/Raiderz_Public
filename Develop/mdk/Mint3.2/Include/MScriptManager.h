#pragma once

class MWLua;

#include "MScriptHelper.h"

namespace mint3 {

/// Mint의 루아 스크립트를 관리
class MScriptManager
{
private:
protected:
	MWLua*			m_pLua;
	MScriptHelper	m_ScriptHelper;
	bool			m_bEnable;

	bool			InitDeclWidgets();
	void			InitDeclEvents();

public:
				MScriptManager();
	virtual		~MScriptManager();
	bool		Initialize(MWLua* pLua);

	void		EnableCallScript( bool bEnable)		{ m_bEnable = bEnable;		}
	bool		IsEnableCallScript()				{ return m_bEnable;			}

	void		DeclWidget(const char* szClass, const char* szName, MWidget* pWidget);
	void		ReleaseWidget(const char* szName);
	void		AddScriptEvent(string& strWidgetName, string& strEventName, string& strScript);

	void		RunScriptFile(const char *szFileName);
	void		RunScriptString(const char* szStream);
	bool		RunScriptEvent(MWidget* pWidget, const char* szEventName);
	bool		RunScriptEvent( const char* szName, const char* szEvent);
	MWLua*		GetLua() { return m_pLua; }
};



} // namespace mint3