#include "stdafx.h"
#include "XFieldInfo.h"
#include "CSFieldInfoHelper.h"
#include "CSDef.h"
#include "XSoundField.h"
#include "XStrings.h"

#include "RSceneManager.h"
#include "RMeshMgr.h"
#include "RMesh.h"

#include "XSensorInfo.h"

// Tag
const char* FIELD_XML_TAG_MAIET			= "maiet";
const char* FIELD_XML_TAG_FIELDINFO		= "FIELDINFO";
const char* MAPINFO_XML_TAG_MARKER		= "MARKER";
const char* MAPINFO_XML_TAG_SENSOR		= "SENSOR";
const char* FIELD_XML_TAG_ENV			= "Env";
const char* FIELD_XML_TAG_SKY			= "Sky";
const char* FIELD_XML_TAG_SOUND			= "SOUND";
const char* FIELD_XML_TAG_PVPAREA		= "PVPAREA";
const char* FIELD_XML_TAG_AREA			= "AREA";
const char* FIELD_XML_TAG_WARP			= "WARP";
const char* FIELD_XML_TAG_EFFECT		= "EFFECT";
const char* FIELD_XML_TAG_BGM			= "BGM";
const char* FIELD_XML_TAG_FLOORS		= "FLOORS";
const char* FIELD_XML_TAG_FLOOR			= "FLOOR";
const char* FIELD_XML_TAG_SOUNDREVERB	= "SOUNDREVERB";
const char* FIELD_XML_TAG_PRELOAD		= "PRELOAD";

// Attribute
const char* FIELD_XML_ATTR_ID			= "id";
const char* FIELD_XML_ATTR_NAME			= "name";
const char* FIELD_XML_ATTR_ZONE			= "zone";
const char* FIELD_XML_ATTR_INSTANCE		= "instance";
const char* FIELD_XML_ATTR_WEATHER_EFFECT = "weather_effect";
const char* FIELD_XML_ATTR_BGM			= "bgm";
const char* FIELD_XML_ATTR_HEIGHT		= "height";
const char* FIELD_XML_ATTR_REVERB		= "reverb";
const char* FIELD_XML_ATTR_PATH			= "path";

const char* FIELD_XML_ATTR_LOCAL		= "local";
const char* FIELD_XML_ATTR_POS_X		= "x";
const char* FIELD_XML_ATTR_POS_Y		= "y";
const char* FIELD_XML_ATTR_POS_Z		= "z";
const char* FIELD_XML_ATTR_DIR_X		= "dir_x";
const char* FIELD_XML_ATTR_DIR_Y		= "dir_y";
const char* FIELD_XML_ATTR_DIR_Z		= "dir_z";
const char* FIELD_XML_ATTR_UP_X			= "up_x";
const char* FIELD_XML_ATTR_UP_Y			= "up_y";
const char* FIELD_XML_ATTR_UP_Z			= "up_z";
const char* FIELD_XML_ATTR_SCALE_X		= "scale_x";
const char* FIELD_XML_ATTR_SCALE_Y		= "scale_y";
const char* FIELD_XML_ATTR_SCALE_Z		= "scale_z";
const char* FIELD_XML_ATTR_MIN_X		= "min_x";
const char* FIELD_XML_ATTR_MIN_Y		= "min_y";
const char* FIELD_XML_ATTR_MIN_Z		= "min_z";
const char* FIELD_XML_ATTR_MAX_X		= "max_x";
const char* FIELD_XML_ATTR_MAX_Y		= "max_y";
const char* FIELD_XML_ATTR_MAX_Z		= "max_z";
const char* FIELD_XML_ATTR_RANDOM_DIR	= "random_dir";
const char*	FIELD_XML_ATTR_BOUNDARY_MIN_X = "boundary_min_x";
const char*	FIELD_XML_ATTR_BOUNDARY_MIN_Y = "boundary_min_y";
const char*	FIELD_XML_ATTR_BOUNDARY_MAX_X = "boundary_max_x";
const char*	FIELD_XML_ATTR_BOUNDARY_MAX_Y = "boundary_max_y";
const char*	FIELD_XML_ATTR_FIXED_TIME = "fixed_time";
const char*	FIELD_XML_ATTR_TUTORIAL		 = "tutorial";

const char*	FIELD_XML_ATTR_PORTALTALENT_USABLE = "returnable";


const char* FIELD_XML_ATTR_ENABLED		= "enabled";
const char* FIELD_XML_ATTR_NEAR			= "near";
const char* FIELD_XML_ATTR_FAR			= "far";
const char* FIELD_XML_ATTR_COLOR_A		= "color_a";
const char* FIELD_XML_ATTR_COLOR_R		= "color_r";
const char* FIELD_XML_ATTR_COLOR_G		= "color_g";
const char* FIELD_XML_ATTR_COLOR_B		= "color_b";
const char* FIELD_XML_ATTR_MODEL		= "model";
const char* FIELD_XML_ATTR_DEFAULT		= "default";
const char* FIELD_XML_ATTR_TYPE			= "type";
const char* FIELD_XML_ATTR_POS_RADIUS	= "radius";
const char* FIELD_XML_ATTR_EFFECT		= "effect";

