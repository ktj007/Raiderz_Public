#include "StdAfx.h"
#include "GFieldInfoValidator.h"
#include "GServerSystems.h"
#include "GDef.h"
#include "GValidateLogger.h"
#include "GFieldInfoMgr.h"
#include "GGlobal.h"
#include "GValidateLogger.h"
#include "GNPCInfoMgr.h"
#include "GFieldInfoLoader.h"
#include "GAIRoomMgr.h"
#include "GSoulBindingInfoMgr.h"

bool GFieldInfoValidator::Check()
{
	if (gmgr.pLootInfoMgr == NULL) return true;
	_VLI(PURE_FILENAME_FIELDLIST);

	for (GFieldInfoMgr::iterator itor = gmgr.pFieldInfoMgr->begin(); 
		itor != gmgr.pFieldInfoMgr->end(); itor++)
	{
		GFieldInfo* pFieldInfo = (*itor).second;

		if (!pFieldInfo->m_bValidation)
		{
			continue;
		}

		_VLE(L"FIELD");
		_VLA(L"id", pFieldInfo->m_nFieldID);
		_VLA(L"name", pFieldInfo->m_strFieldName.c_str());
		_VLP;

		_VLOGGER->ValidateID(pFieldInfo->m_nFieldID);

		CheckLoadCollAndPath(pFieldInfo);
		CheckSpawn(pFieldInfo);
		CheckFieldSize(pFieldInfo);		
		CheckSensor(pFieldInfo);
	}

	return (_VLOGGER->GetValidateResult() && GetReturnValue());
}

void GFieldInfoValidator::CheckLoadCollAndPath( GFieldInfo* pFieldInfo )
{
	if (pFieldInfo->IsLoadingPath() == false)
	{
		_VLOGGER->Log(L"Path 파일을 로딩하지 못했습니다.\n");
	}
	if (pFieldInfo->IsLoadingCollision() == false)
	{
		_VLOGGER->Log(L"Collision 파일을 로딩하지 못했습니다.\n");
	}
}

void GFieldInfoValidator::CheckSpawn( GFieldInfo* pFieldInfo )
{
	for each (const SPAWN_INFO_MAP::value_type& data in pFieldInfo->GetSpawnInfo())
	{
		const SPAWN_INFO& spawn_info = data.second;

		for each (const SPAWN_NPC_INFO& npc_info in spawn_info.vecNPCInfoList)
		{
			if (!gmgr.pNPCInfoMgr->Find(npc_info.nID))
			{
				_VLOGGER->Log(L"Spawn.NPCID 정보를 찾을 수 없습니다. (SpawnID: %d, NPCID: %d)\n", spawn_info.nID, npc_info.nID);
			}
		}

		if (spawn_info.nGroupID != INVALID_SPAWN_GROUP_ID &&
			!gmgr.pAIRoomMgr->HasRoom(spawn_info.nGroupID))
		{
			_VLOGGER->Log(L"Spawn.GroupID 에 적당한 Room 정보를 찾을 수 없습니다. (SpawnID: %d, GroupID: %d)\n", spawn_info.nID, spawn_info.nGroupID);
		}
	}
}

void GFieldInfoValidator::CheckFieldSize( GFieldInfo* pFieldInfo )
{
	if (pFieldInfo->m_bBoundCheck)
	{
		_VLOGGER->ValidateBiggerOrEqualThan(pFieldInfo->m_rtBoundary.left, pFieldInfo->m_rtBoundary.right, _T(FIELD_XML_ATTR_BOUNDARY_MIN_X), _T(FIELD_XML_ATTR_BOUNDARY_MAX_X));
		_VLOGGER->ValidateBiggerOrEqualThan(pFieldInfo->m_rtBoundary.top, pFieldInfo->m_rtBoundary.bottom, _T(FIELD_XML_ATTR_BOUNDARY_MAX_X), _T(FIELD_XML_ATTR_BOUNDARY_MAX_Y));
	}
	else
	{
		_VLOGGER->ValidateBiggerOrEqualThan(pFieldInfo->m_fMinX, pFieldInfo->m_fMaxX, _T(FIELD_XML_ATTR_MIN_X), _T(FIELD_XML_ATTR_MAX_X));
		_VLOGGER->ValidateBiggerOrEqualThan(pFieldInfo->m_fMinY, pFieldInfo->m_fMaxY, _T(FIELD_XML_ATTR_MIN_Y), _T(FIELD_XML_ATTR_MAX_Y));
	}
}

void GFieldInfoValidator::CheckSensor(GFieldInfo* pFieldInfo)
{
	for each(const SENSOR_INFO_MAP_BY_ID::value_type& refSensor in pFieldInfo->m_mapSensorByID)
	{		
		_VLE(L"SENSOR");
		_VLA(L"id", refSensor.second.nID);
		_VLA(L"name", refSensor.second.strName.c_str());
		_VLP;

		if (refSensor.second.DefaultWarpInfo.IsInvalid() && refSensor.second.vecWarpInfos.empty())	continue;
		
		
		// 다이나믹필드 워프센서		
		if (refSensor.second.DefaultWarpInfo.nFieldGroupID != INVALID_ID && refSensor.second.nExitMarkerID == INVALID_ID)
		{
			_VLOGGER->Log(L"exit값이 없습니다. 다이나믹필드로 워프하는 센서는 ExitMarker가 있어야합니다.\n");
		}
		else
		{
			bool bExistInnRoomWarp = false;
			for each(const SENSOR_WARP_INFO& warpInfo in refSensor.second.vecWarpInfos)
			{
				if (WT_INNROOM == warpInfo.eType)
				{
					bExistInnRoomWarp = true;
				}
				if (warpInfo.nFieldGroupID != INVALID_ID && refSensor.second.nExitMarkerID == INVALID_ID)
				{
					_VLOGGER->Log(L"exit값이 없습니다. 다이나믹필드로 워프하는 센서는 ExitMarker가 있어야합니다.\n");
					break;
				}
			}

			if (bExistInnRoomWarp && 1 != refSensor.second.vecWarpInfos.size())
			{
				_VLOGGER->Log(L"여관방 워프가 기술되어 있을때, 다른 워프는 기술할 수 없습니다.\n");
			}
		}


		// 자신의 퇴장마커가 센서 내부에 있으면 안됨
		for each(const pair<unsigned long, MARKER_INFO>& refMarker in pFieldInfo->m_mapMarkerByID)
		{			
			if (refSensor.second.nExitMarkerID != refMarker.second.nID) continue;

			vec3 vPos = refMarker.second.vPoint;
			if (refSensor.second.fRadius < refSensor.second.vPoint.XYDistanceTo(vPos)) continue;			

			_VLOGGER->Log(L"ExitMaker가 센서 내부에 있습니다. ExitMarker는 센서 외부에 있어야합니다.\n");
		}
	}	
}

void GFieldInfoValidator::CheckSoulBinding( GFieldInfo* pFieldInfo )
{
	if (!pFieldInfo->m_bDynamic) return;

	if (false == gmgr.pSoulBindingInfoMgr->IsSoulBindingField(pFieldInfo->m_nFieldID))
	{
		_VLOGGER->Log(L"공용 필드에 최소한 하나 이상의 영혼석 위치가 있어야 합니다.\n");
	}
}
