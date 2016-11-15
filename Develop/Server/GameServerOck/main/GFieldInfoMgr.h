#pragma once

#include "GFieldInfo.h"

class GManagedFieldInfoMgr;
class GPickingZTable;
class GCollisionTreeMgr;

/// 전체 맵정보 관리자
class GFieldInfoMgr : public map<int, GFieldInfo* >, public MTestMemPool<GFieldInfoMgr> 
{
public:
	typedef map<int, FIELD_GROUP_INFO* > FIELD_GROUP_MAP;
	typedef map<int,GPickingZTable*>	 PickingTableMap;
private:
	FIELD_GROUP_MAP	m_mapFieldGroup;
	PickingTableMap	m_mapPickingZTable;
	GCollisionTreeMgr*	m_pCollisionTreeMgr;

	CSZoneList		m_ZoneList;
	CSFieldList		m_FieldList;

	wstring			m_strZonePath;
	wstring			m_strFieldPath;
private:
	bool LoadField(const wchar_t* szFileName, GFieldInfo* pFieldInfo);
	bool LoadFieldGroupInfo(const wchar_t* szFileName);
	GFieldInfo* NewFieldInfo(int nFieldID, CSFieldFileInfo* pFileInfo);
	GPickingZTable* LoadPickingZ(wstring strZonePath, GFieldInfo* pFieldInfo);
public:
	GFieldInfoMgr(void);
	virtual ~GFieldInfoMgr(void);
	void Insert( GFieldInfo* pFieldInfo );
	void InsertGroup( FIELD_GROUP_INFO* pInfo );

	void Clear();
	bool Load(const wchar_t* szZoneList, const wchar_t* szFieldList, const wchar_t* szZonePath, const wchar_t* szFieldPath, bool bIncludeDevFieldList);
	bool ReloadField(int nFieldID);

	GFieldInfo* Find(int nID);
	vec3 FindNPCPos(int nNPCID, int& nFieldID);
	FIELD_GROUP_INFO* FindFieldGroup(int nID);

	FIELD_GROUP_MAP& GetFieldGroupMap() { return m_mapFieldGroup; }
	GCollisionTreeMgr* GetCollisionTreeMgr()		{ return m_pCollisionTreeMgr; }
};