const char* FIELD_XML_ATTR_LEFT			= "left";
const char* FIELD_XML_ATTR_TOP			= "top";
const char* FIELD_XML_ATTR_RIGHT		= "right";
const char* FIELD_XML_ATTR_BOTTOM		= "bottom";
const char* FIELD_XML_ATTR_FACTION1		= "faction1";
const char* FIELD_XML_ATTR_FACTION2		= "faction2";
const char* FIELD_XML_ATTR_FACTION_NAME1	= "faction_name1";
const char* FIELD_XML_ATTR_FACTION_NAME2	= "faction_name2";

const char* FIELD_XML_ATTR_FIELD			= "field";
const char* FIELD_XML_ATTR_MARKER			= "marker";
const char* FIELD_XML_ATTR_TITLE			= "title";
const char* FIELD_XML_ATTR_FIELD_GROUP		= "field_group";
const char* FIELD_XML_ATTR_QUEST			= "quest";
const char* FIELD_XML_ATTR_CONDITION		= "condition";
const char* FIELD_XML_ATTR_GATE_TYPE		= "gate_type";
const char* FIELD_XML_ATTR_WARP_TYPE		= "warp_type";

static const char* GATE_TYPE_STR[] = 
{
	"auto",
	"manual"
};

const char* BGM_XML_TAG[] =
{
	"fieldbgm",

	"dawnbgm",
	"daytimebgm",
	"sunsetbgm",
	"nightbgm",

	"clodybgm",
	"rainbgm",
	"heavyrainbgm",
	"heavysnowbgm",

	"dungeonbgm",
	"normalbgm"
};



XFieldInfoMgr::XFieldInfoMgr(void)
{

}

XFieldInfoMgr::~XFieldInfoMgr(void)
{
	Clear();
}

void XFieldInfoMgr::Clear()
{
	for (std::map< int, XFieldInfo*>::iterator itr = m_mapSimpleFieldInfo.begin();
		itr != m_mapSimpleFieldInfo.end(); ++itr)
	{
		SAFE_DELETE(itr->second );
	}
	m_mapSimpleFieldInfo.clear();

	m_FieldList.Clear();
	m_ZoneList.Clear();

	_ASSERT(m_PreLoadResources.empty());
}

bool XFieldInfoMgr::Load(const wchar_t* szZoneList, const wchar_t* szFieldList)
{
	if (m_ZoneList.Load(szZoneList) == false) return false;

	bool bIncludeDevFieldList = true;
	#ifdef _PUBLISH
	bIncludeDevFieldList = false;
	#endif	

	if (m_FieldList.Load(szFieldList, bIncludeDevFieldList) == false) return false;
	return true;
}

void XFieldInfoMgr::ParseFieldInfo(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo)
{
	wstring strCollision;
	wstring strPath;

	_Attribute(&pFieldInfo->m_nZoneID,		pElement, FIELD_XML_ATTR_ZONE);
	//_Attribute(pFieldInfo->m_strName,		pElement, FIELD_XML_ATTR_NAME,		pXml);
	_Attribute(&pFieldInfo->m_bInstance,	pElement, FIELD_XML_ATTR_INSTANCE);
	_Attribute(&pFieldInfo->m_bWeatherEffect,	pElement, FIELD_XML_ATTR_WEATHER_EFFECT);
	_Attribute(&pFieldInfo->m_fMinX,		pElement, FIELD_XML_ATTR_MIN_X);
	_Attribute(&pFieldInfo->m_fMinY,		pElement, FIELD_XML_ATTR_MIN_Y);
	_Attribute(&pFieldInfo->m_fMaxX,		pElement, FIELD_XML_ATTR_MAX_X);
	_Attribute(&pFieldInfo->m_fMaxY,		pElement, FIELD_XML_ATTR_MAX_Y);

	_Attribute(&pFieldInfo->m_rtBoundary.left,		pElement, FIELD_XML_ATTR_BOUNDARY_MIN_X);
	_Attribute(&pFieldInfo->m_rtBoundary.top,		pElement, FIELD_XML_ATTR_BOUNDARY_MIN_Y);
	_Attribute(&pFieldInfo->m_rtBoundary.right,		pElement, FIELD_XML_ATTR_BOUNDARY_MAX_X);
	_Attribute(&pFieldInfo->m_rtBoundary.bottom,	pElement, FIELD_XML_ATTR_BOUNDARY_MAX_Y);

	_Attribute(&pFieldInfo->m_bPotralTalentUsable,	pElement, FIELD_XML_ATTR_PORTALTALENT_USABLE);
	_Attribute(&pFieldInfo->m_bTutorial,			pElement, FIELD_XML_ATTR_TUTORIAL);

	wstring strValue;
	if (true == _Attribute(strValue, pElement, FIELD_XML_ATTR_FIXED_TIME))
	{
		for (int i = 0; i < MAX_TIME_TYPE; i++)
		{
			if (!_wcsicmp(strValue.c_str(), XStrings::GameTime(GAME_TIME_TYPE(i))))
			{
				pFieldInfo->m_TimeInfo.bFixed = true;
				pFieldInfo->m_TimeInfo.nFixedTime = GAME_TIME_TYPE(i);
				break;
			}
		}
	}

	MXmlElement* pChild = pElement->FirstChildElement();
	for( pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), MAPINFO_XML_TAG_MARKER))
			ParseMarker(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), MAPINFO_XML_TAG_SENSOR))
			ParseSensor(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_ENV))
			ParseEnv(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_PVPAREA))
			ParsePvparea(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_AREA))
			ParseArea(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_BGM))
			ParseBGM(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_FLOORS))
			ParseFloors(pChild, pXml, pFieldInfo);
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_SOUNDREVERB))
			ParseSoundReverbType(pChild, pXml, pFieldInfo);
		else if(!_stricmp(pChild->Value(), FIELD_XML_TAG_PRELOAD))
			ParsePreLoad(pChild, pXml, pFieldInfo);
	}
}

