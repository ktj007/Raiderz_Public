#include "stdafx.h"
#include "WS_Field.h"

#include "WS_Zone.h"

#include "VR_EnvWeather.h"
#include "VR_EntityObjectList.h"
#include "VR_EntityEnvironment.h"
#include "VR_EntityMaterial.h"
#include "VR_EtcPathFind.h"
#include "VR_EtcCollision.h"
#include "VR_EtcCamera.h"
#include "VR_EtcMapView.h"
#include "VR_EtcDev.h"
#include "VR_EnvFieldInfo.h"

#include "VD_DlgNewMiniMap.h"
#include "V_MFCHelper.h"

#include "C_StatusBarHandler.h"
#include "C_XMLConfig.h"
#include "C_XMLEditInfoFile.h"
#include "C_XMLResourceList.h"
#include "C_RubyManager.h"

#include "M_ToolLogicObjectManager.h"

#include "../../Server/GameServer/main/GPickingZTable.h"
#include "../../Server/GameServer/main/GPathFinder.h"
#include "../../Server/GameServer/main/GCollisionTree.h"

#include "MFileSystem.h"
#include "MMD5.h"

#include "C_ControlDoActionImpl.h"
#include "C_ControlMediatorHelperActionAddon.h"
#include "C_ControlMediatorHelperRenderAddon.h"
#include "C_ControlMediatorHelperResourceAddon.h"

#include "C_ControlPopupActionImpl.h"

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

CWorkSpaceField::CWorkSpaceField()
{
	m_weatherNameTable_[SUN] = "SUN";
	m_weatherNameTable_[CLD] = "CLD";
	m_weatherNameTable_[RNY] = "RNY";
	m_weatherNameTable_[HRN] = "HRN";
	m_weatherNameTable_[SNW] = "SNW";
	m_weatherNameTable_[HSW] = "HSW";
	m_weatherNameTable_[CLR] = "CLR";
	InitWeatherInfo_();

	m_nFromZoneID_ = -1;
	m_nFromFieldID_ = -1;
	m_fieldInfo.m_nReferZone = -1;
	m_fieldInfo.m_bTownField = false;
	m_fieldInfo.m_bReturnable = true;

}

const char* CWorkSpaceField::GetWorkSpaceKeyword()
{
	return "field.xml";
}

