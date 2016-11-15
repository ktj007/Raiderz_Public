#include "StdAfx.h"
#include "GFieldInfo.h"
#include "GDef.h"
#include "GPathFinder.h"
#include "GCollisionTree.h"
#include <stdlib.h>
#include "GValidateLogger.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GStrings.h"
#include "GFieldMgr.h"
#include "GScriptGroup_Field.h"
#include "GFieldAttrGrid.h"
#include "GConfig.h"
#include "GPickingZTable.h"
#include "GAIRoomMgr.h"
#include "GFieldInfoMgr.h"
#include "GCollisionTreeMgr.h"
#include "GNPCInfoMgr.h"
#include "GCutsceneInfoMgr.h"

GFieldInfo::GFieldInfo() 
: SFieldInfo()
, m_pCollisionTree(NULL)
, m_nParentID(0)
, m_fMinX(0.0f)
, m_fMaxX(0.0f)
, m_fMinY(0.0f)
, m_fMaxY(0.0f)
, m_nType(FIELDTYPE_NONE)
, m_pPickingZTable(NULL)
, m_fSectorHeight(0.0f)
, m_pFieldGroup(NULL)
, m_nQPVPNPCID1(0)
, m_nQPVPNPCID2(0)
, m_nQPVPRebirthMarkerID1(0)
, m_nQPVPRebirthMarkerID2(0)
, m_nIntroCutsceneID(INVALID_ID)
{
	for (int i = 0; i < MAX_GATHER_TYPE; i++)
	{
		m_nMaxGatherInstance[i] = 0;
	}

	m_pAttrGrid = new GFieldAttrGrid(this);
}


GFieldInfo::~GFieldInfo()
{
	m_pAttrGrid->Fini();
	SAFE_DELETE(m_pAttrGrid);
}

void GFieldInfo::Init( const wstring& strZonePath, const wstring& strFile )
{
	MTimeChecker2 tcPartial;
	wstring strCollisionFileName = strZonePath + strFile;

	// 주의!! LoadCollision이 먼저 호출되고, LoadPath가 호출되어야만
	//  HeightField를 가진 Zone에서 iTerrainCallback이 제대로 동작합니다.
	tcPartial.Start();
	LoadCollision(strCollisionFileName);
	dlog("\t\tLoadCollision(elapsed: %.1f seconds)\n", tcPartial.Check() * 0.001f);

	tcPartial.Start();
	LoadCommon(strCollisionFileName);
	dlog("\t\tLoadCommon(elapsed: %.1f seconds)\n", tcPartial.Check() * 0.001f);

	tcPartial.Start();
	LoadPath(strZonePath, strFile);
	dlog("\t\tLoadPath(elapsed: %.1f seconds)\n", tcPartial.Check() * 0.001f);

	tcPartial.Start();
	InitWeather();
	dlog("\t\tInitWeather(elapsed: %.1f seconds)\n", tcPartial.Check() * 0.001f);

	tcPartial.Start();
	m_pAttrGrid->Init();
	dlog("\t\tAttrGridInit(elapsed: %.1f seconds)\n", tcPartial.Check() * 0.001f);
}

void GFieldInfo::InitWeather()
{
	// 날씨 랜덤값 설정
	m_WeatherInfo.vecRandomData.clear();

	bool bFillRandomData = true;
	if (m_WeatherInfo.vecNodes.empty()) bFillRandomData = false;
	else if (m_WeatherInfo.vecNodes.size() == 1 && m_WeatherInfo.vecNodes[0].nWeather == m_WeatherInfo.nDefault)
	{
		bFillRandomData = false;
	}

	if (bFillRandomData)
	{
		for (size_t i = 0; i < m_WeatherInfo.vecNodes.size(); i++)
		{
			for (int j = 0; j < m_WeatherInfo.vecNodes[i].nPercent; j++)
			{
				m_WeatherInfo.vecRandomData.push_back((m_WeatherInfo.vecNodes[i].nWeather));
			}
		}

		int nMore = 100 - (int)m_WeatherInfo.vecRandomData.size();
		for (int i = 0; i < nMore; i++)
		{
			m_WeatherInfo.vecRandomData.push_back(m_WeatherInfo.nDefault);
		}
	}
}

