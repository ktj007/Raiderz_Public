#include "stdafx.h"
#include "GFieldInfoMgr.h"
#include "GDef.h"
#include "GGlobal.h"
#include "GFieldMgr.h"
#include "GFieldInfoLoader.h"
#include "GConfig.h"
#include "GMasterServerFacade.h"
#include "GPickingZTable.h"
#include "AStlUtil.h"
#include "GConst.h"
#include "GCollisionTreeMgr.h"
#include "GFilePath.h"

GFieldInfoMgr::GFieldInfoMgr(void) : m_pCollisionTreeMgr(new GCollisionTreeMgr())
{
	m_strZonePath = PATH_ZONE;
}

GFieldInfoMgr::~GFieldInfoMgr(void)
{
	Clear();
	SAFE_DELETE(m_pCollisionTreeMgr);
}

void GFieldInfoMgr::Clear()
{
	m_ZoneList.Clear();
	m_FieldList.Clear();

	while (!empty())
	{
		iterator itor = begin();
		delete (*itor).second;
		erase(itor);
	}

	for each (pair<int, FIELD_GROUP_INFO*> each in m_mapFieldGroup)
	{
		delete each.second;
	}
	m_mapFieldGroup.clear();

	SAFE_DELETE_MAP(m_mapPickingZTable);
	
}

bool GFieldInfoMgr::Load(const wchar_t* szZoneList, const wchar_t* szFieldList, const wchar_t* szZonePath, const wchar_t* szFieldPath, bool bIncludeDevFieldList)
{
	m_strZonePath = szZonePath;
	m_strFieldPath = szFieldPath;

	if (m_ZoneList.Load(szZoneList) == false) return false;
	if (m_FieldList.Load(szFieldList, bIncludeDevFieldList) == false) return false;

	// 모든 필드 로딩
	for (CSFieldList::iterator iter=m_FieldList.begin();
		iter !=m_FieldList.end();
		++iter)
	{
		NewFieldInfo((*iter).first, (*iter).second);
	}

	if (LoadFieldGroupInfo(szFieldList) == false) return false;

	mlog("Loaded Field Files: %d\n", m_FieldList.size());
	mlog("Loaded Zone Files: %d\n", m_ZoneList.size());
	mlog("Loaded PickingZ Files: %d\n", m_mapPickingZTable.size());
	
	return true;
}



bool GFieldInfoMgr::ReloadField(int nFieldID)
{
	GFieldInfo* pFieldInfo = Find(nFieldID);
	if (!pFieldInfo)	return false;

	GFieldInfoLoader loader;
	if (!loader.ReloadField(pFieldInfo, m_strFieldPath))
		return false;

	wstring strSystemPath = L"./" + wstring(PATH_SYSTEM);
	wstring strFieldListFileName = GFilePath::MakeFieldList(strSystemPath.c_str(), true);
	m_mapFieldGroup.clear();
	return LoadFieldGroupInfo(strFieldListFileName.c_str());
}

bool GFieldInfoMgr::LoadFieldGroupInfo(const wchar_t* szFileName)
{
	GFieldInfoLoader loader;
	return loader.LoadFieldGroupInfo(&m_mapFieldGroup, szFileName);
}


GFieldInfo* GFieldInfoMgr::Find(int nID)
{
	iterator itor = find(nID);
	if (itor != end())
	{
		return static_cast<GFieldInfo*> ( (*itor).second );
	}

	return NULL;
}

FIELD_GROUP_INFO* GFieldInfoMgr::FindFieldGroup(int nID) 
{ 
	FIELD_GROUP_MAP::iterator iter = m_mapFieldGroup.find(nID);
	if (iter == m_mapFieldGroup.end())
		return NULL;

	return (*iter).second;
}