void CWorkSpaceField::InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction)
{
	_ASSERT( m_pControlMediator_ != NULL);
	CEntityObject::m_pControlMediator = m_pControlMediator_;


	// 환경
	int nEnvTabIdx = _rUiSetter.AddTab("환경");
	_rUiSetter.AddRollup(nEnvTabIdx, CRollupEnvFieldInfo::GetID(), true, WORKBENCH_STATE::STATE_NUM, false);
	_rUiSetter.AddRollup(nEnvTabIdx, CRollupEnvWeather::GetID(), false, WORKBENCH_STATE::STATE_NUM, false);

	// Entity
	int nEntityTabIdx = _rUiSetter.AddTab("Entity");
	_rUiSetter.AddRollup(nEntityTabIdx, CRollupEntityObjectList::GetID(), true, WORKBENCH_STATE::OBJECT_SELECT, true);
	_rUiSetter.AddRollup(nEntityTabIdx, CRollupEntityEnvironment::GetID(), false, WORKBENCH_STATE::OBJECT_SELECT, true);

	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_NEW, nEntityTabIdx, CRollupEntityObjectList::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_SELECT, nEntityTabIdx, CRollupEntityObjectList::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_MOVE, nEntityTabIdx, CRollupEntityObjectList::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_ROTATE, nEntityTabIdx, CRollupEntityObjectList::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::OBJECT_SCALE, nEntityTabIdx, CRollupEntityObjectList::GetID());

	// 기타
	int nEtcIdx = _rUiSetter.AddTab("기타", WORKBENCH_STATE::ETC_PATH_SELECT);
	_rUiSetter.AddRollup(nEtcIdx, CRollupEtcPathFind::GetID(), true, WORKBENCH_STATE::ETC_PATH_SELECT, true);
	_rUiSetter.AddRollup(nEtcIdx, CRollupEtcCollision::GetID(), true, WORKBENCH_STATE::ETC_PATH_SELECT, true);
	_rUiSetter.AddRollup(nEtcIdx, CRollupEtcCamera::GetID(), false, WORKBENCH_STATE::ETC_CAMERA, true);
	_rUiSetter.AddRollup(nEtcIdx, CRollupEtcMapView::GetID(), false, WORKBENCH_STATE::ETC_MAP_VIEW, true);
	_rUiSetter.AddRollup(nEtcIdx, CRollupEtcDev::GetID(), false, WORKBENCH_STATE::ETC_DEV, true);

	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_PATH_SELECT, nEtcIdx, CRollupEtcPathFind::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_CAMERA, nEtcIdx, CRollupEtcCamera::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_MAP_VIEW, nEtcIdx, CRollupEtcMapView::GetID());
	_rUiSetter.BindWBState(WORKBENCH_STATE::ETC_DEV, nEtcIdx, CRollupEtcDev::GetID());

	// 툴바
	_rUiSetter.m_sToolBarButton.m_bLightMap = false;


	//////////////////////////////////////////////////////////////////////////
	//
	// Init Built Function
	//
	//////////////////////////////////////////////////////////////////////////

	class CMakeCl2AndZpkFromChangedZone : public CCoreToolFunctor
	{
	public:
		CMakeCl2AndZpkFromChangedZone(CControlMediator* pMediator)
		{
			m_pControlMediator_ = pMediator;
		}

		std::string GetMD5CheckSum(const std::string& _rFileName)
		{
			MFile file;
			if ( !file.Open(_rFileName.c_str()))
				return std::string("");

			// read file stream
			UINT fileSize = file.GetLength();
			char *pBuffer = new char[fileSize];
			if ( file.Read((void*)pBuffer, fileSize) == false 
				|| fileSize == 0)
			{
				delete [] pBuffer;
				file.Close();

				return std::string("");
			}

			MMD5 md5;
			char pcOutMD5String[16];
			md5.md5_string((unsigned char*)pBuffer, fileSize, (unsigned char*)pcOutMD5String);

			delete[] pBuffer;
			file.Close();

			return std::string(pcOutMD5String);
		}

		bool RunExec(const char* _pSzCmd)
		{
			STARTUPINFO si;
			PROCESS_INFORMATION pi;
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);
			ZeroMemory( &pi, sizeof(pi) );

			// Start the child process. 
			if( !CreateProcess( NULL,	// No module name (use command line)
				(LPSTR)(LPCSTR)_pSzCmd,	// Command line
				NULL,					// Process handle not inheritable
				NULL,					// Thread handle not inheritable
				FALSE,					// Set handle inheritance to FALSE
				0,						// No creation flags
				NULL,					// Use parent's environment block
				NULL,					// Use parent's starting directory 
				&si,					// Pointer to STARTUPINFO structure
				&pi )					// Pointer to PROCESS_INFORMATION structure
				) 
			{
				mlog("Fail running SVN\n");
				return false;
			}

			// Wait until child process exits.
			WaitForSingleObject( pi.hProcess, INFINITE );

			// Close process and thread handles. 
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
			return true;
		}

		virtual const char* GetFuncName(){ return "make_cl2_and_zpk_from_changed_zone"; }
		virtual bool Run()
		{
			// get zone files from server folder
			WIN32_FIND_DATA ffd;
			HANDLE hFind = INVALID_HANDLE_VALUE;

			std::vector<std::string> vecZoneForMakeCl2;
			hFind = FindFirstFile( (CXMLConfig::GetInstance()->strServerZoneFullPath+"*").c_str(), &ffd);
			do
			{
				if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
				}
				else
				{
					std::string strServerZoneFile(ffd.cFileName);
					if (strServerZoneFile.find(".zone.xml") == std::string::npos)
						continue;

					std::string strServerZoneFileFullPath = CXMLConfig::GetInstance()->strServerZoneFullPath + strServerZoneFile;
					std::string server_file_md5 = GetMD5CheckSum(strServerZoneFileFullPath);

					if (server_file_md5.empty())
					{
						mlog("Fail to check sum : %s\n", strServerZoneFileFullPath.c_str());
						continue;
					}

					std::string strClientZoneFileFullPath =  CXMLConfig::GetInstance()->strDataFullPath + "Zone\\" + MGetPureFileName(MGetPureFileName(strServerZoneFile)) + "\\" + strServerZoneFile;
					std::string client_file_md5 = GetMD5CheckSum(strClientZoneFileFullPath);

					if (server_file_md5 != client_file_md5)
					{
						vecZoneForMakeCl2.push_back(strClientZoneFileFullPath);
					}
				}
			}
			while (FindNextFile(hFind, &ffd) != 0);
			FindClose(hFind);

			for (std::vector<std::string>::iterator itr = vecZoneForMakeCl2.begin(); itr != vecZoneForMakeCl2.end(); ++itr)
			{
				if(m_pControlMediator_->LoadZone(itr->c_str(), MAP_EDIT_TYPE_LOGIC, false) == MAP_EDIT_TYPE_ERROR)
				{
					mlog("Fail reading Zone File : %s\n", itr->c_str());
					continue;
				}

				// create cl2 and zpk, copy related files
				m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>()->CreateCollisionTreeBuild();

				// clear resource
				m_pControlMediator_->ResetResource();
				m_pControlMediator_->GetHelperResource()->DeleteTempFile();
			}

			MAP_PARAM::iterator itr_param = m_mapParams_.find("svn_commit");
			if (itr_param != m_mapParams_.end() && itr_param->second == "true")
			{
				if (!RunExec("svn add -N -q ..\\GameServer\\*.cl2"))
					return false;

				if (!RunExec("svn add -N -q ..\\GameServer\\*.zpk"))
					return false;

				if (!RunExec("svn add -N -q ..\\GameServer\\*.pathengine"))
					return false;

				if (!RunExec("svn add -N -q ..\\GameServer\\*.dds"))
					return false;

				if (!RunExec("svn commit -m \"ReCreate cl2 from Zone File\" ..\\GameServer"))
					return false;

				if (!RunExec("svn commit -m \"Sync Client Zone File and Server Zone File\" ..\\..\\Data\\Zone"))
					return false;
			}

			return true;
		}

	private:
		CControlMediator* m_pControlMediator_;
	};

	CRubyManager::GetReference().AddManagedScriptFunctor(new CMakeCl2AndZpkFromChangedZone(m_pControlMediator_));

	// helpers and actions
	_rHelperAndAction.m_pDoActionObjectMove = new CDoActionObjectMoveImpl;
	_rHelperAndAction.m_pDoActionObjectNew = new CDoActionObjectNewImpl;
	_rHelperAndAction.m_pDoActionObjectRotate = new CDoActionObjectRotateImpl;
	_rHelperAndAction.m_pDoActionObjectScale = new CDoActionObjectScaleImpl;
	_rHelperAndAction.m_pDoActionObjectSelect = new CDoActionObjectSelectImpl;
	_rHelperAndAction.m_pDoActionEtcPathSelect = new CDoActionEtcPathSelectImpl;

	_rHelperAndAction.m_pEntityActionHelper = new CControlMediatorHelperEntityActionImpl(m_pControlMediator_);
	_rHelperAndAction.m_pObjectRenderHelper = new CControlMediatorHelperObjectRendererImpl;
	_rHelperAndAction.m_pNewMapResourceHelper = new CControlMediatorHelperNewMapResourceImpl(m_pControlMediator_, CControlMediatorHelperNewMapResourceImpl::E_FIELD);

	_rHelperAndAction.m_pPopupActionRepository = new CControlPopupActionRepository;
	class CFieldMinimapNewFunctor : public CCoreToolFunctor
	{
	public:
		CFieldMinimapNewFunctor(CControlMediator* _pControlMediator){ m_pControlMediator_ = _pControlMediator; }
		virtual ~CFieldMinimapNewFunctor(){}

		virtual bool Run()
		{
			CDlgNewMiniMap* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgNewMiniMap>();
			if (pDlg)
			{
				if ( pDlg->CreateMiniMap( m_pControlMediator_ ) )
					AfxMessageBox("미니맵이 Runtime/Data/Interface/minimap 에 생성되었습니다.", MB_ICONINFORMATION | MB_OK);
				else
					AfxMessageBox("미니맵이 생성에 실패했습니다.", MB_ICONWARNING | MB_OK);

				delete pDlg;
			}
			return true;
		}

		virtual const char* GetFuncName()
		{
			return "필드 미니맵 생성";
		}

	private:
		CControlMediator* m_pControlMediator_;

	};
	_rHelperAndAction.m_pPopupActionRepository->AddManagedResNewFunctor(new CFieldMinimapNewFunctor(m_pControlMediator_));

	CRunGameFunctor* pRunGameFunctor = new CRunGameFunctor(m_pControlMediator_);
	_rHelperAndAction.m_pPopupActionRepository->AddManagedToolEtcFunctor(pRunGameFunctor);

	_rHelperAndAction.m_pKeyEventHandlerHelper = new CSoulHuntKeyboardHelper(pRunGameFunctor);
}

void CWorkSpaceField::HaltWorkSpace( CWorkSpaceHelpersAndActions& _rHelperAndAction )
{
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectMove);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectNew);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectRotate);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectScale);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionObjectSelect);
	SAFE_DELETE(_rHelperAndAction.m_pDoActionEtcPathSelect);

	SAFE_DELETE(_rHelperAndAction.m_pEntityActionHelper);
	SAFE_DELETE(_rHelperAndAction.m_pNewMapResourceHelper);
	SAFE_DELETE(_rHelperAndAction.m_pObjectRenderHelper);
	SAFE_DELETE(_rHelperAndAction.m_pKeyEventHandlerHelper);

	SAFE_DELETE(_rHelperAndAction.m_pPopupActionRepository);
}

void CWorkSpaceField::OnWBObjectEditMouseMessage(VIEW_MESSAGE::ID message)
{
	m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->Listen( CoreMessage(message) );
}

MAP_EDIT_TYPE CWorkSpaceField::OnOpenFile( const char* _pSzFileName )
{
	m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->SetEditingModeReadOnly(false, true);
	return LoadField_(_pSzFileName, true, true);
}

std::string CWorkSpaceField::OnSaveFile(const char* _pSzFileName, bool bSaveAs)
{
	CStatusBarHandler::GetInstance()->ShowStatusBarMessage("필드 저장중");

	std::string strFileName = m_fieldInfo.m_strFieldPath + m_fieldInfo.m_strFieldFile;

	SaveFieldXML_(strFileName.c_str());
	SaveCommonEntityXML(strFileName.c_str(), false);
	m_pControlMediator_->GetHelperResource()->ManageConfigFileRelatedMap(strFileName.c_str(), CControlMediatorHelperResource::MAP_CONFIG_SAVE, true);

	::CopyFile(strFileName.c_str(), (CXMLConfig::GetInstance()->strServerFieldFullPath + m_fieldInfo.m_strFieldFile).c_str() , FALSE );
	return strFileName;
}

