#pragma once

#include "ZFieldInfo.h"

class ZFieldInfoManager
{
public:
	typedef map<int, ZFieldInfo* >			ZFieldInfoMap;
	typedef map<int, FIELD_GROUP_INFO* >	ZFieldGroupInfoMap;

protected:
	ZFieldInfoMap		m_FieldInfoMap;
	ZFieldGroupInfoMap	m_FieldGroupInfoMap;

	CSFieldList			m_FieldList;

	wstring				m_strFieldPath;

	bool LoadField(const wchar_t* szFileName, ZFieldInfo* pFieldInfo);
	ZFieldInfo* NewFieldInfo(int nFieldID, CSFieldFileInfo* pFileInfo);

	bool LoadFieldGroupInfo(const wchar_t* szFileName);

public:
	ZFieldInfoManager();
	virtual ~ZFieldInfoManager();

	void Insert(ZFieldInfo* pFieldInfo);
	void InsertFieldGroup(FIELD_GROUP_INFO* pFieldGroup);

	void Clear();
	bool Load(const wchar_t* szFieldList, const wchar_t* szFieldPath, bool bIncludeDevFieldList);
	ZFieldInfo* Find(int nFieldID);
	FIELD_GROUP_INFO* FindGroupInfo(int nFieldGroupInfo);

	ZFieldInfoMap& GetFieldInfoMap() { return m_FieldInfoMap; }
};