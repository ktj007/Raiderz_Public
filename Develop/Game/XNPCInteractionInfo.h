#pragma once

class XNPCInteractionInfo
{
private:

	INTERACTION_MENUTYPE			m_MenuType;
	wstring							m_strOpeningTextID;
	vector<TD_INTERACTION_ELEMENT>	m_vecInteractionElement;

	wstring							m_strOpeningText;	//º¯È¯¿ë

public:
	INTERACTION_MENUTYPE MenuType() const { return m_MenuType; }
	void MenuType(INTERACTION_MENUTYPE val) { m_MenuType = val; }

	void Clear();

	MWLua::table GetInteractionNodeTable();

	wstring&		GetOpeningTextID()		{ return m_strOpeningTextID; }
	vector<TD_INTERACTION_ELEMENT>&	GetList()	{ return m_vecInteractionElement; }

	const wchar_t* GetOpeningString();
};