void CWorkSpaceField::OnBetweenCoreToolUpdateAndRS3Render()
{
}

void CWorkSpaceField::OnResetResourceAfterSceneCleared(bool bOnDestroy)
{
	InitWeatherInfo_();

	m_vecRenderSceneNodes.clear();
}

bool CWorkSpaceField::LoadFieldXML_(const char* _pSzName, const char* _pSzLoadAs /* = NULL */)
{
	// init entity object repository
	CToolLogicObjectManager* pLogicManager = m_pControlMediator_->GetToolLogicObjectMgr();
	if (pLogicManager->GetEntityObjectRepository().empty())
	{
		pLogicManager->InitLogicObjectManager<CEntityObject>();
	}

	MXml kXml;
	if ( !kXml.LoadFile(_pSzName))
	{
		m_pControlMediator_->GetWorkSpaceMgr()->GetWorkSpaceErrorMessage().push_back("필드 XML 파일을 열수 없습니다.");
		return false;
	}

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("maiet");
	if(pRootElement == NULL)
	{
		m_pControlMediator_->GetWorkSpaceMgr()->GetWorkSpaceErrorMessage().push_back("잘못된 필드 XML 파일 - maiet 노드 없음");
		return false;
	}
	MXmlElement *pFieldInfo = pRootElement->FirstChildElement("FIELDINFO");
	if ( pFieldInfo == NULL )
	{
		m_pControlMediator_->GetWorkSpaceMgr()->GetWorkSpaceErrorMessage().push_back("잘못된 필드 XML 파일 - FIELDINFO 노드 없음");
		return false;
	}

	m_fieldInfo.Clear();

	//set field info from file name
	bool bLoadZoneConfig = true;
	if(_pSzLoadAs != NULL)
	{
		std::string fullPath(_pSzLoadAs);
		int find_token = fullPath.rfind("\\") + 1;
		m_fieldInfo.m_strFieldPath = fullPath.substr(0, find_token);
		m_fieldInfo.m_strFieldFile = fullPath.substr(find_token, fullPath.length() - find_token);
		bLoadZoneConfig = false;
	}
	else
	{
		std::string fullPath(_pSzName);
		int find_token = fullPath.rfind("\\") + 1;
		m_fieldInfo.m_strFieldPath = fullPath.substr(0, find_token);
		m_fieldInfo.m_strFieldFile = fullPath.substr(find_token, fullPath.length() - find_token);
	}

	//set field info from xml
	std::string strInstance;
	_Attribute(&m_fieldInfo.m_nReferZone, pFieldInfo, "zone");
	_Attribute(&m_fieldInfo.m_nMinX , pFieldInfo, "min_x");
	_Attribute(&m_fieldInfo.m_nMinY , pFieldInfo, "min_y");
	_Attribute(&m_fieldInfo.m_nMaxX , pFieldInfo, "max_x");
	_Attribute(&m_fieldInfo.m_nMaxY , pFieldInfo, "max_y");
	_Attribute(&m_fieldInfo.m_rtBoundary.left , pFieldInfo, "boundary_min_x");
	_Attribute(&m_fieldInfo.m_rtBoundary.top , pFieldInfo, "boundary_min_y");
	_Attribute(&m_fieldInfo.m_rtBoundary.right , pFieldInfo, "boundary_max_x");
	_Attribute(&m_fieldInfo.m_rtBoundary.bottom , pFieldInfo, "boundary_max_y");
	_Attribute(&m_fieldInfo.m_bWeatherEffect , pFieldInfo, "weather_effect");
	_Attribute(m_fieldInfo.m_bTownField , pFieldInfo, "town");
	_Attribute(m_fieldInfo.m_bReturnable , pFieldInfo, "returnable");
	_Attribute(m_fieldInfo.m_bTutorial , pFieldInfo, "tutorial");
	_Attribute(&m_fieldInfo.m_nMaxGatherMine , pFieldInfo, "max_gather_mine");
	_Attribute(&m_fieldInfo.m_nMaxGatherHerb , pFieldInfo, "max_gather_herb");
	_Attribute(&m_fieldInfo.m_nMaxGatherRelic , pFieldInfo, "max_gather_relic");
	_Attribute(&m_fieldInfo.m_nMaxGatherLumber , pFieldInfo, "max_gather_lumber");

	_Attribute(&m_fieldInfo.m_nQPVPNPC1 , pFieldInfo, "qpvpnpc1");
	_Attribute(&m_fieldInfo.m_nQPVPNPC2 , pFieldInfo, "qpvpnpc2");

	_Attribute(&m_fieldInfo.m_nQPVPRebirth1 , pFieldInfo, "qpvprebirth1");
	_Attribute(&m_fieldInfo.m_nQPVPRebirth2 , pFieldInfo, "qpvprebirth2");

	_Attribute(&m_fieldInfo.m_fSectorHeight, pFieldInfo, "sector_height");


	string strValue;
	if (true == _Attribute(strValue, pFieldInfo, "fixed_time"))
	{
		for (int i = 0; i < MAX_TIME_TYPE; i++)
		{
			if (!_stricmp(strValue.c_str(), GameTime(GAME_TIME_TYPE(i))))
			{
				m_fieldInfo.m_TimeInfo.bFixed = true;
				m_fieldInfo.m_TimeInfo.nFixedTime = GAME_TIME_TYPE(i);
				break;
			}
		}
	}

	if (!CXMLResourceList::GetReference().MakeZoneIdFileNameMap())
		return false;

	if( !CXMLResourceList::GetReference().GetZoneFileNameFromId(m_fieldInfo.m_nReferZone, m_strZoneName_) )
		return false;

	//씬 리소스를 로딩
	if(m_pControlMediator_->LoadZone(m_strZoneName_.c_str(), MAP_EDIT_TYPE_LOGIC, bLoadZoneConfig) == MAP_EDIT_TYPE_ERROR)
		return false;

	//존 리소스를 로딩
	CWorkSpaceZone* pZoneWS = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceZone>();
	if (pZoneWS)
		pZoneWS->LoadZoneCommonXmlAndNavigationMesh(m_strZoneName_.c_str());

	//로딩된 존 리소스로부터 필드 리소스 Set
	SetFieldResourceWithLoadedZone_();

	// 로딩된 존 리소스 수정 불가로 변경
	SetEditableLoadedZone_();
	
	//Field XML로 부터 Entity 로딩
	MXmlElement* pChildElement = pFieldInfo->FirstChildElement();
	while(pChildElement != NULL)
	{
		const char *pValue = pChildElement->Value();

		if ( _stricmp(pValue,"BGM")==0 )
		{
			_Attribute(m_fieldInfo.m_strBGM[0]	, pChildElement, BGM_XML_TAG[0], "");
			MXmlElement* pEnvBGMElement = pChildElement->FirstChildElement("ENVBGM");
			while (pEnvBGMElement != NULL)
			{
				for (int i=1; i<BGM_MAX; i++ )
				{
					if(m_fieldInfo.m_strBGM[i].length() == 0)
					{
						bool bRet = _Attribute(m_fieldInfo.m_strBGM[i], pEnvBGMElement, BGM_XML_TAG[i], "");
						if (bRet) break;
					}
				}

				pEnvBGMElement = pEnvBGMElement->NextSiblingElement();
			}
		}

		else if ( _stricmp(pValue,"FLOORS")==0 )
		{
			for ( MXmlElement* pFloor = pChildElement->FirstChildElement();  pFloor != NULL;  pFloor = pFloor->NextSiblingElement())
			{
				if ( _stricmp( pFloor->Value(), "FLOOR") == 0)
				{
					int nFloor;
					_Attribute( &nFloor, pFloor, "height", 0);

					m_fieldInfo.m_vFloors.push_back( nFloor);
				}
			}
		}

		else if ( _stricmp(pValue,"SOUNDREVERB") == 0)
		{
			_Attribute( &m_fieldInfo.m_nSoundReverbType, pChildElement, "reverb", 0);
		}

		//Env, PORTAL, NPC, NPC_AREA
		else if ( _stricmp(pValue,"Env")==0 )
		{
			std::string strEnvSetName;
			_Attribute(strEnvSetName, pChildElement, "set");
			if(strEnvSetName.empty())
			{
				strEnvSetName = "default";
			}

			//Set Environment
			m_pControlMediator_->GetViewPtr<CRollupEntityEnvironment>()->SetEnvironmentSet(strEnvSetName.c_str(), true);

			MXmlElement* pWeatherElement = pChildElement->FirstChildElement("WEATHER");
			if(pWeatherElement == NULL)
			{
				m_pControlMediator_->GetWorkSpaceMgr()->GetWorkSpaceErrorMessage().push_back("잘못된 필드 XML 파일 - Weather 정보 없음");
			}
			else
			{
				std::string strDefault(pWeatherElement->Attribute("default"));
				m_weatherNamePercent_[strDefault] = WEATHER_DEFAULT;

				MXmlElement* pAddElement = pWeatherElement->FirstChildElement("ADD");
				while(pAddElement != NULL)
				{
					std::string strWeather(pAddElement->Attribute("weather"));
					int weather_per;
					pAddElement->Attribute("percent", &weather_per);
					m_weatherNamePercent_[strWeather] = weather_per;
					pAddElement = pAddElement->NextSiblingElement();
				}
			}
		}
		else if ( _stricmp(pValue,"PVPAREA")==0 )
		{
			PVP_AREA_INFO infoPVP;
			_Attribute(&infoPVP.rcArea.left, pChildElement, "left");
			_Attribute(&infoPVP.rcArea.top, pChildElement, "top");
			_Attribute(&infoPVP.rcArea.right, pChildElement, "right");
			_Attribute(&infoPVP.rcArea.bottom, pChildElement, "bottom");
			_Attribute(&infoPVP.nFaction1, pChildElement, "faction1");
			_Attribute(&infoPVP.nFaction2, pChildElement, "faction2");

			_Attribute(infoPVP.strFactionName1, pChildElement, "faction_name1");
			_Attribute(infoPVP.strFactionName2, pChildElement, "faction_name2");

			m_fieldInfo.m_PvPAreas.push_back(infoPVP);
		}
		else if ( _stricmp(pValue,"PRELOAD")==0 )
		{
			MXmlElement* pEluElement = pChildElement->FirstChildElement("ELU");
			while (pEluElement)
			{
				std::string name;
				_Attribute(name, pEluElement, "name");
				m_fieldInfo.m_FieldPreLoadList.push_back(name);
				pEluElement = pEluElement->NextSiblingElement("ELU");
			}
		}
		else
		{
			CEntityObject::TYPE type = CEntityObject::IdentifyTypeFromChar(pValue);
			if(type != CEntityObject::ENTITY_OBJ_NUM)
			{
				CEntityObject* pEntityObject = CEntityObject::CreateEntityObject(type);

				std::string strProperty(pValue);
				if( strProperty == "SENSOR")
				{
					if (pChildElement->FirstChildElement("WARP"))
					{
						strProperty += "_WARP";
					}
				}

				if (strProperty == "SPAWN")
				{
					std::string strCustomSpawn;
					if (_Attribute( strCustomSpawn, pChildElement, "gather"))
					{
						strProperty = "GATHER_OBJECT";
					}
				}

				pEntityObject->SetZoneLoad(false);
				pEntityObject->SetEditable(true);

				pEntityObject->LoadPropertyFromXML( m_pControlMediator_->GetToolLogicObjectMgr()->GetPropertyTable(strProperty.c_str()), pChildElement);
				pLogicManager->AddToManager(pEntityObject);
			}
			else
			{
				std::string strMessage("정의되지 않은 Element : ");
				m_pControlMediator_->GetWorkSpaceMgr()->GetWorkSpaceErrorMessage().push_back(strMessage + pValue);
			}
		}
		pChildElement = pChildElement->NextSiblingElement();
	}

	

	// field common.xml 파일 읽기
	std::string strFieldName(_pSzName);
	int nPos = strFieldName.find(".xml");
	if (nPos != std::string::npos )
	{
		strFieldName.replace(nPos, 11, ".common.xml");
		if(LoadCommonEntityXML(strFieldName.c_str(), false) == false)
			mlog("common.xml 을 읽어오지 못했습니다.\n");
	}

	return true;
}

