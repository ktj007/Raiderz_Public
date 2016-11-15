#include "stdafx.h"
#include "XWorld.h"
#include "XBaseApplication.h"
#include "RSceneManager.h"
#include "RCameraSceneNode.h"
#include "XFieldInfo.h"
#include "RGrassSystem.h"
#include "XMyPlayer.h"
#include "XModuleEntity.h"
#include "CCommandTable.h"
#include "XController.h"
#include "XZone.h"
#include "XCollisionTree.h"
#include "XFieldInfoBuilder.h"

#include "RTreePassRenderer.h"
#include "RTreeSceneNode.h"

#include "RBackgroundWorker.h"

#include "RHeightField.h"
#include "RTexture.h"

#include "XTextureMtInfo.h"
#include "XSoundField.h"

#include "MMemoryLeak.h"
#include "RPointLightSceneNode.h"

#include "RSceneNodeInstance.h"
#include "RMeshNode.h"

//////////////////////////////////////////////////////////////////////////
XWorld::XWorld() : m_pTerrain(NULL), m_pInfo(NULL), m_pWorldSceneNode(NULL)
{
	m_pZone = new XZone();
	
	m_bWorldLoadingComplete = false;
}

XWorld::~XWorld()
{
	SAFE_DELETE(m_pZone);
}

void XWorld::Final(bool bForChange)
{
	gvar.World.nDynamicFieldExpiredTime = 0;

	if (m_pInfo)
	{
		delete m_pInfo;
		m_pInfo = NULL;
	}

	if (!bForChange)
	{
		m_pTerrain = NULL;
	}

	m_Env.Destroy();

	if( m_pWorldSceneNode )
	{
		//m_pWorldSceneNode->Clear();

		if (!bForChange)
		{
			global.smgr->DeleteRootSceneNode(m_pWorldSceneNode);
			m_pWorldSceneNode = NULL;
			//SAFE_DELETE(m_pWorldSceneNode);
		}
	}

	// 필드 프리로드 정리
	if (info.field)
		info.field->ClearFieldPreLoadResources();
}

void XWorld::FinalForChange()
{
	Final(true);
}


bool XWorld::Init(XWorldInitParam& worldInitParam)
{
	m_WorldInitParam = worldInitParam;

	_ASSERT(MMemoryLeak::CheckMemory());

	// 필드
	if (InitFieldInfo(worldInitParam.nFieldID, worldInitParam.strZoneName.c_str()) == false)
	{
		return false;
	}

	// 로드할 월드의 파일이름 설정
	wstring strZoneFileName( PATH_ZONE + m_pInfo->m_strZoneFile + L"/" + m_pInfo->m_strZoneFile + FILEEXT_ZONE );
	wstring strZoneEnvFileName( PATH_ZONE + m_pInfo->m_strZoneFile + L"/" + m_pInfo->m_strZoneFile + FILEEXT_ZONE_ENV );
	wstring strZoneSoundFileName( PATH_ZONE + m_pInfo->m_strZoneFile + L"/" + m_pInfo->m_strZoneFile + FILEEXT_ZONE_SOUND);

	// 월드 씬노드
	_InitWorldScene(strZoneFileName, worldInitParam.bBackgroundLoading);

	// 환경
	_InitEnv(strZoneEnvFileName);

	// 사운드
	LoadZoneSound(strZoneSoundFileName.c_str());

	// 초기 장치설정 --
	InitDevice();

	// 로딩된 인스턴스들을 가지고 세팅하는 부분 --
	SetupTerrainAndTree(); // Terrain/Tree 설정

	// 컬리전 --
	m_pZone->Load(m_pInfo, m_pTerrain);

	// 백그라운드 로딩이 아닐때
	if (worldInitParam.bBackgroundLoading == false)
	{
		OnWorldLoadingComplete();
	}

	_ASSERT(MMemoryLeak::CheckMemory());

	// 로딩 완료
	dlog("XWorld::InitWorld() Completed\n");
	return true;
}

bool XWorld::InitForChange(XWorldInitParam& worldInitParam)
{
	return Init(worldInitParam);
}

void XWorld::SetupTerrainAndTree()
{
	m_pTerrain = global.smgr->GetCurrentTerrain();

	// 지형 관련 설정
	if (m_pTerrain)
	{
		m_pTerrain->SetGrassLodParams(XCONST::GRASS_LOD_NEAR, XCONST::GRASS_LOD_FAR);
	}
}

float XWorld::GetGroundZ( vec3& vPos, float fPickAddOriginZ /*= 10.0f*/ )
{
	return m_pZone->GetGroundZ(vPos, fPickAddOriginZ);
}

MPoint XWorld::GetSectorIndex(float fPosX, float fPosY)
{
	if(m_pInfo)
		return m_pInfo->GetSectorIndex(fPosX, fPosY);

	return MPoint(0, 0);
}