bool GFieldInfo::LoadCommon(wstring& strZoneFilename)
{
	wstring strXmlFilename = strZoneFilename + L".common.xml";

	return gmgr.pAIRoomMgr->Load(strXmlFilename);
}

bool GFieldInfo::LoadCollision(wstring& strZoneFilename)
{
	if (gmgr.pFieldInfoMgr == NULL)
	{
		_ASSERT(0);
		return false;
	}
	GCollisionTreeMgr* pCollisionTreeMgr = gmgr.pFieldInfoMgr->GetCollisionTreeMgr();

	m_pCollisionTree = pCollisionTreeMgr->Find(m_nZoneID);
	if (m_pCollisionTree)
	{
		return true;
	}

	m_pCollisionTree = new GCollisionTree();
	if (NULL == m_pCollisionTree) return false;

	if(!m_pCollisionTree->LoadZone(m_nZoneID, strZoneFilename))
	{
		mlog("Collision File Not Found : zone=%d , filename=%s\n", m_nZoneID, MLocale::ConvUTF16ToAnsi(strZoneFilename.c_str()).c_str());

		delete m_pCollisionTree;
		m_pCollisionTree = NULL;
		return false;
	}

	pCollisionTreeMgr->Insert(m_nZoneID, m_pCollisionTree);

	return true;
}

bool GFieldInfo::LoadPath(const wstring& strZonePath, const wstring& strPathFile)
{
	wstring strFile = strZonePath + strPathFile;
	
	RHeightField *pHeightField = NULL;
	if ( m_pCollisionTree )
		pHeightField = m_pCollisionTree->GetHeightField();

	if (gsys.pPathFinder)
	{
		if (gsys.pPathFinder->AddZone(m_nZoneID, strFile.c_str(), pHeightField) == false)
		{
			mlog("Path File Not Found : zone=%d , filename=%s\n", m_nZoneID, MLocale::ConvUTF16ToAnsi(strFile.c_str()).c_str());
		}
	}

	return true;
}

const GAME_WEATHER_TYPE GFieldInfo::GetRandomWeather() const
{
	if (m_WeatherInfo.vecNodes.empty()) return m_WeatherInfo.nDefault;

	size_t nSize = m_WeatherInfo.vecRandomData.size();
	if (nSize <= 0) return m_WeatherInfo.nDefault;

	int nIndex = RandomNumber(0, nSize - 1);
	return m_WeatherInfo.vecRandomData[nIndex];
}

const MARKER_INFO* GFieldInfo::FindMarker(unsigned long nID) const
{
	MARKER_INFO_MAP_BY_ID::const_iterator itor = m_mapMarkerByID.find(nID);
	if (m_mapMarkerByID.end() == itor) return NULL;

	return &(*itor).second;
}

const MARKER_INFO* GFieldInfo::GetFirstMarker(void) const
{
	if (m_mapMarkerByID.empty())	return NULL;

	MARKER_INFO_MAP_BY_ID::const_iterator it = m_mapMarkerByID.begin();
	return &it->second;
}

void GFieldInfo::InsertMarkerInfo(const MARKER_INFO& markerInfo)
{
	m_mapMarkerByID.insert(MARKER_INFO_MAP_BY_ID::value_type(markerInfo.nID, markerInfo));
}

void GFieldInfo::InsertSpawnInfo( const SPAWN_INFO& info )
{
	m_mapSpawn.insert(SPAWN_INFO_MAP::value_type(info.nID, info));
}