void CWorkSpaceField::SaveFieldXML_(const char* _pSzName)
{
	MXml kXml;
	//MXmlDeclaration* pDecl = kXml.CreateDeclaration("1.0", "UTF-8", "");
	MXmlDeclaration* pDecl = kXml.CreateDeclaration("1.0", "euc-kr", "");
	kXml.Doc()->LinkEndChild(pDecl);

	MXmlElement *pRootElement = new MXmlElement("maiet");
	kXml.Doc()->LinkEndChild(pRootElement);

	MXmlElement* pElementFieldInfo = new MXmlElement( "FIELDINFO" );
	pRootElement->LinkEndChild(pElementFieldInfo);

	_SetAttribute(pElementFieldInfo, "zone", m_fieldInfo.m_nReferZone);
	_SetAttribute(pElementFieldInfo, "min_x", m_fieldInfo.m_nMinX);
	_SetAttribute(pElementFieldInfo, "min_y", m_fieldInfo.m_nMinY);
	_SetAttribute(pElementFieldInfo, "max_x", m_fieldInfo.m_nMaxX);
	_SetAttribute(pElementFieldInfo, "max_y", m_fieldInfo.m_nMaxY);
	_SetAttribute(pElementFieldInfo, "boundary_min_x", m_fieldInfo.m_rtBoundary.left);
	_SetAttribute(pElementFieldInfo, "boundary_min_y", m_fieldInfo.m_rtBoundary.top);
	_SetAttribute(pElementFieldInfo, "boundary_max_x", m_fieldInfo.m_rtBoundary.right);
	_SetAttribute(pElementFieldInfo, "boundary_max_y", m_fieldInfo.m_rtBoundary.bottom);

	_SetAttribute(pElementFieldInfo, "weather_effect", m_fieldInfo.m_bWeatherEffect);

	if (m_fieldInfo.m_TimeInfo.bFixed)
	{
		_SetAttribute(pElementFieldInfo, "fixed_time", GameTime(m_fieldInfo.m_TimeInfo.nFixedTime));
	}

	if(m_fieldInfo.m_bTownField)
		_SetAttribute(pElementFieldInfo, "town", "true");
	else
		_SetAttribute(pElementFieldInfo, "town", "false");

	if(m_fieldInfo.m_bReturnable)
		_SetAttribute(pElementFieldInfo, "returnable", "true");
	else
		_SetAttribute(pElementFieldInfo, "returnable", "false");

	if(m_fieldInfo.m_bTutorial)
		_SetAttribute(pElementFieldInfo, "tutorial", "true");
	else
		_SetAttribute(pElementFieldInfo, "tutorial", "false");

	if (m_fieldInfo.m_nMaxGatherMine > 0)
		_SetAttribute(pElementFieldInfo, "max_gather_mine", m_fieldInfo.m_nMaxGatherMine);
	if (m_fieldInfo.m_nMaxGatherHerb > 0)
		_SetAttribute(pElementFieldInfo, "max_gather_herb", m_fieldInfo.m_nMaxGatherHerb);
	if (m_fieldInfo.m_nMaxGatherRelic > 0)
		_SetAttribute(pElementFieldInfo, "max_gather_relic", m_fieldInfo.m_nMaxGatherRelic);
	if (m_fieldInfo.m_nMaxGatherLumber > 0)
		_SetAttribute(pElementFieldInfo, "max_gather_lumber", m_fieldInfo.m_nMaxGatherLumber);

	if (m_fieldInfo.m_nQPVPNPC1 > 0)
		_SetAttribute(pElementFieldInfo, "qpvpnpc1", m_fieldInfo.m_nQPVPNPC1);
	if (m_fieldInfo.m_nQPVPNPC2 > 0)
		_SetAttribute(pElementFieldInfo, "qpvpnpc2", m_fieldInfo.m_nQPVPNPC2);

	if (m_fieldInfo.m_nQPVPRebirth1 > 0)
		_SetAttribute(pElementFieldInfo, "qpvprebirth1", m_fieldInfo.m_nQPVPRebirth1);
	if (m_fieldInfo.m_nQPVPRebirth2 > 0)
		_SetAttribute(pElementFieldInfo, "qpvprebirth2", m_fieldInfo.m_nQPVPRebirth2);

	if (m_fieldInfo.m_fSectorHeight > 0.0f)
		_SetAttribute(pElementFieldInfo, "sector_height", m_fieldInfo.m_fSectorHeight);

	if ( m_fieldInfo.m_vFloors.empty() == false)
	{
		MXmlElement* pElementFloors = new MXmlElement( "FLOORS");
		pElementFieldInfo->LinkEndChild( pElementFloors);

		for ( vector< int>::iterator itr = m_fieldInfo.m_vFloors.begin();  itr != m_fieldInfo.m_vFloors.end();  itr++)
		{
			MXmlElement* pElementFloor = new MXmlElement( "FLOOR");
			pElementFloors->LinkEndChild( pElementFloor);
			_SetAttribute( pElementFloor, "height", (*itr));
		}
	}

	if (IsBGMEmpty() == false)
		SaveBGMXML(pElementFieldInfo);

	if ( m_fieldInfo.m_nSoundReverbType > 0)
	{
		MXmlElement* pElementReverb = new MXmlElement( "SOUNDREVERB");
		pElementFieldInfo->LinkEndChild( pElementReverb);
		_SetAttribute( pElementReverb, "reverb", m_fieldInfo.m_nSoundReverbType);
	}

	if ( false == m_fieldInfo.m_FieldPreLoadList.empty())
	{
		MXmlElement* pElementPreLoad = new MXmlElement( "PRELOAD");
		for (std::vector<std::string>::iterator itr = m_fieldInfo.m_FieldPreLoadList.begin();
			itr != m_fieldInfo.m_FieldPreLoadList.end(); ++itr)
		{
			MXmlElement* pElementElu = new MXmlElement( "ELU" );
			_SetAttribute(pElementElu, "name", itr->c_str());
			pElementPreLoad->LinkEndChild(pElementElu);
		}
		pElementFieldInfo->LinkEndChild( pElementPreLoad);
	}

	//////////////////////////
	//
	// Environment
	MXmlElement* pElementEnv = new MXmlElement( "Env" );
	pElementFieldInfo->LinkEndChild(pElementEnv);
	std::string strSetName;
	m_pControlMediator_->GetViewPtr<CRollupEntityEnvironment>()->GetCurrentEnvironmentSet(strSetName);
	_SetAttribute(pElementEnv, "set", strSetName);

	//weather
	MXmlElement* pElementWeather = new MXmlElement("WEATHER");
	pElementEnv->LinkEndChild(pElementWeather);
	std::string strDefault;
	typedef std::vector< std::pair< std::string, int > > STR_INT;
	STR_INT vecStrInt;
	for(int i = 0; i < WEATHER_NUM; ++i)
	{
		std::string strKey(m_weatherNameTable_[i]);
		int weatherPercent = m_weatherNamePercent_[strKey];
		if(weatherPercent == WEATHER_DEFAULT)
			strDefault = strKey;
		else
			vecStrInt.push_back( pair< std::string, int >( strKey, weatherPercent));
	}
	_SetAttribute(pElementWeather, "default", strDefault);
	int added_weather = WEATHER_NUM -1;
	ASSERT( added_weather == vecStrInt.size());
	for(int i_a = 0; i_a < added_weather; ++i_a)
	{
		MXmlElement* pElementWeatherAddProp = new MXmlElement("ADD");
		pElementWeather->LinkEndChild(pElementWeatherAddProp);
		_SetAttribute(pElementWeatherAddProp, "weather", vecStrInt[i_a].first);
		_SetAttribute(pElementWeatherAddProp, "percent", vecStrInt[i_a].second);
	}

	// PVPAREA
	for (size_t i = 0; i < m_fieldInfo.m_PvPAreas.size(); i++)
	{
		MXmlElement* pElementPvPArea = new MXmlElement( "PVPAREA" );
		pElementFieldInfo->LinkEndChild(pElementPvPArea);
		PVP_AREA_INFO& pvpAreaInfo = m_fieldInfo.m_PvPAreas[i];

		_SetAttribute(pElementPvPArea, "top", pvpAreaInfo.rcArea.top);
		_SetAttribute(pElementPvPArea, "left", pvpAreaInfo.rcArea.left);
		_SetAttribute(pElementPvPArea, "bottom", pvpAreaInfo.rcArea.bottom);
		_SetAttribute(pElementPvPArea, "right", pvpAreaInfo.rcArea.right);
		_SetAttribute(pElementPvPArea, "faction1", pvpAreaInfo.nFaction1);
		_SetAttribute(pElementPvPArea, "faction2", pvpAreaInfo.nFaction2);
		_SetAttribute(pElementPvPArea, "faction_name1", pvpAreaInfo.strFactionName1);
		_SetAttribute(pElementPvPArea, "faction_name2", pvpAreaInfo.strFactionName2);
	}

	//Entity Object에서 저장하기
	for(int i = 0; i < CEntityObject::ENTITY_OBJ_NUM; ++i)
	{
		ENTITY_OBJECTLIST* pList = m_pControlMediator_->GetToolLogicObjectMgr()->GetEntityObjectList<CEntityObject>((CEntityObject::TYPE)i);
		if (pList == NULL)
			continue;

		for(ENTITY_OBJECTLIST::iterator itr = pList->begin(); itr != pList->end(); ++itr)
		{
			(*itr)->SaveToXML(*pElementFieldInfo, false);
		}
	}

	kXml.SaveFile(_pSzName);
}