void XFieldInfoMgr::ParseBGM(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo)
{
	_Attribute(pFieldInfo->strBGMSoundName[0],	pElement, "fieldbgm");
	MXmlElement* pEnvBGMElement = pElement->FirstChildElement("ENVBGM");
	while (pEnvBGMElement != NULL)
	{
		for (int i=1; i<BGM_MAX; i++ )
		{
			if(pFieldInfo->strBGMSoundName[i].length() == 0)
			{
				bool bRet = _Attribute(pFieldInfo->strBGMSoundName[i], pEnvBGMElement, BGM_XML_TAG[i], "");
				if (bRet) break;
			}
		}

		pEnvBGMElement = pEnvBGMElement->NextSiblingElement();
	}
}

void XFieldInfoMgr::ParseFloors(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo)
{
	for ( MXmlElement* pChild = pElement->FirstChildElement();  pChild != NULL;  pChild = pChild->NextSiblingElement())
	{
		if ( _stricmp( pChild->Value(), FIELD_XML_TAG_FLOOR) == 0)
		{
			int nFloor;
			_Attribute( &nFloor, pChild, FIELD_XML_ATTR_HEIGHT, 0);

			pFieldInfo->m_vFloors.push_back( nFloor);
		}
	}
}

void XFieldInfoMgr::ParseSoundReverbType(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo)
{
	_Attribute( &pFieldInfo->m_nSoundReverbType, pElement, FIELD_XML_ATTR_REVERB, 0);
}

void XFieldInfoMgr::ParseMarker(MXmlElement* pElement, MXml* pXml, XFieldInfo* pMapInfo)
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
	info.nFieldID = pMapInfo->m_nFieldID;

	assert(pMapInfo->m_mapMarkerByID.find(info.nID) == pMapInfo->m_mapMarkerByID.end());
	pMapInfo->m_mapMarkerByID.insert(MARKER_INFO_MAP_BY_ID::value_type(info.nID, info));

	//assert(pMapInfo->m_mapMarkerByName.find(info.strName) == pMapInfo->m_mapMarkerByName.end());
	//pMapInfo->m_mapMarkerByName.insert(MARKER_INFO_MAP_BY_NAME::value_type(info.strName, info));
}

