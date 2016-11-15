#pragma once

class CStringNode
{
public:
	enum Attribute
	{
		PLURAL
	};

	typedef map<Attribute, tstring>	attribute_map;
public:
	CStringNode();
	virtual ~CStringNode();

	const TCHAR* GetText(void) const;
	const TCHAR* GetAttribute(Attribute attr) const;

	void SetText(tstring strText);
	void SetAttribute(Attribute attr, tstring strText);

private:
	tstring m_strText;
	attribute_map	m_mapAttribute;
};