void CWorkSpaceField::SaveBGMXML( MXmlElement* pElementFieldInfo )
{
	// 배경음 저장
	MXmlElement* pElementBGM = new MXmlElement( "BGM" );
	pElementFieldInfo->LinkEndChild(pElementBGM);
	if ( m_fieldInfo.m_strBGM[0].length() > 0 )
		_SetAttribute(pElementBGM, BGM_XML_TAG[0], m_fieldInfo.m_strBGM[0]);

	// 환경음 저장
	for (int i=1; i<BGM_MAX; i++ )
	{
		if ( m_fieldInfo.m_strBGM[i].length() > 0 )
		{
			MXmlElement* pElementEnvBGM = new MXmlElement("ENVBGM");
			pElementBGM->LinkEndChild(pElementEnvBGM);
			_SetAttribute(pElementEnvBGM, BGM_XML_TAG[i], m_fieldInfo.m_strBGM[i]);
		}
	}
}

void CWorkSpaceField::SetFieldCreationInfo( int _zoneID, int _fieldID, const char* _pSzFieldName )
{
	m_nFromZoneID_ = _zoneID;
	m_nFromFieldID_ = _fieldID;

	std::string fullPath(_pSzFieldName);
	int find_token = fullPath.rfind("\\") + 1;
	m_fieldInfo.m_strFieldPath = fullPath.substr(0, find_token);
	m_fieldInfo.m_strFieldFile = fullPath.substr(find_token, fullPath.length() - find_token);
}