void XFieldInfoMgr::ParseSensor(MXmlElement* pElement, MXml* pXml, XFieldInfo* pMapInfo)
{
	XSensorInfo info;

	_Attribute(&info.nID, pElement, FIELD_XML_ATTR_ID);
	_Attribute(info.strName, pElement, FIELD_XML_ATTR_NAME);
	_Attribute(&info.bLocal, pElement, FIELD_XML_ATTR_LOCAL);
	_Attribute(&info.vPoint.x, pElement, FIELD_XML_ATTR_POS_X);
	_Attribute(&info.vPoint.y, pElement, FIELD_XML_ATTR_POS_Y);
	_Attribute(&info.vPoint.z, pElement, FIELD_XML_ATTR_POS_Z);
	_Attribute(&info.bStartingEnable, pElement, FIELD_XML_ATTR_ENABLED);
	_Attribute(&info.fRadius, pElement, FIELD_XML_ATTR_POS_RADIUS);
	_Attribute(info.strEffect, pElement, FIELD_XML_ATTR_EFFECT);
	if (_Attribute(info.strGroupName, pElement, "groupname", "none"))
	{
		if (info.strGroupName != L"none")
		{
			map<wstring, vector<int>>::iterator itor;
			itor = pMapInfo->m_mapGroupSensor.find(info.strGroupName);
			if (itor == pMapInfo->m_mapGroupSensor.end())
			{
				vector<int> list;
				list.push_back(info.nID);
				pMapInfo->m_mapGroupSensor.insert(map<wstring, vector<int>>::value_type(info.strGroupName, list));
			}
			else
			{
				vector<int>& list = itor->second;
				list.push_back(info.nID);
			}
		}
	}

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), FIELD_XML_TAG_WARP))
		{
			SENSOR_WARP_INFO infoWarp;

			string strWarpType;
			if (_Attribute(strWarpType,			pChild, FIELD_XML_ATTR_WARP_TYPE))
			{
				infoWarp.eType = static_cast<WARP_TYPE>(XStrings::StringToWarpType(MLocale::ConvAnsiToUCS2(strWarpType.c_str()).c_str()));
			}			
			
			wstring strTitle;
			_Attribute(strTitle,				pChild, FIELD_XML_ATTR_TITLE, "Untitled");
			wcsncpy_s(infoWarp.szTitle, strTitle.c_str(), 80);

			_Attribute(infoWarp.nFieldID,		pChild, FIELD_XML_ATTR_FIELD);
			_Attribute(infoWarp.nMarkerID,		pChild, FIELD_XML_ATTR_MARKER);
			_Attribute(infoWarp.nFieldGroupID,	pChild, FIELD_XML_ATTR_FIELD_GROUP);
			_Attribute(infoWarp.nQuestID,		pChild, FIELD_XML_ATTR_QUEST);
			_Attribute(infoWarp.nConditionID,	pChild, FIELD_XML_ATTR_CONDITION);
			
			string strGateType;
			if (_Attribute(strGateType,		pChild, FIELD_XML_ATTR_GATE_TYPE))
			{
				infoWarp.eGateType = static_cast<GATE_TYPE>(XStrings::StringToGateType(MLocale::ConvAnsiToUCS2(strGateType.c_str()).c_str()));
			}
			
			if (strGateType == GATE_TYPE_STR[1])
			{
				infoWarp.eGateType = TGT_MANUAL;
			}
			else
			{
				infoWarp.eGateType = TGT_AUTO;
			}

			info.vecWarpInfos.push_back(infoWarp);
		}
		else if (!_stricmp(pChild->Value(), FIELD_XML_TAG_EFFECT))
		{
			//TODO 센서이팩트 정리되면 empty검사는 없애도 된다.
			if (info.strEffect.empty())
			{
				_Attribute(info.strEffect, pChild, FIELD_XML_ATTR_EFFECT);

				_Attribute(&info.vEffectLocalPos.x, pChild, FIELD_XML_ATTR_POS_X);
				_Attribute(&info.vEffectLocalPos.y, pChild, FIELD_XML_ATTR_POS_Y);
				_Attribute(&info.vEffectLocalPos.z, pChild, FIELD_XML_ATTR_POS_Z);

				_Attribute(&info.vEffectDir.x, pChild, FIELD_XML_ATTR_DIR_X);
				_Attribute(&info.vEffectDir.y, pChild, FIELD_XML_ATTR_DIR_Y);
				_Attribute(&info.vEffectDir.z, pChild, FIELD_XML_ATTR_DIR_Z);

				_Attribute(&info.vEffectUp.x, pChild, FIELD_XML_ATTR_UP_X);
				_Attribute(&info.vEffectUp.y, pChild, FIELD_XML_ATTR_UP_Y);
				_Attribute(&info.vEffectUp.z, pChild, FIELD_XML_ATTR_UP_Z);

				_Attribute(&info.vEffectScale.x, pChild, FIELD_XML_ATTR_SCALE_X);
				_Attribute(&info.vEffectScale.y, pChild, FIELD_XML_ATTR_SCALE_Y);
				_Attribute(&info.vEffectScale.z, pChild, FIELD_XML_ATTR_SCALE_Z);
			}
		}
	}

	assert(pMapInfo->m_mapSensorByID.find(info.nID) == pMapInfo->m_mapSensorByID.end());
	pMapInfo->m_mapSensorByID.insert(SENSOR_INFO_MAP_BY_ID::value_type(info.nID, info));
}

void XFieldInfoMgr::ParseEnv(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo)
{
	MXmlElement* pChild = pElement->FirstChildElement();
	for( pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		//if (!_stricmp(pChild->Value(), FIELD_XML_TAG_FOG))
		//{
		//	_Attribute(&pFieldInfo->Fog.bEnabled,	pChild, FIELD_XML_ATTR_ENABLED);
		//	_Attribute(&pFieldInfo->Fog.fNearZ,		pChild, FIELD_XML_ATTR_NEAR);
		//	_Attribute(&pFieldInfo->Fog.fFarZ,		pChild, FIELD_XML_ATTR_FAR);
		//	
		//	int a=0,r=0,g=0,b=0;
		//	_Attribute(&a,							pChild, FIELD_XML_ATTR_COLOR_A);
		//	_Attribute(&r,							pChild, FIELD_XML_ATTR_COLOR_R);
		//	_Attribute(&g,							pChild, FIELD_XML_ATTR_COLOR_G);
		//	_Attribute(&b,							pChild, FIELD_XML_ATTR_COLOR_B);

		//	pFieldInfo->Fog.nColor = D3DCOLOR_ARGB(a, r, g, b);
		//}
		if (!_stricmp(pChild->Value(), FIELD_XML_TAG_SKY))
		{
			_Attribute(pFieldInfo->strSkyModelName,	pChild, FIELD_XML_ATTR_MODEL);
		}
	}
}