bool XWorld::IsManagedSector(int tar_sx, int tar_sy, int my_sx, int my_sy, const int nSectorSize)
{
	return ((abs(my_sx - tar_sx)<=nSectorSize) && (abs(my_sy - tar_sy)<=nSectorSize));
}


void XWorld::Update(float fDelta)
{
	m_Env.Update(fDelta, GetCollisionTree());
}

void XWorld::OnCmdSetTime(GAME_TIME_TYPE nTime, bool bFade, float fFadingTime)
{
	if (IsNowLoading())
		return;

	m_Env.SetTime(nTime, bFade, fFadingTime);

	SetupTerrainAndTree();			// 시간 변경에 따른 지형과 나무 설정

	PlayFieldSound();
}

void XWorld::OnCmdSetWeather(GAME_WEATHER_TYPE nWeather, bool bFade, float fFadingTime)
{
	if (IsNowLoading())
		return;

	m_Env.SetWeather(nWeather, bFade, fFadingTime);

	PlayFieldSound();
}

bool XWorld::OnCmdSetEnv(const wchar_t* szEnvname, GAME_TIME_TYPE nTime, GAME_WEATHER_TYPE nWeather, bool bFade /* = false */, float fFadingTime /* = 0.0f */)
{
	if (IsNowLoading())
		return false;

	m_Env.SetEnvSet(szEnvname);

	bool ret = m_Env.SetEnv(nTime, nWeather, bFade, fFadingTime);

	SetupTerrainAndTree();			// 시간 변경에 따른 지형과 나무 설정

	PlayFieldSound();

	return ret;
}

bool XWorld::OnCmdSetEnv(GAME_TIME_TYPE nTime, GAME_WEATHER_TYPE nWeather, bool bFade, float fFadingTime)
{
	if (IsNowLoading())
		return false;

	bool ret = m_Env.SetEnv(nTime, nWeather, bFade, fFadingTime);

	SetupTerrainAndTree();			// 시간 변경에 따른 지형과 나무 설정

	PlayFieldSound();

	return ret;
}

bool XWorld::IsEnvSet(string szEnvname )
{
	std::vector<std::string> vecArray;
	m_Env.GetEnvironmentSet()->GetNames(vecArray);

	for(std::vector<std::string>::iterator itor = vecArray.begin(); itor != vecArray.end(); ++itor)
	{
		if (szEnvname == *(itor))	return true;
	}

	return false;
}

bool XWorld::PickCollision( vec3 vOrigin, vec3 vTo, vec3* pvOutPos/*=NULL*/, vec3* pvOutNormal/*=NULL*/, DWORD dwAttribute )
{
	return m_pZone->PickCollision(vOrigin, vTo, pvOutPos, pvOutNormal, dwAttribute);
}

wstring XWorld::GetMaterialName(vec3 vEntityPosition)
{
	return m_pZone->GetMaterialName(vEntityPosition);
}

void XWorld::InitDevice()
{
	global.app->SetClearColor(0xFF000000);
	global.device->SetAmbient(0xFFFFFFFF);
	global.device->SetCullMode(RCULL_CW);
	global.device->SetTextureFilter(0,RTF_LINEAR);
	global.device->SetTextureFilter(1,RTF_LINEAR);
	global.device->SetLighting(false);
	global.device->SetDepthBias(0);
}

bool XWorld::InitFieldInfo( int nFieldID, const wchar_t* szZoneName)
{
	// world info 초기화 --
	if (m_pInfo)
	{
		delete m_pInfo;
	}

	// 씬 로딩 정보 읽기 --
	XFieldInfoBuilder fieldInfoBuilder;
	m_pInfo = fieldInfoBuilder.NewFieldInfo(global.app->GetMode(), nFieldID, szZoneName);

	info.field->LoadFieldPreLoadResources( m_pInfo );
	info.field->LoadFieldSound( m_pInfo);

	if (m_pInfo == NULL)
	{
		mlog3("XWorld::InitWorld() - 맵이 없음 (field: %d)\n", nFieldID);
		_ASSERT(0);	// 맵이 없다
		return false;
	}
	return true;
}

