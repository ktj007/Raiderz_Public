#pragma once

#include "MSingleton.h"
#include "MintTypes.h"

class XColorGroup
{
private:
	wstring			m_strName;
	map<int,MCOLOR>	m_mapColors;
public:
	XColorGroup() {}
	~XColorGroup() {}
	int			GetID(size_t nIndex);
	void		SetColor(int nID, MCOLOR color);
	MCOLOR		GetColor(int nID);
	void		SetName(const wchar_t* szName) { m_strName = szName;}
	const wchar_t* GetName() { return m_strName.c_str(); }
	size_t		GetSize() { return m_mapColors.size(); }
};

class XColorTable
{
private:
	map<wstring, XColorGroup*>		m_ColorGroupMap;
	void DeleteAllColorGroups();
protected:
	void ParseColorTable(MXmlElement* pXmlElement);
	uint32 ParseColorValue( const wchar_t* szColorValue );
public:
	XColorTable() {}
	virtual ~XColorTable();
	bool Load(const wchar_t* szFileName);

	void AddColorGroup(XColorGroup* pColorGroup);
	MCOLOR GetColor(const wchar_t* szGroupName, int nID);
	XColorGroup* GetGroup(const wchar_t* szGroupName);
};