void XFieldInfoMgr::ParsePvparea( MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo )
{
	PVP_AREA_INFO infoPVP;
	_Attribute(&infoPVP.rcArea.left, pElement, FIELD_XML_ATTR_LEFT);
	_Attribute(&infoPVP.rcArea.top, pElement, FIELD_XML_ATTR_TOP);
	_Attribute(&infoPVP.rcArea.right, pElement, FIELD_XML_ATTR_RIGHT);
	_Attribute(&infoPVP.rcArea.bottom, pElement, FIELD_XML_ATTR_BOTTOM);
	_Attribute(&infoPVP.nFaction1, pElement, FIELD_XML_ATTR_FACTION1);
	_Attribute(&infoPVP.nFaction2, pElement, FIELD_XML_ATTR_FACTION2);

	_Attribute(infoPVP.strFactionName1, pElement, FIELD_XML_ATTR_FACTION_NAME1);
	_Attribute(infoPVP.strFactionName2, pElement, FIELD_XML_ATTR_FACTION_NAME2);


	pFieldInfo->m_PvPAreas.push_back(infoPVP);
}

void XFieldInfoMgr::ParseArea( MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo )
{
	AREA_INFO infoArea;
	infoArea.bEnterLocal = false;

	_Attribute(&infoArea.nID, pElement, FIELD_XML_ATTR_ID);

	// 문자열 테이블 참조하도록 합니다.
	infoArea.strName = CSFormatString::Format(L"AREA_NAME_{0}_{1}", FSParam(pFieldInfo->m_nFieldID, infoArea.nID));

	_Attribute(&infoArea.bPath, pElement, FIELD_XML_ATTR_PATH, false);

	MXmlElement* pChild = pElement->FirstChildElement();
	for( pChild; pChild != NULL; pChild=pChild->NextSiblingElement() )
	{
		if (!_stricmp(pChild->Value(), "CYLINDER"))
		{
			XCylinder cylinder;
			_Attribute(&cylinder.bottom.x	, pChild, FIELD_XML_ATTR_POS_X);
			_Attribute(&cylinder.bottom.y	, pChild, FIELD_XML_ATTR_POS_Y);
			_Attribute(&cylinder.bottom.z	, pChild, FIELD_XML_ATTR_POS_Z);
			_Attribute(&cylinder.radius		, pChild, FIELD_XML_ATTR_POS_RADIUS);
			_Attribute(&cylinder.height		, pChild, "height");

			infoArea.veccylinder.push_back(cylinder);
		}
	}

	pFieldInfo->m_Areas.push_back(infoArea);
}

bool XFieldInfoMgr::LoadField(const wchar_t* szFileName, XFieldInfo* pFieldInfo)
{
	MXml xml;

	std::string strFileName = MLocale::ConvUTF16ToAnsi(szFileName);
	if (!xml.LoadFile(strFileName.c_str())) 
	{
		_ASSERT(0);
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FIELD_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), FIELD_XML_TAG_FIELDINFO))
		{
			ParseFieldInfo(pElement, &xml, pFieldInfo);
		}
	}

	std::string strPath = MGetPurePath(strFileName);
	std::string strPureFileName = MGetPureFileName(strFileName);

	std::string strFiledNpcFileName = strPath + "map/" + strPureFileName + ".npc.xml";
	MXml npcFieldXml;
	if (npcFieldXml.LoadFile(strFiledNpcFileName.c_str()))
	{
		MXmlHandle docFiledNpcHandle = npcFieldXml.DocHandle();
		MXmlElement* pNpcElement = docFiledNpcHandle.FirstChild(FIELD_XML_TAG_MAIET).FirstChildElement().Element();

		char buffer[64] = {0, };
		NPC_UIMAP_INFO info;

		while(pNpcElement)
		{
			_Attribute(info.nNPCID, pNpcElement, "id");
			_Attribute( &info.vPos.x, pNpcElement, "x", 0.0f);
			_Attribute( &info.vPos.y, pNpcElement, "y", 0.0f);
			_Attribute( &info.vPos.z, pNpcElement, "z", 0.0f);

			for (int i = 1; i <= NPC_UIMAP_INFO::TYPE_CNT; ++i)
			{
				sprintf(buffer, "type%d", i);
				if(_Attribute(info.strTypeName, pNpcElement, buffer, &npcFieldXml))
				{
					sprintf(buffer, "icon%d", i);
					_Attribute(info.strIconName, pNpcElement, buffer, &npcFieldXml);
					info.strLookState = MLocale::ConvUTF16ToAnsi(info.strIconName);

					if(info.strTypeName == L"craft")
					{
						info.strLookPart = "artisan";
						pFieldInfo->m_NpcUiInfo[NPC_UIMAP_INFO::TYPE_CRAFT].push_back( info );
					}
					else if (info.strTypeName == L"shop")
					{
						info.strLookPart = "merchant";
						pFieldInfo->m_NpcUiInfo[NPC_UIMAP_INFO::TYPE_SHOP].push_back( info );
					}
				}
			}
			pNpcElement = pNpcElement->NextSiblingElement("NPC");
		}
	}

	return true;
}