// 필드 정보가 로드 된 다음 존 정보를 로드 한다.
bool XWorld::LoadZoneSound( const wchar_t* szFileName )
{
	if ( m_pInfo == NULL)	return false;

	MXml xml;
	if ( xml.LoadFile( MLocale::ConvUTF16ToAnsi(szFileName).c_str()) == false)	return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild( "ROOT").FirstChildElement().Element();
	for ( pElement;  pElement != NULL;  pElement = pElement->NextSiblingElement())
	{
		if ( _stricmp( pElement->Value(), "SOUND") == 0)
		{
			FIELDSOUND_INFO info;

			_Attribute( info.strName, pElement, "soundname");
			if ( info.strName.empty()  ||  info.strName.find( L"auto") != -1)		continue;

			_Attribute( &info.vPos.x, pElement, "x", 0.0f);
			_Attribute( &info.vPos.y, pElement, "y", 0.0f);
			_Attribute( &info.vPos.z, pElement, "z", 0.0f);
			_Attribute( &info.fMinDist, pElement, "distance_min", 100.0f);
			_Attribute( &info.fMaxDist, pElement, "distance_max", 1000.0f);
//			_Attribute( &info.m_nFlagType, pElement, "type", 11);
//			_Attribute( info.strEffectType, pElement, "effecttype");

			m_pInfo->m_vecSound.push_back( info);
		}
	}

	return true;
}

void XWorld::LoadObjSound()
{
	if ( m_pWorldSceneNode == NULL  ||  m_pInfo == NULL)	return;

	const char* SET_SOUND		= "set_sound";
	const char* SET_SOUND_NAME	= "set_sound_name";
	const char* SET_SOUND_MIN	= "set_sound_minradius";
	const char* SET_SOUND_MAX	= "set_sound_maxradius";

	RSceneNodeList NodeList = m_pWorldSceneNode->GetChildren();
	for ( RSceneNodeList::iterator itr = NodeList.begin();  itr != NodeList.end();  itr++)
	{
		RSceneNode* pSceneNode = (*itr);
		if ( strcmp( pSceneNode->GetNodeIDString(), RSID_SCENEINSTANCE) != 0)		continue;

		const RSceneNodeList& listChildren = pSceneNode->GetChildren();
		if ( listChildren.empty() == true)											continue;

		RSceneNode* ppSceneNode = listChildren.front();
		if ( strcmp( ppSceneNode->GetNodeIDString(), RSID_ACTOR) != 0)				continue;


		RActor* pActor = static_cast<RActor*>( ppSceneNode);
		int count = pActor->GetActorNodeCount();
		for ( int nActorNodeIndex = 0;  nActorNodeIndex < count;  nActorNodeIndex++)
		{
			RActorNode *pActorNode = pActor->GetActorNodes()[ nActorNodeIndex];
			int nPropertyCount = pActorNode->m_pMeshNode->GetUserPropertyCount();
			if( nPropertyCount == 0)		continue;

			string strToken;
			const string& strProperty = pActorNode->m_pMeshNode->GetUserPropertyString( 0);
			stringstream sstream(strProperty);
			sstream >> strToken;

			if ( strcmp( strToken.c_str(),SET_SOUND) != 0)		continue;
			{
				sstream >> strToken;
				sstream >> strToken;

				if ( strcmp( strToken.c_str(), "true") !=0)		continue;


				FIELDSOUND_INFO info;
				info.vPos = pActorNode->GetWorldPosition();

				for ( int i = 1;  i < nPropertyCount;  i++)
				{
					const string& strProperty = pActorNode->m_pMeshNode->GetUserPropertyString( i);
					stringstream sstream( strProperty);
					sstream >> strToken;

					if ( strcmp( strToken.c_str(), SET_SOUND_NAME) == 0)
					{
						sstream >> strToken;
						sstream >> strToken;
						info.strName = MLocale::ConvAnsiToUTF16(strToken.c_str());
					}
					else if ( strcmp( strToken.c_str(), SET_SOUND_MIN) == 0)
					{
						sstream >> strToken;
						sstream >> strToken;
						info.fMinDist = atoi( strToken.c_str());
					}
					else if ( strncmp( strProperty.c_str(), SET_SOUND_MAX,strlen( SET_SOUND_MAX)) == 0)
					{
						sstream >> strToken;
						sstream >> strToken;
						info.fMaxDist = atoi( strToken.c_str());
					}
				}

				m_pInfo->m_vecSound.push_back( info);
			}
		}
	}
}

void XWorld::OnWorldLoadingComplete()
{
	if (m_pWorldSceneNode)
	{
		m_Env.OnWorldLoadingComplete( m_pWorldSceneNode ); // 월드 로딩이 완료된 후 환경 설정이 되어야 한다.
		m_Env.SetEnv(gvar.World.nGameTime, gvar.World.nWeather); // 환경 설정

		if ( global.sound)
			global.sound->SetReverb( m_pInfo->m_nSoundReverbType);

		PlayFieldSound();


		m_pWorldSceneNode->SetVisible(true);

		// 피직스
		if (m_WorldInitParam.bLoadPhysx)
		{
			vec3 vPos = vec3::ZERO;
			if (gvar.Game.pMyPlayer)
			{
				vPos = gvar.Game.pMyPlayer->GetPosition();
			}
			m_pZone->InitPxStaticBody(m_pInfo, m_pTerrain, vPos);
		}

		// 오브젝트 사운드
		LoadObjSound();

		m_bWorldLoadingComplete = true;
	}
}

