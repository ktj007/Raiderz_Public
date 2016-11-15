#pragma once


class XMintScriptDeclarer
{
public:
	void			DeclWidget(MWLua* pLua, const char* szClass, const char* szName, MWidget* pWidget);
	void			InitDeclWidgets(MWLua* pLua);
};