XFieldInfo* XFieldInfoMgr::NewFieldInfo(int nFieldID)
{
	CSFieldFileInfo* pFieldFileInfo = m_FieldList.Find(nFieldID);
	if (pFieldFileInfo == NULL) return NULL;

	XFieldInfo* pFieldInfo = new XFieldInfo();
	pFieldInfo->m_nFieldID = pFieldFileInfo->nFieldID;
	pFieldInfo->m_strFieldName = pFieldFileInfo->strName;
	pFieldInfo->m_strFieldFile = pFieldFileInfo->strFile;

	wstring strFileName = wstring(PATH_FIELD) + pFieldFileInfo->strFile + wstring(FILEEXT_FIELD);
	if (LoadField(strFileName.c_str(), pFieldInfo) == false)
	{
		delete pFieldInfo;
		return NULL;
	}

	// 존 정보
	CSZoneFileInfo* pZoneFileInfo = m_ZoneList.Find(pFieldInfo->m_nZoneID);
	if (pZoneFileInfo)
	{
		pFieldInfo->m_strZoneName = pZoneFileInfo->strName;
		pFieldInfo->m_strZoneFile = pZoneFileInfo->strFile;
	}

	// 필드스크립트 로딩.
	global.script->GetGlueFieldEvent().Load(pFieldInfo);

	// 필드 정보 후처리
	pFieldInfo->Cook();

	return pFieldInfo;
}

XFieldInfo* XFieldInfoMgr::NewFieldInfo(const wchar_t* szFieldName)
{
	wstring strFile = wstring(szFieldName);
	XFieldInfo* pFieldInfo = new XFieldInfo();
	if (LoadField(strFile.c_str(), pFieldInfo) == false)
	{
		delete pFieldInfo;
		return NULL;
	}

	// 존 정보
	CSZoneFileInfo* pZoneFileInfo = m_ZoneList.Find(pFieldInfo->m_nZoneID);
	if (pZoneFileInfo)
	{
		pFieldInfo->m_strZoneName = pZoneFileInfo->strName;
		pFieldInfo->m_strZoneFile = pZoneFileInfo->strFile;
	}

	// 필드스크립트 로딩.
	global.script->GetGlueFieldEvent().Load(pFieldInfo);

	// 필드 정보 후처리
	pFieldInfo->Cook();

	return pFieldInfo;
}

XFieldInfo* XFieldInfoMgr::NewFieldInfoFromZone(const wchar_t* szZoneName)
{
	wstring strFile = wstring(szZoneName);

	XFieldInfo* pFieldInfo = new XFieldInfo();

	// 존 정보
	pFieldInfo->m_strZoneName = szZoneName;
	pFieldInfo->m_strZoneFile = szZoneName;

	return pFieldInfo;
}

XFieldInfo* XFieldInfoMgr::GetSimpleFieldInfo( int nFieldID)
{
	CSFieldFileInfo* pFieldFileInfo = m_FieldList.Find( nFieldID);
	if ( pFieldFileInfo == NULL)		return NULL;

	XFieldInfo* pFieldInfo = GetFieldInfo(nFieldID);
	if (pFieldInfo != NULL) return pFieldInfo;

	pFieldInfo = new XFieldInfo();
	pFieldInfo->m_nFieldID = pFieldFileInfo->nFieldID;
	pFieldInfo->m_strFieldName = pFieldFileInfo->strName;
	pFieldInfo->m_strFieldFile = pFieldFileInfo->strFile;

	wstring strFileName = wstring(PATH_FIELD) + pFieldFileInfo->strFile + wstring(FILEEXT_FIELD);
	if (LoadField(strFileName.c_str(), pFieldInfo) == false)
	{
		delete pFieldInfo;
		return NULL;
	}

	// 심플 필드정보 캐쉬
	m_mapSimpleFieldInfo.insert(std::map< int, XFieldInfo*>::value_type(nFieldID, pFieldInfo));

	return pFieldInfo;
}

XFieldInfo* XFieldInfoMgr::GetFieldInfo(int nFieldID)
{
	std::map< int, XFieldInfo*>::iterator itor = m_mapSimpleFieldInfo.find(nFieldID);

	if (itor != m_mapSimpleFieldInfo.end()) 
	{
		return itor->second;
	} 

	return NULL;
}