GFieldInfo* GFieldInfoMgr::NewFieldInfo( int nFieldID, CSFieldFileInfo* pFieldFileInfo )
{
	GFieldInfo* pFieldInfo = new GFieldInfo();
	pFieldInfo->Assign(pFieldFileInfo);

	Insert(pFieldInfo);

	GFieldInfoLoader loader;
	if (loader.LoadField(pFieldInfo, m_strFieldPath, m_strZonePath, m_ZoneList) == false)
	{
		GFieldInfoMgr::iterator itor = find(pFieldInfo->m_nFieldID);
		if (itor != end()) erase(itor);
		delete pFieldInfo;
		return NULL;
	}

	// PickingZ 정보 로딩
	if (m_mapPickingZTable.find(pFieldInfo->m_nZoneID) == m_mapPickingZTable.end())
	{
		m_mapPickingZTable.insert(make_pair(pFieldInfo->m_nZoneID, LoadPickingZ(m_strZonePath, pFieldInfo)));
	}
	pFieldInfo->SetPickingZTable(m_mapPickingZTable[pFieldInfo->m_nZoneID]);
		
	return pFieldInfo;
}

GPickingZTable* GFieldInfoMgr::LoadPickingZ(wstring strZonePath, GFieldInfo* pFieldInfo)
{
	VALID_RET(pFieldInfo, NULL);
	GPickingZTable::Desc desc;
	desc.pCollisionTree = pFieldInfo->GetCollisionTree();
	VALID_RET(desc.pCollisionTree, NULL);
	desc.pPathFinder = gsys.pPathFinder;
	VALID_RET(desc.pPathFinder, NULL);
	desc.nZoneID = pFieldInfo->m_nZoneID;
	desc.fMaxX = pFieldInfo->m_fMaxX;
	desc.fMinX = pFieldInfo->m_fMinX;
	desc.fMaxY = pFieldInfo->m_fMaxY;
	desc.fMinY = pFieldInfo->m_fMinY;

	GPickingZTable* pPickingZTable = new GPickingZTable(desc);
	wstring strFilename = strZonePath + pFieldInfo->m_strZoneFile + L".zpk";

	MTimeChecker2 time_checker;
	time_checker.Start();

	wstring strAreaFilename = strZonePath + pFieldInfo->m_strZoneFile + L".pickz.xml";
	pPickingZTable->LoadPickingZSize(strAreaFilename);

	if(MIsExistFile(strFilename.c_str()))
	{
		pPickingZTable->LoadFromFile(strFilename);
		dlog("PickingZ Loaded (zone=%d, filename=%s, elapsed: %.1f seconds)\n", pFieldInfo->m_nZoneID, MLocale::ConvUTF16ToAnsi(strFilename.c_str()).c_str(), time_checker.Check() * 0.001f);
	}
	else
	{
		mlog("PickingZ File Not Found, Now cooking : zone=%d, filename=%s\n", pFieldInfo->m_nZoneID, MLocale::ConvUTF16ToAnsi(strFilename.c_str()).c_str());
		pPickingZTable->RunSamplingWork();
		pPickingZTable->SaveToFile(strFilename);
		mlog("Cooking Complete (elapsed: %.1f seconds)\n", time_checker.Check() * 0.001f);
	}

	return pPickingZTable;
}

void GFieldInfoMgr::Insert( GFieldInfo* pFieldInfo )
{
	if (NULL == pFieldInfo) return;

	this->insert(GFieldInfoMgr::value_type(pFieldInfo->m_nFieldID, pFieldInfo));
}

void GFieldInfoMgr::InsertGroup( FIELD_GROUP_INFO* pInfo )
{
	if (NULL == pInfo) return;

	m_mapFieldGroup.insert(GFieldInfoMgr::FIELD_GROUP_MAP::value_type(pInfo->nID, pInfo));
}

vec3 GFieldInfoMgr::FindNPCPos( int nNPCID, int& nFieldID)
{
	GFieldInfo* pFieldInfo = Find(nFieldID);

	if (NULL != pFieldInfo)
	{
		return pFieldInfo->GetSpawnPos(nNPCID);
	}
	else
	{
		for each (const map<int, GFieldInfo*>::value_type& val in *this)
		{
			GFieldInfo* pFieldInfo = val.second;
			if (NULL == pFieldInfo) continue;

			vec3 vPos = pFieldInfo->GetSpawnPos(nNPCID);
			if (vec3::ZERO != vPos)
			{
				nFieldID = pFieldInfo->m_nFieldID;
				return vPos;
			}
		}
	}

	return vec3::ZERO;
}

void GFieldWeatherInfo::Clear()
{
	nDefault = DEFAULT_GAME_WEATHER;
	vecNodes.clear();
	vecRandomData.clear();
}