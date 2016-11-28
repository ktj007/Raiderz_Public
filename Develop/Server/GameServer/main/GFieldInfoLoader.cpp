#include "stdafx.h"
#include "GFieldInfoLoader.h"
#include "GDef.h"
#include "GPathFinder.h"
#include "GCollisionTree.h"
#include <stdlib.h>
#include "GValidateLogger.h"
#include "GGlobal.h"
#include "GScriptManager.h"
#include "GStrings.h"
#include "GScriptGroup_Field.h"
#include "GTestSystem.h"
#include "GNPCInfoMgr.h"
#include "GNPCInfo.h"
#include "GConst.h"

bool GFieldInfoLoader::LoadField( GFieldInfo* pFieldInfo, wstring strFieldPath, wstring strZonePath, CSZoneList& zoneList )
{
	MTimeChecker2 tcAll;
	MTimeChecker2 tcPartial;
	tcAll.Start();
	wstring strFieldFileName = pFieldInfo->MakeFieldFileName(strFieldPath.c_str());

	mlog1("LoadField(%s)...Begin\n", MLocale::ConvUTF16ToAnsi(strFieldFileName.c_str()).c_str());
	
	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(strFieldFileName.c_str()).c_str())) 
	{
		mlog3("Field Loading Failed : (%s)\n", MLocale::ConvUTF16ToAnsi(strFieldFileName.c_str()).c_str());
		return false;
	}


	_VLI(strFieldFileName.c_str());
	_VLE(_T(FIELD_XML_TAG_FIELDINFO));	
	_VLP;


	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FIELD_XML_TAG_MAIET).FirstChildElement().Element();

	_Attribute(&pFieldInfo->m_nZoneID,			pElement, FIELD_XML_ATTR_ZONE);
	_Attribute(&pFieldInfo->m_nParentID,		pElement, FIELD_XML_ATTR_PARNET);
	_Attribute(&pFieldInfo->m_fMinX,			pElement, FIELD_XML_ATTR_MIN_X);
	_Attribute(&pFieldInfo->m_fMinY,			pElement, FIELD_XML_ATTR_MIN_Y);
	_Attribute(&pFieldInfo->m_fMaxX,			pElement, FIELD_XML_ATTR_MAX_X);
	_Attribute(&pFieldInfo->m_fMaxY,			pElement, FIELD_XML_ATTR_MAX_Y);
	_Attribute(&pFieldInfo->m_rtBoundary.left,		pElement, FIELD_XML_ATTR_BOUNDARY_MIN_X);
	_Attribute(&pFieldInfo->m_rtBoundary.top,		pElement, FIELD_XML_ATTR_BOUNDARY_MIN_Y);
	_Attribute(&pFieldInfo->m_rtBoundary.right,		pElement, FIELD_XML_ATTR_BOUNDARY_MAX_X);
	_Attribute(&pFieldInfo->m_rtBoundary.bottom,	pElement, FIELD_XML_ATTR_BOUNDARY_MAX_Y);
	if (pFieldInfo->m_rtBoundary.left != 0 ||
		pFieldInfo->m_rtBoundary.top != 0 ||
		pFieldInfo->m_rtBoundary.right != 0 ||
		pFieldInfo->m_rtBoundary.bottom != 0)
	{
		pFieldInfo->m_bBoundCheck = true;
	}
	
	wstring strValue;
	if (true == _Attribute(strValue, pElement, FIELD_XML_ATTR_FIXED_TIME))
	{
		for (int i = 0; i < MAX_TIME_TYPE; i++)
		{
			if (!_wcsicmp(strValue.c_str(), GStrings::GameTime(GAME_TIME_TYPE(i))))
			{
				pFieldInfo->m_TimeInfo.bFixed = true;
				pFieldInfo->m_TimeInfo.nFixedTime = GAME_TIME_TYPE(i);
				break;
			}
		}
	}


	_Attribute(&pFieldInfo->m_fSectorHeight,	pElement, FIELD_XML_ATTR_SECTOR_HEIGHT);
	
	_Attribute(&pFieldInfo->m_nMaxGatherInstance[GATHER_MINE],		pElement, FIELD_XML_ATTR_MAX_GATHER_MINE);
	_Attribute(&pFieldInfo->m_nMaxGatherInstance[GATHER_HERB],		pElement, FIELD_XML_ATTR_MAX_GATHER_HERB);
	_Attribute(&pFieldInfo->m_nMaxGatherInstance[GATHER_RELIC],		pElement, FIELD_XML_ATTR_MAX_GATHER_RELIC);
	_Attribute(&pFieldInfo->m_nMaxGatherInstance[GATHER_LUMBER],	pElement, FIELD_XML_ATTR_MAX_GATHER_LUMBER);
	_Attribute(&pFieldInfo->m_bPotralTalentUsable, pElement, FIELD_XML_ATTR_PORTALTALENT_USABLE);
	_Attribute(&pFieldInfo->m_nQPVPNPCID1, pElement, FIELD_XML_ATTR_QUESTPVP_NPC1);
	_Attribute(&pFieldInfo->m_nQPVPNPCID2, pElement, FIELD_XML_ATTR_QUESTPVP_NPC2);
	_Attribute(&pFieldInfo->m_nQPVPRebirthMarkerID1, pElement, FIELD_XML_ATTR_QUESTPVP_REBIRTH1);
	_Attribute(&pFieldInfo->m_nQPVPRebirthMarkerID2, pElement, FIELD_XML_ATTR_QUESTPVP_REBIRTH2);

	if (true == _Attribute(strValue, pElement, FIELD_XML_ATTR_TYPE))
	{
		pFieldInfo->m_nType = (FIELD_TYPE) GStrings::StringToFieldType(strValue.c_str());
	}

	tcPartial.Start();
	LoadFieldData(pElement->FirstChildElement(), &xml, pFieldInfo);
	dlog("\tLoadFieldData (elapsed: %.1f seconds)\n", tcPartial.Check() * 0.001f);

	// 존 정보
	tcPartial.Start();
	CSZoneFileInfo* pZoneFileInfo = zoneList.Find(pFieldInfo->m_nZoneID);
	if (pZoneFileInfo)
	{
		pFieldInfo->m_strZoneName = pZoneFileInfo->strName;
		pFieldInfo->m_strZoneFile = pZoneFileInfo->strFile;
	}
	pFieldInfo->Init(strZonePath, pFieldInfo->m_strZoneFile);
	dlog("\tFieldInit(elapsed: %.1f seconds)\n", tcPartial.Check() * 0.001f);

	

	tcPartial.Start();
	GetScriptField()->Load(pFieldInfo);
	dlog("\tFieldScript Loaded(elapsed: %.1f seconds)\n", tcPartial.Check() * 0.001f);

	// 센서의 스크립트 사용 여부
	pFieldInfo->AutoAssign_IsSensorScript();

	tcPartial.Start();
	pFieldInfo->Cooking();
	dlog("\tFieldCooked Loaded(elapsed: %.1f seconds)\n", tcPartial.Check() * 0.001f);

	dlog("LoadField()...Complete (elapsed: %.1f seconds)\n", tcAll.Check() * 0.001f);

	return true;
}