void CWorkSpaceField::CreateField()
{
	//////////////////////////////////////////////////////
	///
	/// zone_id / field_id 비교하여 field 생성
	///  a,  0 : 새로운 존으로부터 생성
	///  a, -1 : 기존 존으로 부터 생성
	/// -1,  a : 기존 필드로 부터 생성
	///
	/// zone_id / field_id 는 SetFieldCreationInfo에서 정해진다.

	//새로운 존으로 부터 생성
	if(m_nFromZoneID_ > 0 && m_nFromFieldID_ == 0)
	{
		//존 생성,  존 생성에 필요한 정보는 CDlgMapNew의 PutNewZoneInfoForCreation_에서 채워져 온다.
		//바꾸어야 할 존 정보
		m_pControlMediator_->CreateZone();
		m_pControlMediator_->SaveZone(NULL, true, false);
		m_pControlMediator_->BroadCastMessage(VIEW_MESSAGE::SCENE_LOADED);

		//생성 시점에서 존에 대한 정보는
		//m_pControlMediator->m_mapInfo 에 저장되어 있다.
		m_strZoneName_ = m_pControlMediator_->m_mapInfo.m_strScenePath + m_pControlMediator_->m_mapInfo.m_strSceneFile;
		SetCreatedFieldResourceWithLoadedZone_();
	}
	//기존 존으로 부터 생성
	else if(m_nFromZoneID_ > 0 && m_nFromFieldID_ == -1)
	{
		if (!CXMLResourceList::GetReference().MakeZoneIdFileNameMap())
		{
			AfxMessageBox("ZoneList.xml 파일이 잘못되어있습니다.");
			return;
		}

		if(CXMLResourceList::GetReference().GetZoneFileNameFromId(m_nFromZoneID_, m_strZoneName_) == false)
		{
			AfxMessageBox("존 ID 설정이 잘못되었습니다.");
			return;
		}

		if(m_pControlMediator_->LoadZone(m_strZoneName_.c_str(), MAP_EDIT_TYPE_LOGIC, false) == MAP_EDIT_TYPE_ERROR)
		{
			AfxMessageBox("기존의 존 정보가 올바르지 않습니다.");
			return;
		}

		SetCreatedFieldResourceWithLoadedZone_();
	}
	//기존 필드로부터 생성
	else if(m_nFromZoneID_ == -1 && m_nFromFieldID_ > 0)
	{
		//기존 필드 이름
		CString cstrFromFieldName;
		cstrFromFieldName.Format("%d.field.xml", m_nFromFieldID_);
		std::string strFromFieldName( m_fieldInfo.m_strFieldPath + cstrFromFieldName.GetString() );

		// 에디터 관련정보 로딩
		CXMLEditInfoFile::GetReference().LoadFieldInformationFile(strFromFieldName.c_str());

		std::string strNewFieldName(m_fieldInfo.m_strFieldPath + m_fieldInfo.m_strFieldFile);
		LoadFieldXML_( strFromFieldName.c_str(), strNewFieldName.c_str() );
	}
}

void CWorkSpaceField::SetWeatherValue(int _weatherKind, int _percent)
{
	if (_percent == WEATHER_DEFAULT)
	{
		for (W_NAME_PERCENT::iterator itr = m_weatherNamePercent_.begin(); itr != m_weatherNamePercent_.end(); ++itr)
		{
			if (itr->second == WEATHER_DEFAULT)
			{
				itr->second = 0;
			}
		}
	}
	m_weatherNamePercent_[m_weatherNameTable_[_weatherKind]] = _percent;
}

void CWorkSpaceField::GetWeatherValue(int _weatherKind, int& _refPercent)
{
	_refPercent = m_weatherNamePercent_[m_weatherNameTable_[_weatherKind]];
}

void CWorkSpaceField::SetFieldResourceWithLoadedZone_()
{
	//Load Property Table
	m_pControlMediator_->GetToolLogicObjectMgr()->LoadTablesSafely();

	//존에서 가져오는 field 정보, 존이 불린후에 field임을 설정한다.
	m_fieldInfo.m_nMinX = (int)m_pControlMediator_->m_mapInfo.m_orgX;
	m_fieldInfo.m_nMaxX = (int)m_pControlMediator_->m_mapInfo.m_width + (int)m_pControlMediator_->m_mapInfo.m_orgX;

	m_fieldInfo.m_nMinY = (int)m_pControlMediator_->m_mapInfo.m_orgY;
	m_fieldInfo.m_nMaxY = (int)m_pControlMediator_->m_mapInfo.m_height + (int)m_pControlMediator_->m_mapInfo.m_orgY;

	RoundOffFieldMinMax(m_fieldInfo.m_nMinX);
	RoundOffFieldMinMax(m_fieldInfo.m_nMinY);
	RoundOffFieldMinMax(m_fieldInfo.m_nMaxX);
	RoundOffFieldMinMax(m_fieldInfo.m_nMaxY);

	//존 리소스로부터 EnvSet Load
	std::vector<std::string> vecEnvSet;
	m_pControlMediator_->GetViewPtr<CRollupEnvWeather>()->GetEnvSetName(vecEnvSet);

	//set Entity Environment and Script
	int field_id = atoi( m_fieldInfo.m_strFieldFile.substr(0, 4).c_str() );
	m_pControlMediator_->GetViewPtr<CRollupEntityEnvironment>()->ResetEntityEnvironment(vecEnvSet);
	m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>()->ResetEntityScript(field_id);
}

void CWorkSpaceField::SetCreatedFieldResourceWithLoadedZone_()
{
	m_fieldInfo.m_nReferZone = m_nFromZoneID_;
	m_fieldInfo.m_bTownField = false;
	m_fieldInfo.m_bReturnable = true;

	SetFieldResourceWithLoadedZone_();
	m_pControlMediator_->GetViewPtr<CRollupEntityEnvironment>()->SetEnvironmentSet("default", true);

	//Load 정보 초기화
	CXMLEditInfoFile::GetReference().LoadFieldInformationFile(NULL);
}

void CWorkSpaceField::InitWeatherInfo_()
{
	m_weatherNamePercent_["SUN"] = WEATHER_DEFAULT;
	m_weatherNamePercent_["CLD"] = 10;
	m_weatherNamePercent_["RNY"] = 10;
	m_weatherNamePercent_["HRN"] = 10;
	m_weatherNamePercent_["SNW"] = 10;
	m_weatherNamePercent_["HSW"] = 10;
	m_weatherNamePercent_["CLR"] = 10;
}

MAP_EDIT_TYPE CWorkSpaceField::LoadField_( const char* _pSzFileName, bool _bLoadMapConfig, bool _bBroadCastLogicLoadMessage )
{
	if(LoadFieldXML_(_pSzFileName) == false)
	{
		m_pControlMediator_->m_mapInfo.m_eMapEditType = MAP_EDIT_TYPE_ERROR;
		return m_pControlMediator_->m_mapInfo.m_eMapEditType;
	}

	//config 파일 로딩
	if (_bLoadMapConfig)
		m_pControlMediator_->GetHelperResource()->ManageConfigFileRelatedMap(_pSzFileName, CControlMediatorHelperResource::MAP_CONFIG_LOAD, true);

	//필드가 로딩되었다고 view에게 알리기
	if (_bBroadCastLogicLoadMessage)
		m_pControlMediator_->BroadCastMessage(VIEW_MESSAGE::LOGIC_LOADED);

	return m_pControlMediator_->m_mapInfo.m_eMapEditType;
}