void GFieldInfo::InsertSensor( SENSOR_INFO& info )
{
	if (m_mapSensorByID.find(info.nID) != m_mapSensorByID.end())	
	{
		mlog3("이미 등록된 센서 입니다. (FieldID:%d, SensorID:%d)\n", this->m_nFieldID, info.nID);
		return;
	}

	wchar_t szBuff[MAX_ID_STRING_LENGTH];
	_itow_s(info.nID, szBuff, MAX_ID_STRING_LENGTH, 10);
	info.strName = szBuff;

	m_mapSensorByID.insert(SENSOR_INFO_MAP_BY_ID::value_type(info.nID, info));
}

const SENSOR_INFO* GFieldInfo::FindSensor( int nID ) const
{
	SENSOR_INFO_MAP_BY_ID::const_iterator iter = m_mapSensorByID.find(nID);
	if (iter == m_mapSensorByID.end())	return NULL;

	return &iter->second;
}

void GFieldInfo::ChangeSensorSetting( SENSOR_INFO& source_info)
{
	SENSOR_INFO_MAP_BY_ID::iterator iter = m_mapSensorByID.find(source_info.nID);
	if (iter == m_mapSensorByID.end())	
		return;

	iter->second = source_info;
}

void GFieldInfo::AutoAssign_IsSensorScript()
{
	SENSOR_INFO_MAP_BY_ID::iterator iter = m_mapSensorByID.begin();
	for (iter; iter!=m_mapSensorByID.end(); ++iter)
	{
		SENSOR_INFO& sensor = iter->second;
		if (GetScriptField()->HasSensorScript(m_nFieldID, sensor.nID))
		{
			sensor.bHasScript = true;
		}
	}
}

void GFieldInfo::Reset()
{
	m_WeatherInfo.Clear();	// 날씨 정보
	m_TimeInfo.Clear();		// 시간 정보
	m_mapSpawn.clear();
	m_mapMarkerByID.clear();
	m_mapSensorByID.clear();
	m_mapSpawnPos.clear();
}

bool GFieldInfo::IsLoadingCollision()
{
	return (m_pCollisionTree != NULL);
}

bool GFieldInfo::IsLoadingPath()
{
	if (gsys.pPathFinder)
	{
		if (gsys.pPathFinder->GetMesh(m_nZoneID) != NULL)
			return true;
	}
	return false;
}

void GFieldInfo::Cooking()
{
	for each (const PVP_AREA_INFO& each in m_PvPAreas)
	{
		vector<GFieldAttrGrid::Cell*> vecCells;
		if (!m_pAttrGrid->GetCellsViaRect(each.rcArea, vecCells))
			continue;

		for each (GFieldAttrGrid::Cell* pCell in vecCells)
		{
			pCell->bPvPEnable = true;
			pCell->nPvPFaction1 = each.nFaction1;
			pCell->nPvPFaction2 = each.nFaction2;
		}
	}

	if (CONFIG_AUTO_TEST_AI == GConfig::m_strAutoTestType)
	{
		// AI 테스트중에는 AI LOD 끄기
		m_bAILODEnable = false;
	}

	for each (const SPAWN_INFO_MAP::value_type& val in m_mapSpawn)
	{
		 const SPAWN_INFO& spawnInfo = val.second;

		 for each (const SPAWN_NPC_INFO& spawnNPCInfo in spawnInfo.vecNPCInfoList)
		 {
			 m_mapSpawnPos.insert(SPAWNPOS_INFO_MAP::value_type(spawnNPCInfo.nID, spawnInfo.vPoint));

			 if (gmgr.pNPCInfoMgr->IsQuestNPC(spawnNPCInfo.nID))
			 {
				 m_setQuestNPC.insert(spawnNPCInfo.nID);
			 }			 
		 }
	}

	m_nIntroCutsceneID = gmgr.pCutsceneInfoMgr->GetFieldIntroCutscneneID(m_nFieldID);
}