void XWorld::_InitWorldScene( wstring &strZoneFileName, bool bBackgroundLoading )
{	
	_ASSERT(MMemoryLeak::CheckMemory());
	
	global.smgr->GetPrimaryCamera()->SetPosition( gvar.World.GetNextMapStartPos() );
	global.smgr->GetPrimaryCamera()->UpdateViewFrustrum();

	// 씬매니저로부터 리소스 로딩
	RBackgroundWorker& bg_worker = REngine::GetBackgroundWorker();

	bool bNewWorldSceneNode = false;

	if (m_pWorldSceneNode == NULL)
	{
		//m_pWorldSceneNode = new RWorldSceneNode();
		m_pWorldSceneNode = global.smgr->CreateRootSceneNode();
		 global.smgr->SetCurrentRootSceneNode(m_pWorldSceneNode);

		bNewWorldSceneNode = true;
	}

	if (bNewWorldSceneNode)
	{
		//REngine::GetBackgroundWorker().SetActive(false);

		m_pWorldSceneNode->SetXMLFileName( MLocale::ConvUTF16ToAnsi(strZoneFileName.c_str()) );
		global.smgr->LoadWorldSceneNodeFromRootXMLFileWithLoadingProgress(m_pWorldSceneNode, MLocale::ConvUTF16ToAnsi(strZoneFileName.c_str()).c_str(), bBackgroundLoading);

		//REngine::GetBackgroundWorker().SetActive(true);
	}

	bg_worker.FlushReadyQueueAndRun();

	m_pWorldSceneNode->SetVisible(false);

	m_bWorldLoadingComplete = false;

	_ASSERT(MMemoryLeak::CheckMemory());

}

void XWorld::_InitEnv( wstring &strZoneEnvFileName )
{
	// 환경 만들기 / 설정 --
	if (m_Env.Create(this) == false)
	{
		mlog3("WorldEnv Init Failed\n");
	}

	if(m_Env.LoadZoneEnv(strZoneEnvFileName.c_str()) == false)
	{
		mlog3("Error: WorldEnv Init Failed\n");
	}
}

rs3::RMatrix XWorld::GetTerrainMatrix( const vec3& vPos, const vec3& vDir, const vec3& vUp, bool bUseAlign )
{
	return m_pZone->GetTerrainMatrix(vPos, vDir, vUp, bUseAlign);
}

XCollisionTree* XWorld::GetCollisionTree()
{
	_ASSERT( m_pZone );
	return m_pZone->GetCollisionTree();
}


void XWorld::PlayFieldSound()
{
	if ( gg.game == NULL  ||  m_pInfo == NULL)			return;
	if ( XGetSoundField()->IsMutedAmbient() == true)	return;

	wstring strSound;
	if ( m_pInfo->strBGMSoundName[ DUNGEON_BGM].empty() == false)
	{
		strSound = m_pInfo->strBGMSoundName[ DUNGEON_BGM];
	}
	else
	{
		switch ( m_Env.GetCurrentWeather())
		{
		case WEATHER_HEAVY_RAINY :
			strSound = m_pInfo->strBGMSoundName[ HEAVYRAIN_BGM];
			break;

		case WEATHER_HEAVY_SNOWY :
			strSound = m_pInfo->strBGMSoundName[ HEAVYSNOW_BGM];
			break;

		case WEATHER_RAINY :
			strSound = m_pInfo->strBGMSoundName[ RAIN_BGM];
			break;

		case WEATHER_CLOUDY :
			strSound = m_pInfo->strBGMSoundName[ CLODY_BGM];
			break;


		default :

			switch ( m_Env.GetCurrentTime())
			{
			case TIME_DAWN :
				strSound = m_pInfo->strBGMSoundName[ DAWN_BGM];
				break;

			case TIME_DAYTIME :
				strSound = m_pInfo->strBGMSoundName[ DAYTIME_BGM];
				break;

			case TIME_SUNSET :
				strSound = m_pInfo->strBGMSoundName[ SUNSET_BGM];
				break;

			case TIME_NIGHT :
				strSound = m_pInfo->strBGMSoundName[ NIGHT_BGM];
				break;

			default :
				strSound = m_pInfo->strBGMSoundName[ NORMAL_BGM];
				break;
			}

			break;
		}
	}


	if ( global.sound)
	{
		global.sound->Stop( "ambient");
		global.sound->Play( MLocale::ConvUTF16ToAnsi(strSound.c_str()).c_str(), "ambient");
	}
}

MRect XWorld::GetPickZPartialArea()
{
	if(m_pZone->m_bPickZPartialArea)
	{
		return m_pZone->m_rtPickZPartialArea;
	}

	MRect rtEmpty;

	return rtEmpty;
}