const wchar_t* XFieldInfoMgr::GetFieldName( int nFieldID )
{
	CSFieldFileInfo* pFieldFileInfo = m_FieldList.Find(nFieldID);
	if (pFieldFileInfo == NULL) return L"";

	return XGetStr(pFieldFileInfo->strName);
}

bool XFieldInfoMgr::LoadFieldSound( XFieldInfo* pFieldInfo)
{
	if ( pFieldInfo == NULL)		return false;

	wchar_t szFieldName[ MAX_PATH_NAME_LEN];
	swprintf_s( szFieldName,  L"%s%d%s", PATH_FIELD, pFieldInfo->m_nFieldID, FILEEXT_FIELD_SOUND);


	// Clear field sound
	XGetSoundField()->Clear();
	pFieldInfo->m_vecSound.clear();


	// Clear local reverb
	if ( global.sound)
		global.sound->ClearLocalReverb();


	MXml xml;
	if ( xml.LoadFile( MLocale::ConvUTF16ToAnsi( szFieldName).c_str()) == false)
		return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild( FIELD_XML_TAG_MAIET).FirstChildElement().Element();
	for ( pElement;  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		if ( _stricmp( pElement->Value(), "SOUND") == 0)
		{
			FIELDSOUND_INFO info;

			_Attribute( info.strName, pElement, "name");
			_Attribute( &info.vPos.x, pElement, "x", 0.0f);
			_Attribute( &info.vPos.y, pElement, "y", 0.0f);
			_Attribute( &info.vPos.z, pElement, "z", 0.0f);
			_Attribute( &info.fMinDist, pElement, "distance_min", 100.0f);
			_Attribute( &info.fMaxDist, pElement, "distance_max", 1000.0f);
			_Attribute( &info.bMuteAmbient, pElement, "mute_ambient", false);
			_Attribute( &info.nReverb, pElement, "reverb", -1);

			if ( info.strName.empty() == false)
				pFieldInfo->m_vecSound.push_back( info);

			if ( info.nReverb >= 0)
			{
				if ( global.sound)
					global.sound->AddLocalReverb( info.nReverb, info.vPos, info.fMinDist, info.fMaxDist);
			}
		}
	}

	return true;
}

wstring XFieldInfoMgr::GetZoneNameFromFieldID( int nFieldID )
{
	CSFieldFileInfo* pFieldFileInfo = m_FieldList.Find( nFieldID);
	if ( pFieldFileInfo == NULL)
		return L"";

	wstring strFileName = PATH_FIELD + pFieldFileInfo->strFile + FILEEXT_FIELD;

	MXml xml;

	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(strFileName.c_str()).c_str())) 
	{
		_ASSERT(0);
		return L"";
	}

	int nZoneID = 0;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FIELD_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), FIELD_XML_TAG_FIELDINFO))
		{
			_Attribute(&nZoneID,		pElement, FIELD_XML_ATTR_ZONE);
		}
	}

	CSZoneFileInfo* pZoneFileInfo = m_ZoneList.Find(nZoneID);
	if (pZoneFileInfo)
	{
		return pZoneFileInfo->strFile;
	}

	return L"";

}

bool XFieldInfoMgr::IsFieldPreLoadResource( const wstring& _resName )
{
	if (m_PreLoadResources.find(_resName) == m_PreLoadResources.end())
		return false;

	return true;
}

void XFieldInfoMgr::ClearFieldPreLoadResources()
{
	for (std::map< std::wstring, rs3::RResource* >::iterator itr = m_PreLoadResources.begin();
		itr != m_PreLoadResources.end(); ++itr)
	{
		RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
		if (pSceneManager)
		{
			pSceneManager->ReleaseResource(itr->second);
		}
	}
	m_PreLoadResources.clear();
}

void XFieldInfoMgr::ParsePreLoad( MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo )
{
	pFieldInfo->m_ParsedPreLoadResources.clear();

	MXmlElement* pEluElement = pElement->FirstChildElement("ELU");
	while(pEluElement)
	{
		string name;
		_Attribute(name, pEluElement, "name");

		pFieldInfo->m_ParsedPreLoadResources.push_back(name);
		pEluElement = pEluElement->NextSiblingElement("ELU");
	}
}

void XFieldInfoMgr::LoadFieldPreLoadResources(XFieldInfo* pFieldInfo)
{
	_ASSERT(m_PreLoadResources.empty());
	if (NULL == pFieldInfo)
		return;
	
	for(std::vector< std::string >::iterator itr = pFieldInfo->m_ParsedPreLoadResources.begin();
		itr != pFieldInfo->m_ParsedPreLoadResources.end(); ++itr)
	{
		wstring nameUTF16 = MLocale::ConvAnsiToUTF16(itr->c_str());
		RMesh* pMesh = REngine::GetSceneManager().CreateResource< RMesh >( itr->c_str() );
		if (pMesh->BuildAll() && (m_PreLoadResources.find(nameUTF16) == m_PreLoadResources.end()))
		{
			m_PreLoadResources.insert(std::map< std::wstring, rs3::RResource* >::value_type(nameUTF16, pMesh));
		}
		else
		{
			REngine::GetSceneManager().ReleaseResource( pMesh );
		}
	}
}