void GFieldInfoLoader::LoadFieldData( MXmlElement* pChild, MXml* pXml, GFieldInfo* pFieldInfo )
{
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		string test = pChild->Value();

		if (!_stricmp(pChild->Value(), FIELD_XML_TAG_SPAWN))
			ParseSpawn(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_MARKER))
			ParseMarker(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_SENSOR))
			ParseSensor(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_ENV))
			ParseEnv(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_PVPAREA))
			ParsePvparea(pChild, pXml, pFieldInfo);
	}
}

void GFieldInfoLoader::ParseSpawn(MXmlElement* pElement, MXml* pXml, GFieldInfo* pFieldinfo)
{
	SPAWN_INFO info;

	_Attribute(&info.nID, pElement, FIELD_XML_ATTR_ID);

	_VLE(_T(FIELD_XML_TAG_SPAWN));
	_VLA(_T(FIELD_XML_ATTR_ID), info.nID);
	_VLP;

	_Attribute(&info.bEnable, pElement, FIELD_XML_ATTR_ENABLED);
	_Attribute(&info.vPoint.x, pElement, FIELD_XML_ATTR_POS_X);
	_Attribute(&info.vPoint.y, pElement, FIELD_XML_ATTR_POS_Y);
	_Attribute(&info.vPoint.z, pElement, FIELD_XML_ATTR_POS_Z);
	_Attribute(&info.vDir.x, pElement, FIELD_XML_ATTR_DIR_X);
	_Attribute(&info.vDir.y, pElement, FIELD_XML_ATTR_DIR_Y);
	_Attribute(&info.vDir.z, pElement, FIELD_XML_ATTR_DIR_Z);
	_Attribute(&info.fRadius, pElement, FIELD_XML_ATTR_POS_RADIUS);
	_Attribute(&info.nRespawnTime, pElement, FIELD_XML_ATTR_RESPAWN_TIME);
	_Attribute(&info.nStartTime, pElement, FIELD_XML_ATTR_START_TIME);
	_Attribute(&info.bRandomDir, pElement, FIELD_XML_ATTR_RANDOM_DIR);
	_Attribute(&info.fRoamDepth, pElement, FIELD_XML_ATTR_ROAM_DEPTH);
	_Attribute(&info.fRoamRadius, pElement, FIELD_XML_ATTR_ROAM_RADIUS);
	_Attribute(&info.nGroupID, pElement, FIELD_XML_ATTR_GROUP);
	_Attribute(&info.fCombatRadius, pElement, FIELD_XML_ATTR_COMBAT_RADIUS);
	
	// Time Condition
	for (int j=1; j<=4; j++)
	{
		char buff[64];
		_itoa(j, buff, 10);
		string strAttrName = FIELD_XML_ATTR_TIME;
		strAttrName += buff;

		string strDurationTime;
		_Attribute(strDurationTime, pElement, strAttrName.c_str());
		for (int i=0; i<MAX_TIME_TYPE; i++)
		{
			if (!strcmp(strDurationTime.c_str(), MLocale::ConvUTF16ToAnsi(GAME_TIME_TYPE_NAME[i]).c_str()))
			{
				info.bUseConditionTime = true;
				info.bEnableTimeList[i] = true;
				break;
			}
		}
	}

	// Time-detail Condition
	_Attribute(&info.pairEnableTime.first, pElement, FIELD_XML_ATTR_TIME_START, -1);
	_Attribute(&info.pairEnableTime.second, pElement, FIELD_XML_ATTR_TIME_END, -1);

	// Weather Condition
	for (int j=1; j<=4; j++)
	{
		char buff[64];
		_itoa(j, buff, 10);
		string strAttrName = FIELD_XML_ATTR_WEATHER;
		strAttrName += buff;

		string strDurationWeather;
		_Attribute(strDurationWeather, pElement, strAttrName.c_str());
		for (int i=0; i<MAX_WEATHER_TYPE; i++)
		{
			if (!strcmp(strDurationWeather.c_str(), MLocale::ConvUTF16ToAnsi(GAME_WEATHER_TYPE_NAME[i]).c_str()))
			{
				info.bUseConditionWeather = true;
				info.bEnableWeatherList[i] = true;
				break;
			}
		}
	}


	bool bHasDefault = false;

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), FIELD_XML_TAG_NPC))
			ParseNPC(pChild, pXml, &info, bHasDefault);
	}

	// 디폴트 값 설정
	{
#define MIN_RESPAWN_TIME 1

		if (info.nRespawnTime < MIN_RESPAWN_TIME)
			info.nRespawnTime = MIN_RESPAWN_TIME;
	}

	// 디폴트 NPC 타입 값 정의
	int nTotalPickValue = MAX_SPAWN_RATE;
	for (size_t i=0; i<info.vecNPCInfoList.size(); i++)
	{
		SPAWN_NPC_INFO& npc_info = info.vecNPCInfoList[i];

		if (npc_info.bDefaultSet)
			continue;

		if ((long long)(nTotalPickValue - npc_info.nPickRate) > 0)
			nTotalPickValue -= npc_info.nPickRate;
		else
			nTotalPickValue = 0;
	}

	for (size_t i=0; i<info.vecNPCInfoList.size(); i++)
	{
		SPAWN_NPC_INFO& npc_info = info.vecNPCInfoList[i];

		if (npc_info.bDefaultSet)
		{
			npc_info.nPickRate = nTotalPickValue;
			nTotalPickValue = 0;
			break;
		}
	}

	// rate 합이 1000이 안되고, 디폴트 NPC도 없지만, NPC가 1마리만 있다면 그놈이 디폴트
	if (0 < nTotalPickValue && info.vecNPCInfoList.size() == 1)
	{
		SPAWN_NPC_INFO& npc_info = info.vecNPCInfoList[0];
		npc_info.nPickRate += nTotalPickValue;
		nTotalPickValue = 0;
	}

	// 오차보정: +1정도 남으면 첫번째 NPC에 할당
	if (nTotalPickValue == 1 && 0 < info.vecNPCInfoList.size())
	{
		SPAWN_NPC_INFO& npc_info = info.vecNPCInfoList[0];
		npc_info.nPickRate += nTotalPickValue;
		nTotalPickValue = 0;
	}

	// npc가 채집용인지 체크
	if (!info.vecNPCInfoList.empty())
	{
		SPAWN_NPC_INFO& spawnNPCInfo = info.vecNPCInfoList[0];
		GNPCInfo* pNPCInfo = gmgr.pNPCInfoMgr->Find(spawnNPCInfo.nID);
		if (pNPCInfo)
		{
			if (pNPCInfo->nNpcType == NPC_TYPE_GATHER)
			{
				info.nGatherType = pNPCInfo->nGatherType;
				// 채집 엔티티는 한곳에 몰려서 생성되는 것을 방지하기 위해 StartTime을
				// 랜덤으로 정합니다. 
				info.nStartTime = GMath::RandomNumber(0, GConst::GATHER_RESPAWN_TIME[info.nGatherType]);	
				info.nRespawnTime = GConst::GATHER_RESPAWN_TIME[info.nGatherType];
			}
		}
	}

	if (pFieldinfo->m_bValidation)
	{
		// rate합이 1000 미만이면서, 디폴트가 없으면 오류.
		_VLOGGER->ValidateSumTotal(MAX_SPAWN_RATE - nTotalPickValue, MAX_SPAWN_RATE, _T(FIELD_XML_TAG_SPAWN), _T(FIELD_XML_ATTR_RATE));
	}


	char szBuff[MAX_ID_STRING_LENGTH];
	_itoa_s(info.nID, szBuff, MAX_ID_STRING_LENGTH, 10);
	info.strName = szBuff;

	//assert(pFieldinfo->m_mapSpawn.find(info.nID) == pFieldinfo->m_mapSpawn.end());
	if (pFieldinfo->m_mapSpawn.find(info.nID) != pFieldinfo->m_mapSpawn.end())
	{
		mlog3("중복된 NPC 스폰ID입니다. (Field: %d, SpawnID: %d)\n", pFieldinfo->m_nFieldID, info.nID);
	}


	pFieldinfo->InsertSpawnInfo(info);
}