void GFieldInfo::RemoveSpawnTime()
{
	for (SPAWN_INFO_MAP::iterator it = m_mapSpawn.begin(); it != m_mapSpawn.end(); ++it)
	{
		(*it).second.nStartTime = 0;
		(*it).second.nRespawnTime = 0;
	}
}

bool GFieldInfo::IsSectorHeightLimitField() const
{
	return m_fSectorHeight != 0.0f;
}

bool GFieldInfo::IsAllowSectorHeightLimit(float fDifferenceHeight) const
{
	if (IsSectorHeightLimitField())
	{
		// 높이차가 허용높이 안쪽이면 성공
		return abs(fDifferenceHeight) <= abs(m_fSectorHeight);
	}

	return true;
}

float GFieldInfo::MinX() const
{
//	if (m_bBoundCheck)
//		return m_rtBoundary.left;

	return m_fMinX;
}

float GFieldInfo::MinY() const
{
//	if (m_bBoundCheck)
//		return m_rtBoundary.top;

	return m_fMinY;
}

float GFieldInfo::MaxX() const
{
//	if (m_bBoundCheck)
//		return m_rtBoundary.right;

	return m_fMaxX;
}

float GFieldInfo::MaxY() const
{
//	if (m_bBoundCheck)
//		return m_rtBoundary.bottom;

	return m_fMaxY;
}

void GFieldInfo::SetFieldMaxXY_ForTest( float x, float y )
{
	m_fMaxX = x;
	m_fMaxY = y;
}

void GFieldInfo::SetFieldMinXY_ForTest( float x, float y )
{
	m_fMinX = x;
	m_fMinY = y;
}

bool GFieldInfo::IsInWarpSensor(vec3 vPos)
{
	for each (const SENSOR_INFO_MAP_BY_ID::value_type& data in  m_mapSensorByID)
	{
		const SENSOR_INFO& sensorInfo = data.second;

		if (sensorInfo.DefaultWarpInfo.IsInvalid() && sensorInfo.vecWarpInfos.empty()) continue;
		if (!sensorInfo.IsExistAutoGate()) continue;
		if (sensorInfo.fRadius < sensorInfo.vPoint.XYDistanceTo(vPos)) continue;

		return true;
	}

	return false;
}

const MARKER_INFO* GFieldInfo::GetLeaveMarker(int nToFieldID) const
{
	GFieldInfo* pToFieldInfo = gmgr.pFieldInfoMgr->Find(nToFieldID);
	if (NULL == pToFieldInfo) return NULL;

	for each (const SENSOR_INFO_MAP_BY_ID::value_type& data in  m_mapSensorByID)
	{
		const SENSOR_INFO& sensorInfo = data.second;

		if (nToFieldID == sensorInfo.DefaultWarpInfo.nFieldID)
		{
			const MARKER_INFO* pMarkerInfo = pToFieldInfo->FindMarker(sensorInfo.DefaultWarpInfo.nMarkerID);
			return pMarkerInfo;
		}

		for each (const vector<SENSOR_WARP_INFO>::value_type& data in sensorInfo.vecWarpInfos)
		{
			const SENSOR_WARP_INFO& sensorWarpInfo = data;

			if (nToFieldID == sensorWarpInfo.nFieldID)
			{
				const MARKER_INFO* pMarkerInfo = pToFieldInfo->FindMarker(sensorWarpInfo.nMarkerID);
				return pMarkerInfo;
			}
		}
	}

	return NULL;
}

vec3 GFieldInfo::GetSpawnPos( int nNPCID )
{
	SPAWNPOS_INFO_MAP::iterator itor = m_mapSpawnPos.find(nNPCID);
	if (m_mapSpawnPos.end() == itor) return vec3::ZERO;

	return itor->second;
}

const SPAWN_INFO* GFieldInfo::FindSpawnInfo( unsigned long nID ) const
{
	SPAWN_INFO_MAP::const_iterator iter = m_mapSpawn.find(nID);
	if (iter == m_mapSpawn.end())	return NULL;

	return &iter->second;
}
