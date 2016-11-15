#pragma once

class XDialogTextInfoMgr
{
public:
	XDialogTextInfoMgr();
	void Clear();

	bool Load(const wchar_t* szFileName);
	bool Reload(const wchar_t* szFileName);

	const wchar_t* Get(int nTextID);

private:
	void ParseDialog(MXmlElement* pElement, MXml* pXml);
	void ParseSay(MXmlElement* pElement, MXml* pXml);
	void ParseSelect(MXmlElement* pElement, MXml* pXml);

	int	MakeNewTextID();
	void InsertNewText(int nTextID, wstring strText);

private:
	int					m_nNowTextID;
	map<int, wstring>	m_mapText;
};