void GFieldInfoLoader::ParseMarker( MXmlElement* pElement, MXml* pXml, GFieldInfo* pFieldInfo )
{
	MARKER_INFO info;

	_Attribute(&info.nID, pElement, FIELD_XML_ATTR_ID);
	_Attribute(info.strName, pElement, FIELD_XML_ATTR_NAME);
	_Attribute(&info.vPoint.x, pElement, FIELD_XML_ATTR_POS_X);
	_Attribute(&info.vPoint.y, pElement, FIELD_XML_ATTR_POS_Y);
	_Attribute(&info.vPoint.z, pElement, FIELD_XML_ATTR_POS_Z);
	_Attribute(&info.vDir.x, pElement, FIELD_XML_ATTR_DIR_X);
	_Attribute(&info.vDir.y, pElement, FIELD_XML_ATTR_DIR_Y);
	_Attribute(&info.vDir.z, pElement, FIELD_XML_ATTR_DIR_Z);
	_Attribute(&info.bRandomDir, pElement, FIELD_XML_ATTR_RANDOM_DIR);
	info.nFieldID = pFieldInfo->m_nFieldID;

	char szBuff[MAX_ID_STRING_LENGTH];
	_itoa_s(info.nID, szBuff, MAX_ID_STRING_LENGTH, 10);
	info.strName = szBuff;

	//assert(pMapInfo->m_mapMarkerByID.find(info.nID) == pMapInfo->m_mapMarkerByID.end());
	if (pFieldInfo->m_mapMarkerByID.find(info.nID) != pFieldInfo->m_mapMarkerByID.end())
	{
		mlog3("중복된 NPC 마커ID입니다. (Field: %d, MarkerID: %d)\n", pFieldInfo->m_nFieldID, info.nID);
	}
	pFieldInfo->m_mapMarkerByID.insert(MARKER_INFO_MAP_BY_ID::value_type(info.nID, info));

}