bool CWorkSpaceField::LoadField( int _nFieldId )
{
	char szFileName[256]= "";
	sprintf_s(szFileName, "%sfield/%d.field.xml", CXMLConfig::GetInstance()->pcDataPath, _nFieldId);

	return (MAP_EDIT_TYPE_LOGIC == LoadField_(szFileName, false, false));
}

void CWorkSpaceField::RoundOffFieldMinMax( int& _nValue )
{
	if (_nValue < 0)
	{
		int nUnitMin = -1000;
		while(true)
		{
			if (nUnitMin <= _nValue)
			{
				_nValue = nUnitMin;
				break;
			}
			else
			{
				nUnitMin = nUnitMin -1000;
			}
		}
	}
	else if (_nValue > 0)
	{
		int nUnitMax = 1000;
		while(true)
		{
			if (nUnitMax >= _nValue)
			{
				_nValue = nUnitMax;
				break;
			}
			else
			{
				nUnitMax = nUnitMax + 1000;
			}
		}
	}
}

void CWorkSpaceField::OnCreateWorkspaceFile()
{
	CreateField();

	//맵 관련 에디트 정보
	std::string fullPath(m_fieldInfo.m_strFieldPath + m_fieldInfo.m_strFieldFile);
	CXMLEditInfoFile::GetReference().SaveFieldInformationFile(fullPath.c_str(), true);
}

const TCHAR* SERVERZONE_XML_TAG_ZONE			= _T("ZONE");
const TCHAR* SERVERZONE_XML_TAG_MAP_SIZE		= _T("MAP_SIZE");
const TCHAR* SERVERZONE_XML_TAG_TERRAIN_SIZE	= _T("TERRAIN_SIZE");

class CZoneServerFileSaver
{
private:
	struct _ZoneInfo
	{
		vec3		m_vMapSize;
		vec3		m_vTerrainSize;

		_ZoneInfo() : m_vMapSize(vec3::ZERO), m_vTerrainSize(vec3::ZERO) {}
	};

	bool _Save( _ZoneInfo& zoneInfo, const TCHAR* szZoneFileName )
	{
		USES_CONVERSION_EX;

		MXml xml;
		MXmlDocument* root = xml.Doc();

		TCHAR szValue[512];

		MXmlDeclaration* pDec = xml.CreateDeclaration("1.0", "UTF-8", "");
		root->LinkEndChild(pDec);

		MXmlElement* pZoneElement = new MXmlElement(SERVERZONE_XML_TAG_ZONE);

		MXmlElement* pMapSizeElement = new MXmlElement(SERVERZONE_XML_TAG_MAP_SIZE);
		pZoneElement->LinkEndChild(pMapSizeElement);
		_stprintf_s(szValue, _T("%f %f %f"), zoneInfo.m_vMapSize.x, zoneInfo.m_vMapSize.y, zoneInfo.m_vMapSize.z);
		_SetContents(pMapSizeElement, T2CA_EX(szValue, 0));

		MXmlElement* pTerrainSizeElement = new MXmlElement(SERVERZONE_XML_TAG_TERRAIN_SIZE);
		pZoneElement->LinkEndChild(pTerrainSizeElement);
		_stprintf_s(szValue, _T("%f %f %f"), zoneInfo.m_vTerrainSize.x, zoneInfo.m_vTerrainSize.y, zoneInfo.m_vTerrainSize.z);
		_SetContents(pTerrainSizeElement, T2CA_EX(szValue, 0));

		root->LinkEndChild(pZoneElement);

		return xml.SaveFile(T2CA_EX(szZoneFileName, 0));
	}
public:
	bool Save(CControlMediator::MAP_INFO& mapInfo, rs3::RToolSceneManager* pSceneManager, std::string strZoneFileName)
	{
		_ZoneInfo zoneInfo;
		zoneInfo.m_vMapSize.x = mapInfo.m_width;
		zoneInfo.m_vMapSize.y = mapInfo.m_height;
		zoneInfo.m_vMapSize.z = 0.0f;

		if (pSceneManager->GetCurrentTerrain())
		{
			zoneInfo.m_vTerrainSize.x = mapInfo.m_width;
			zoneInfo.m_vTerrainSize.y = mapInfo.m_height;
			zoneInfo.m_vTerrainSize.z = 0.0f;
		}

		return _Save(zoneInfo, strZoneFileName.c_str());
	}

};

void CWorkSpaceField::CreateCollisionTreeBuild()
{
	CString cl2FileNameFullPath(( m_pControlMediator_->m_mapInfo.m_strScenePath).c_str());
	CString zoneFileNameFullPath(cl2FileNameFullPath);
	CString justZoneIdFullPath(cl2FileNameFullPath);

	CString fileName((m_pControlMediator_->m_mapInfo.m_strSceneFile).c_str());
	zoneFileNameFullPath += fileName;

	fileName.Replace(".zone.xml", "");
	justZoneIdFullPath = justZoneIdFullPath + fileName;
	cl2FileNameFullPath = cl2FileNameFullPath + fileName + ".cl2";

	// 클라이언트용
	bool bSaveOK = m_pControlMediator_->GetViewPtr<CRollupEtcCollision>()->CreateCl2(cl2FileNameFullPath.GetString());

	// 클라이언트의 cl2파일을 서버에도 복사한다.
	if (bSaveOK)
	{
		// cl2.xml 파일은 서버에 넣지 않는다.
		CControlMediatorHelperResource::CopyToServerZoneFolder(cl2FileNameFullPath);

		//////////////////////////////////////////////////////////////////////////
		//
		// cl2 관련파일 서버폴더에 복사
		//
		//////////////////////////////////////////////////////////////////////////

		// zone 파일
		std::string strZoneFileName = justZoneIdFullPath + ".zone.xml";
		CControlMediatorHelperResource::CopyToServerZoneFolder(strZoneFileName.c_str());

		// TODO: 기룡씨 고쳐주세용.
		strZoneFileName = justZoneIdFullPath + ".server.zone.xml";
		std::string strServerZoneFilename = CControlMediatorHelperResource::MakeServerZoneFile(strZoneFileName.c_str());
		CZoneServerFileSaver zoneServerFileSaver;
		zoneServerFileSaver.Save(m_pControlMediator_->m_mapInfo, m_pControlMediator_->GetToolSceneManager(), strServerZoneFilename);

		// dds 파일
		std::string strDDSFileName = justZoneIdFullPath + "_dis.dds";
		CControlMediatorHelperResource::CopyToServerZoneFolder(strDDSFileName.c_str());

		// navigation 관련
		string strNaviFileName = justZoneIdFullPath + ".navmesh.pathengine";
		CControlMediatorHelperResource::CopyToServerZoneFolder(strNaviFileName.c_str());
		strNaviFileName = justZoneIdFullPath + ".small.collision.pathengine";
		CControlMediatorHelperResource::CopyToServerZoneFolder(strNaviFileName.c_str());
		strNaviFileName = justZoneIdFullPath + ".small.pathfind.pathengine";
		CControlMediatorHelperResource::CopyToServerZoneFolder(strNaviFileName.c_str());
	}

	CStatusBarHandler::GetInstance()->ShowStatusBarMessage("충돌메시 완료");
}


