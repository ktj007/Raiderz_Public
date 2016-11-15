#pragma once


// 사전 노드
class CSWordNode
{
public:
	enum Attribute
	{
		PROTOTYPE,
		PLURAL
	};

	int	m_nID;
	wstring m_strKey;
	map<Attribute, wstring> m_mapWord;
};


// 사전
class CSDictionary
{
public:
	typedef map<int, CSWordNode*>		id_map;
	typedef map<wstring, CSWordNode*>	key_map;
	typedef map<wstring, CSWordNode*>	prototype_map;

public:
	CSDictionary();
	virtual ~CSDictionary();

	bool Load(wstring strDictionaryFilename);
	
	virtual wstring Plural(wstring strPrototype);

private:
	id_map			m_mapId;
	key_map			m_mapKey;
	prototype_map	m_mapPrototype;
};