void GFieldInfoLoader::ParseSensor( MXmlElement* pElement, MXml* pXml, GFieldInfo* pFieldInfo )
{
	VALID(pFieldInfo);

	SENSOR_INFO info;

	_Attribute(&info.nID, pElement, FIELD_XML_ATTR_ID);
	
	_VLE(_T(FIELD_XML_TAG_SENSOR));
	_VLA(_T(FIELD_XML_ATTR_ID), info.nID);
	_VLP;

	_Attribute(info.strName, pElement, FIELD_XML_ATTR_NAME);
	_Attribute(&info.vPoint.x, pElement, FIELD_XML_ATTR_POS_X);
	_Attribute(&info.vPoint.y, pElement, FIELD_XML_ATTR_POS_Y);
	_Attribute(&info.vPoint.z, pElement, FIELD_XML_ATTR_POS_Z);
	_Attribute(&info.bStartingEnable, pElement, FIELD_XML_ATTR_ENABLED);
	_Attribute(&info.nExitMarkerID, pElement, FIELD_XML_ATTR_EXIT);
	_Attribute(&info.nCheckpointMarkerID, pElement, FIELD_XML_ATTR_CHECKPOINT);
	_Attribute(&info.fRadius, pElement, FIELD_XML_ATTR_POS_RADIUS);

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), FIELD_XML_TAG_WARP))
		{
			SENSOR_WARP_INFO infoWarp;

			string strWarpType;
			if (_Attribute(strWarpType,			pChild, FIELD_XML_ATTR_WARP_TYPE))
			{
				infoWarp.eType = static_cast<WARP_TYPE>(GStrings::StringToWarpType(MLocale::ConvAnsiToUCS2(strWarpType.c_str()).c_str()));
			}			

			string strTitle;
			_Attribute(strTitle,		pChild, FIELD_XML_ATTR_TITLE, pXml);
			wcsncpy_s(infoWarp.szTitle, MLocale::ConvAnsiToUCS2(strTitle.c_str()).c_str(), 80);

			_Attribute(infoWarp.nConditionID,	pChild, FIELD_XML_ATTR_CONDITION);
			_Attribute(infoWarp.nFieldID,		pChild, FIELD_XML_ATTR_FIELD);
			_Attribute(infoWarp.nFieldGroupID,	pChild, FIELD_XML_ATTR_FIELD_GROUP);
			_Attribute(infoWarp.nMarkerID,		pChild, FIELD_XML_ATTR_MARKER);
			_Attribute(infoWarp.nQuestID,		pChild, FIELD_XML_ATTR_QUEST);			
			
			string strValue;
			_Attribute(strValue,	pChild, FIELD_XML_ATTR_GATE_TYPE, MLocale::ConvUTF16ToAnsi(GStrings::GateTypeToString(0)).c_str());
			infoWarp.eGateType = static_cast<GATE_TYPE>(GStrings::StringToGateType(MLocale::ConvAnsiToUCS2(strValue.c_str()).c_str()));

			if (infoWarp.nConditionID == INVALID_ID)
			{
				if (info.DefaultWarpInfo.IsInvalid())
				{
					info.DefaultWarpInfo = infoWarp;
				}
				else
				{
					//assert(false && L"센서 기본 워프정보를 두번이상 등록하려고 했습니다.");
					mlog3("필드 워프: 센서 기본 워프정보를 두번이상 등록하려고 했습니다. (Field: %d, TargetField: %d, TargetFieldGroupID: %d)\n", 
						pFieldInfo->m_nFieldID, infoWarp.nFieldID, infoWarp.nFieldGroupID);
				}
			}
			else
			{
				info.vecWarpInfos.push_back(infoWarp);
			}
		}
	}

	pFieldInfo->InsertSensor(info);

}