bool CWorkSpaceField::LoadCommonEntityXML(const char* _pSzName, bool bZoneMode)
{
	// init entity object repository
	CToolLogicObjectManager* pLogicManager = m_pControlMediator_->GetToolLogicObjectMgr();
	if (pLogicManager->GetEntityObjectRepository().empty())
	{
		pLogicManager->InitLogicObjectManager<CEntityObject>();
	}

	MXml kXml;
	if (!kXml.LoadFile(_pSzName))
		return false;

	// Load Entity
	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("maiet");
	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement();
		pChildElement != NULL; pChildElement = pChildElement->NextSiblingElement())
	{
		const char *pValue = pChildElement->Value();
		_ASSERT(pValue);

		CEntityObject::TYPE type = CEntityObject::IdentifyTypeFromChar(pValue);
		if(type != CEntityObject::ENTITY_OBJ_NUM)
		{
			CEntityObject* pEntityObject = CEntityObject::CreateEntityObject(type);
			SElementAtXML *pElementAtXML = pLogicManager->GetPropertyTable(pValue);
			if ( pElementAtXML->m_strSubInfo != "common" )	// Un-common 개체는 여기서 읽지 않는다.
				continue;

			pEntityObject->SetZoneLoad(bZoneMode);
			pEntityObject->LoadPropertyFromXML(pElementAtXML, pChildElement);
			pLogicManager->AddToManager(pEntityObject);
		}
		else
		{
			mlog("정의되지 않은 Element : %s\n", pValue);
		}
	}

	// 기존에 읽은 Entity 들은 전부 에디트 하지 못하게끔 .
	std::vector<ENTITY_OBJECTLIST>& entityRepo = pLogicManager->GetEntityObjectRepository();
	int nEntityRepoSize = (int)entityRepo.size();
	for(int i = 0; i < nEntityRepoSize; ++i)
	{
		for(ENTITY_OBJECTLIST::iterator itr = entityRepo[i].begin(); itr != entityRepo[i].end(); ++itr)
		{
			CEntityObject *pEntityObject = (*itr);
			if ( pEntityObject->GetZoneLoad() == bZoneMode )
				pEntityObject->SetEditable(true);
			else
				pEntityObject->SetEditable(false);
		}
	}

	return true;
}

void CWorkSpaceField::SaveCommonEntityXML(const char* _pSzName, bool bZoneMode)
{
	MXml kXml;
	MXmlDeclaration* pDecl = kXml.CreateDeclaration("1.0", "euc-kr", "");
	kXml.Doc()->LinkEndChild(pDecl);

	MXmlElement *pRootElement = new MXmlElement("maiet");
	kXml.Doc()->LinkEndChild(pRootElement);

	//Entity Object에서 저장하기
	CToolLogicObjectManager* pLogicManager = m_pControlMediator_->GetToolLogicObjectMgr();
	std::vector<ENTITY_OBJECTLIST>& entityRepo = pLogicManager->GetEntityObjectRepository();
	int nEntityRepoSize = (int)entityRepo.size();
	for(int i = 0; i < nEntityRepoSize; ++i)
	{
		for(ENTITY_OBJECTLIST::iterator itr = entityRepo[i].begin(); itr != entityRepo[i].end(); ++itr)
		{
			CEntityObject *pEntityObject = *itr;
			if ( pEntityObject->GetZoneLoad() == bZoneMode )
				pEntityObject->SaveToXML(*pRootElement, true);
		}
	}

	const char *EXT_NAME = ".common.xml";

	std::string strFilename = _pSzName;
	if ( bZoneMode )
	{
		int npos = strFilename.find(".zone.xml");
		if ( npos != string::npos )
		{
			strFilename.replace(npos, strlen(EXT_NAME), EXT_NAME);
			kXml.SaveFile(strFilename.c_str());
		}
	}
	else
	{
		size_t npos = strFilename.find(".xml");
		if ( npos != string::npos )
		{
			strFilename.replace(npos, strlen(EXT_NAME), EXT_NAME);
			kXml.SaveFile(strFilename.c_str());
		}
	}
}

bool CWorkSpaceField::IsBGMEmpty()
{
	for (int i = 0; i< BGM_MAX; i++)
	{
		if(!m_fieldInfo.m_strBGM[i].empty())
		{
			return false;
		}
	}

	return true;
}

// 필드값이 아닌 디폴트 값 로딩용
void CWorkSpaceField::LoadBGMXML( const char* _pSzFileName )
{
	MXml kXml;
	if ( !kXml.LoadFile(_pSzFileName))
	{
		return;
	}

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("maiet");
	if (pRootElement == NULL) return;

	MXmlElement* pChildElement = pRootElement->FirstChildElement();
	while(pChildElement != NULL)
	{
		const char *pValue = pChildElement->Value();

		if ( _stricmp(pValue,"BGM")==0 )
		{
			_Attribute(m_fieldInfo.m_strBGM[0]	, pChildElement, BGM_XML_TAG[0], "");
			MXmlElement* pEnvBGMElement = pChildElement->FirstChildElement("ENVBGM");
			while (pEnvBGMElement != NULL)
			{
				for (int i=1; i<BGM_MAX; i++ )
				{
					if(m_fieldInfo.m_strBGM[i].length() == 0)
					{
						bool bRet = _Attribute(m_fieldInfo.m_strBGM[i], pEnvBGMElement, BGM_XML_TAG[i], "");
						if (bRet) break;
					}
				}

				pEnvBGMElement = pEnvBGMElement->NextSiblingElement();
			}
		}
		pChildElement = pChildElement->NextSiblingElement();
	}
}

void CWorkSpaceField::SetSectorHeight(float fSectorHeight)
{
	if (0.0f <= fSectorHeight)
	{
		m_fieldInfo.m_fSectorHeight = fSectorHeight;
	}
}

float CWorkSpaceField::GetSectorHeight()
{
	return m_fieldInfo.m_fSectorHeight;
}

const char* CWorkSpaceField::GameTime( int nGameTime )
{
	switch (nGameTime)
	{
	case TIME_DAWN:			return "Dawn";
	case TIME_DAYTIME:		return "Daytime";
	case TIME_SUNSET:		return "Sunset";
	case TIME_NIGHT:		return "Night";
	case TIME_CUSTOM1:		return "Custom1";
	case TIME_CUSTOM2:		return "Custom2";
	}
	return "";
}

bool CWorkSpaceField::OnCheckAlwaysPickingWhenMoused()
{
	// 피킹을 하지말야하는 조건...

	// 드래그일때 하지 마세요. 느려요.
	if(m_pControlMediator_->m_mouseInfo.mouseState.bDrag == true)
		return false;

	return true;
}

void CWorkSpaceField::RegisterToRenderSceneNode( rs3::RSceneNode* _pSceneNode )
{
	_pSceneNode->RemoveFromParent();
	rs3::REngine::GetSceneManager().AddSceneNode(_pSceneNode);

	m_vecRenderSceneNodes.push_back(_pSceneNode);
}

void CWorkSpaceField::ClearRenderSceneNodes()
{
	for(std::vector< rs3::RSceneNode* >::iterator itr = m_vecRenderSceneNodes.begin();
		itr != m_vecRenderSceneNodes.end(); ++itr)
	{
		(*itr)->RemoveFromParent();
	}

	m_vecRenderSceneNodes.clear();
}

void CWorkSpaceField::SetEditableLoadedZone_()
{
	LOGIC_OBJECTLIST& rList = m_pControlMediator_->GetToolLogicObjectMgr()->GetNoneEntityObjectList();
	
	for(LOGIC_OBJECTLIST::iterator it = rList.begin(); it != rList.end(); ++it)
	{
		(*it)->SetEditable(false);
	}
}