XSensorInfo	XFieldInfo::m_NullSensorInfo;

XSensorInfo& XFieldInfo::GetSensorInfo( unsigned long nID )
{
	SENSOR_INFO_MAP_BY_ID::iterator itor = m_mapSensorByID.find(nID);

	if (m_mapSensorByID.end() == itor) return m_NullSensorInfo;

	return (*itor).second;
}

void XFieldInfo::Cook_PvPAreas()
{
	// 서버에서는 CELL로 PVP 지역을 구분하므로 클라이언트도 CELL 경계값에 맞게 다시 맞춰준다.
	for (size_t i = 0; i < m_PvPAreas.size(); ++i)
	{
		PVP_AREA_INFO& area = m_PvPAreas[i];

		MPoint ptGridLeftTop = CSFieldInfoHelper::ConvertToGridPos(area.rcArea.left, area.rcArea.top, m_fMinX, m_fMinY);
		MPoint ptGridRightBottom = CSFieldInfoHelper::ConvertToGridPos(area.rcArea.right, area.rcArea.bottom, m_fMinX, m_fMinY);

		area.rcArea.left = m_fMinX + (ptGridLeftTop.x * FIELD_ATTR_CELL_SIZE);
		area.rcArea.top = m_fMinY + (ptGridLeftTop.y * FIELD_ATTR_CELL_SIZE);

		area.rcArea.right = m_fMinX + ((ptGridRightBottom.x + 1) * FIELD_ATTR_CELL_SIZE);
		area.rcArea.bottom = m_fMinY + ((ptGridRightBottom.y + 1) * FIELD_ATTR_CELL_SIZE);
	}
}

void XFieldInfo::Cook_Sensors()
{
	vector<XQuestInfo*>& vecQuestInfos = info.quest->GetAllQuestInfo();

	for (vector<XQuestInfo*>::iterator itor = vecQuestInfos.begin(); itor != vecQuestInfos.end(); ++itor)
	{
		XQuestInfo* pQuestInfo = *itor;

		if(pQuestInfo->IsAutoParty())
		{
			if(pQuestInfo->nPartyFieldID == m_nFieldID)
			{
				SENSOR_INFO_MAP_BY_ID::iterator itor = m_mapSensorByID.find(pQuestInfo->nPartySensorID);
				if(itor == m_mapSensorByID.end()) continue;

				XSensorInfo pSenSorInfo = (*itor).second;
				pSenSorInfo.bAutoParty = true;
				pSenSorInfo.nQuestID = pQuestInfo->nID;

				m_mapSensorByID.erase(itor);
				m_mapSensorByID.insert(SENSOR_INFO_MAP_BY_ID::value_type(pSenSorInfo.nID, pSenSorInfo));
			}
		}
	}
}

MPoint XFieldInfo::GetSectorIndex( float fPosX, float fPosY )
{
	float fMin_X = m_fMinX;
	float fMin_Y = m_fMinY;

	int x = (int)((fPosX - fMin_X) / SECTOR_SIZE);
	int y = (int)((fPosY - fMin_Y) / SECTOR_SIZE);

	return MPoint(x, y);
}

vec3 XFieldInfo::GetNPCPos( int nNPCID, bool bFilter )
{
	for(int i = NPC_UIMAP_INFO::TYPE_CNT-1; i >=0 ; --i)
	{
		NPC_UIMAP_INFO_VECTOR* pVecNpcUiInfo = NULL;
		if (bFilter)
		{
			if ((i == NPC_UIMAP_INFO::TYPE_CRAFT && XGETCFG_GAME_SHOWCRAFTICON) ||
				(i == NPC_UIMAP_INFO::TYPE_SHOP && XGETCFG_GAME_SHOWSHOPICON) )
			{
				pVecNpcUiInfo = &m_NpcUiInfo[i];
			}
		}
		else
		{
			pVecNpcUiInfo = &m_NpcUiInfo[i];
		}
		

		if (NULL == pVecNpcUiInfo) continue;

		for (NPC_UIMAP_INFO_VECTOR:: iterator itr = pVecNpcUiInfo->begin(); itr != pVecNpcUiInfo->end(); ++itr)
		{
			NPC_UIMAP_INFO& info = (*itr);
			if (info.nNPCID == nNPCID)	return info.vPos;
		}
	}

	return vec3::ZERO;
}

wstring XFieldInfo::GetName()
{
	return XGetStr(m_strFieldName);
}

wstring AREA_INFO::GetName()
{
	return XGetStr(strName);
}