void GFieldInfoLoader::ParseEnv( MXmlElement* pElement, MXml* pXml, GFieldInfo* pFieldInfo )
{
	string strValue;

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), FIELD_XML_TAG_WEATHER))
		{
			_Attribute(strValue, pChild, FIELD_XML_ATTR_DEFAULT);
			for (int i = 0; i < MAX_WEATHER_TYPE; i++)
			{
				if (!_wcsicmp(MLocale::ConvAnsiToUCS2(strValue.c_str()).c_str(), GStrings::WeatherSimple(GAME_WEATHER_TYPE(i))))
				{
					pFieldInfo->m_WeatherInfo.nDefault = GAME_WEATHER_TYPE(i);
					break;
				}
			}

			MXmlElement* pAddElement = pChild->FirstChildElement();
			for (pAddElement; pAddElement != NULL; pAddElement=pAddElement->NextSiblingElement() )
			{
				if (!_stricmp(pAddElement->Value(), FIELD_XML_TAG_ADD))
				{
					GFieldWeatherInfoNode node;

					_Attribute(strValue, pAddElement, FIELD_XML_ATTR_WEATHER);

					for (int i = 0; i < MAX_WEATHER_TYPE; i++)
					{
						if (!_wcsicmp(MLocale::ConvAnsiToUCS2(strValue.c_str()).c_str(), GStrings::WeatherSimple(GAME_WEATHER_TYPE(i))))
						{
							node.nWeather = GAME_WEATHER_TYPE(i);
							break;
						}
					}

					_Attribute(&node.nPercent, pAddElement, FIELD_XML_ATTR_PERCENT);
					node.nPercent = min(node.nPercent, 100);

					if ((node.nWeather != DEFAULT_GAME_WEATHER) && (node.nPercent > 0))
					{
						pFieldInfo->m_WeatherInfo.vecNodes.push_back(node);
					}
				}
			}

		}
	}

}

void GFieldInfoLoader::ParsePvparea( MXmlElement* pElement, MXml* pXml, GFieldInfo* pFieldInfo )
{
	PVP_AREA_INFO infoPVP;
	_Attribute(&infoPVP.rcArea.left, pElement, FIELD_XML_ATTR_LEFT);
	_Attribute(&infoPVP.rcArea.top, pElement, FIELD_XML_ATTR_TOP);
	_Attribute(&infoPVP.rcArea.right, pElement, FIELD_XML_ATTR_RIGHT);
	_Attribute(&infoPVP.rcArea.bottom, pElement, FIELD_XML_ATTR_BOTTOM);
	_Attribute(&infoPVP.nFaction1, pElement, FIELD_XML_ATTR_FACTION1);
	_Attribute(&infoPVP.nFaction2, pElement, FIELD_XML_ATTR_FACTION2);
	
	if (pFieldInfo->m_bValidation)
	{
		_VLOGGER->ValidateBiggerOrEqualThan((float)infoPVP.rcArea.left, (float)infoPVP.rcArea.right, _T(FIELD_XML_ATTR_LEFT), _T(FIELD_XML_ATTR_RIGHT));
		_VLOGGER->ValidateBiggerOrEqualThan((float)infoPVP.rcArea.top, (float)infoPVP.rcArea.bottom, _T(FIELD_XML_ATTR_TOP), _T(FIELD_XML_ATTR_BOTTOM));
	}

	pFieldInfo->m_PvPAreas.push_back(infoPVP);
}

void GFieldInfoLoader::ParseNPC( MXmlElement* pElement, MXml* pXml, SPAWN_INFO* pSpwanInfo, bool& bHasDefault )
{
	SPAWN_NPC_INFO info;

	_Attribute(&info.bDefaultSet, pElement, FIELD_XML_ATTR_DEFAULT);
	_Attribute(&info.nID, pElement, FIELD_XML_ATTR_ID);
	_Attribute(&info.nPickRate, pElement, FIELD_XML_ATTR_RATE);

	if (info.bDefaultSet == true)
	{
		// 이미 디폴트가 있는 경우
		if (bHasDefault == true)
		{
			assert(0 && L"already declare default spawn npc set.");
			return;
		}

		bHasDefault = true;
	}

	pSpwanInfo->vecNPCInfoList.push_back(info);

}

bool GFieldInfoLoader::LoadFieldGroupInfo(GFieldInfoMgr::FIELD_GROUP_MAP* pFieldGroupMap, const wchar_t* szFileName )
{
	dlog("LoadFieldGroupInfo(%s)...Begin\n", MLocale::ConvUTF16ToAnsi(szFileName).c_str());

	_VLI(szFileName);

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) 
	{
		_ASSERT(0);
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FIELD_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), FIELD_XML_TAG_FIELDLIST))
		{
			MXmlElement* pFieldElement = pElement->FirstChildElement(FIELD_XML_TAG_FIELD);
			for( pFieldElement; pFieldElement != NULL; pFieldElement=pFieldElement->NextSiblingElement() )
			{
				if (!_stricmp(pFieldElement->Value(), FIELD_XML_TAG_FIELDGROUP))
				{			
					int nFieldGroupID;
					_Attribute(&nFieldGroupID,				pFieldElement, FIELD_XML_ATTR_ID);


					_VLE(_T(FIELD_XML_TAG_FIELDGROUP));
					_VLA(_T(FIELD_XML_ATTR_ID), nFieldGroupID);
					_VLP;


					bool bValidation;
					_Attribute(&bValidation,				pFieldElement, FIELD_XML_ATTR_VALIDATION);


					int nStartFieldID;
					_Attribute(&nStartFieldID,		pFieldElement, FIELD_XML_ATTR_START_FIELD);
					GFieldInfo* pStartFieldInfo = gmgr.pFieldInfoMgr->Find(nStartFieldID);
					const MARKER_INFO* pMarkerInfo = NULL;
					//if (NULL == pStartFieldInfo)
					//{
					//	if (bValidation)
					//	{
					//		_VLOGGER->Log(FIELD_XML_ATTR_START_FIELD IS_NOT_IN_FIELDLIST);
					//	}
					//	continue;
					//}

					// start field must be dynmicfield
					if (pStartFieldInfo)
					{
						if (!pStartFieldInfo->m_bDynamic)
						{
							if (bValidation)
							{
								_VLOGGER->ValidateTrue(pStartFieldInfo->m_bDynamic, _T(FIELD_XML_ATTR_START_FIELD));
							}
							continue;
						}

						DWORD dwStartMarkerID;
						_Attribute(&dwStartMarkerID,	pFieldElement, FIELD_XML_ATTR_START_MARKER);
						pMarkerInfo = pStartFieldInfo->FindMarker(dwStartMarkerID);
						_ASSERT(NULL != pMarkerInfo);
						if (NULL == pMarkerInfo)
						{
							if (bValidation)
							{
								_VLOGGER->Log(_T(FIELD_XML_ATTR_START_MARKER) IS_NOT_IN_PERCENT_D_FILEEXT_FIELD, nStartFieldID);
							}
							continue;
						}
					}
					
					FIELD_GROUP_INFO* info = new FIELD_GROUP_INFO;
					info->nID = nFieldGroupID;
					info->pStartFieldInfo = pStartFieldInfo;
					info->pStartMarkerInfo = pMarkerInfo;
					info->bValidation = bValidation;

					MXmlElement* pChild = pFieldElement->FirstChildElement();
					for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
					{
						if (!_stricmp(pChild->Value(), FIELD_XML_TAG_FIELD))
						{
							int nFieldID;
							_Attribute(&nFieldID,				pChild, FIELD_XML_ATTR_ID);

							_VLE(_T(FIELD_XML_TAG_FIELD));
							_VLA(_T(FIELD_XML_ATTR_ID), nFieldID);
							_VLP;

							GFieldInfo* pFieldInfo = gmgr.pFieldInfoMgr->Find(nFieldID);
							_ASSERT(NULL != pFieldInfo);
							if (NULL == pFieldInfo)
							{
								if (bValidation)
								{
									_VLOGGER->Log(_T(FIELD_XML_ATTR_START_FIELD) IS_NOT_IN_FIELDLIST);
								}
								continue;
							}

							pFieldInfo->m_pFieldGroup = info;
							info->vecFieldInfo.push_back(pFieldInfo);
						}
					}

					InsertGroup(pFieldGroupMap, info);
				}
			}
		}
	}

	dlog("LoadFieldGroupInfo()...Complete\n");

	return true;
}

void GFieldInfoLoader::InsertGroup( GFieldInfoMgr::FIELD_GROUP_MAP* pFieldGroupMap, FIELD_GROUP_INFO* pInfo )
{
	if (NULL == pInfo) return;

	pFieldGroupMap->insert(GFieldInfoMgr::FIELD_GROUP_MAP::value_type(pInfo->nID, pInfo));
}

bool GFieldInfoLoader::ReloadField( GFieldInfo* pFieldInfo, wstring strFieldPath )
{
	pFieldInfo->Reset();

	wstring strFieldName = pFieldInfo->MakeFieldFileName(strFieldPath.c_str());

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(strFieldName.c_str()).c_str())) 
	{
		mlog3("Field Reloading Failed : (%s)\n", MLocale::ConvUTF16ToAnsi(strFieldName.c_str()).c_str());
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FIELD_XML_TAG_MAIET).FirstChildElement().Element();

	LoadFieldData(pElement->FirstChildElement(), &xml, pFieldInfo);
	GetScriptField()->Load(pFieldInfo);

	return true;
}