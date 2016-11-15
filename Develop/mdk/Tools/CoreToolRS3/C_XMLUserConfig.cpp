#include "StdAfx.h"
#include "C_XMLUserConfig.h"
#include "M_ToolCamera.h"
#include "MXml.h"
#include "V_MFCHelper.h"
#include "MainFrm.h"

#include "VR_TerrainHeight.h"
#include "VR_TerrainFlat.h"
#include "VR_TerrainAverage.h"
#include "VR_TerrainSplat.h"
#include "VR_TerrainRelax.h"
#include "VR_TerrainGrass.h"

CXMLUserConfig::CXMLUserConfig(void) : m_strTextLoginID_("nobody"), m_strTextLoginIP_("localhost")
{
}

CXMLUserConfig::~CXMLUserConfig(void)
{
}

bool CXMLUserConfig::LoadConfigRelatedMapFile(const char *filename, rs3::RToolCamera* _pToolCamera)
{
	//초기화
	m_nWbState_ = -1;
	m_lastEnvInfo_.m_nTime = 0;
	m_lastEnvInfo_.m_nWeather = 0;
	m_lastEnvInfo_.m_strEnvSetName = "default";
	m_lastEnvInfo_.m_bVisible = true;

	//파일이름으로부터 확장자 바꾸기
	std::string reName(filename);
	size_t n_ext = reName.rfind(".xml");
	size_t n_name = reName.rfind("\\");
	if (n_name == std::string::npos)
	{
		n_name = reName.rfind("/");
		_ASSERT(n_name != std::string::npos);
	}
	reName = "user\\" + reName.substr(n_name, n_ext - n_name) + ".config";

	MXml kXml;
	if ( !kXml.LoadFile(reName.c_str()))
		return false;
   
	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("MAP_CONFIG");
	if ( pRootElement == NULL )
		return false;

	m_b1st3rdPerson = false;
	//map_config 파일 초기화
	m_lastCameraInfo_.vPos.Set(0, 0, 0);
	//for saved camera
	rs3::RVector vPos, vDir;
	_pToolCamera->ResetRememberLocation();

	pRootElement->Attribute("wb", &m_nWbState_);
	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement();
		pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		if ( _stricmp( pChildElement->Value(), "LAST_CAMERA") == 0)
		{
			double x, y, z, a_x, a_y, a_z;

			MXmlElement* pPosElement = pChildElement->FirstChildElement("POS");
			if(pPosElement == NULL) return true;
			pPosElement->Attribute("x", &x); m_lastCameraInfo_.vPos.x = (float)x;
			pPosElement->Attribute("y", &y); m_lastCameraInfo_.vPos.y = (float)y;
			pPosElement->Attribute("z", &z); m_lastCameraInfo_.vPos.z = (float)z;

			MXmlElement* pDirElement = pChildElement->FirstChildElement("DIR");
			if(pDirElement == NULL) return true;
			pDirElement->Attribute("x", &a_x); m_lastCameraInfo_.vDir.x = (float)a_x;
			pDirElement->Attribute("y", &a_y); m_lastCameraInfo_.vDir.y = (float)a_y;
			pDirElement->Attribute("z", &a_z); m_lastCameraInfo_.vDir.z = (float)a_z;
		}
		else if ( _stricmp( pChildElement->Value(), "SAVED_CAMERA") == 0)
		{
			int nIndex;
			_Attribute(nIndex, pChildElement, "index");

			double x, y, z, a_x, a_y, a_z;

			MXmlElement* pPosElement = pChildElement->FirstChildElement("POS");
			if(pPosElement == NULL) return true;
			pPosElement->Attribute("x", &x); vPos.x = (float)x;
			pPosElement->Attribute("y", &y); vPos.y = (float)y;
			pPosElement->Attribute("z", &z); vPos.z = (float)z;

			MXmlElement* pDirElement = pChildElement->FirstChildElement("DIR");
			if(pDirElement == NULL) return true;
			pDirElement->Attribute("x", &a_x); vDir.x = (float)a_x;
			pDirElement->Attribute("y", &a_y); vDir.y = (float)a_y;
			pDirElement->Attribute("z", &a_z); vDir.z = (float)a_z;

			_pToolCamera->RememberLocation(nIndex, vPos, vDir);
		}
		else if ( _stricmp( pChildElement->Value(), "LAST_ENV_SET") == 0)
		{
			_Attribute(m_lastEnvInfo_.m_nTime, pChildElement, "time");
			_Attribute(m_lastEnvInfo_.m_nWeather, pChildElement, "weather");
			_Attribute(m_lastEnvInfo_.m_strEnvSetName, pChildElement, "name");
			std::string strVisible;
			_Attribute(strVisible, pChildElement, "visible");
			if(strVisible == "true") m_lastEnvInfo_.m_bVisible = true;
			else m_lastEnvInfo_.m_bVisible = false;
		}
	}
	return true;
}

void CXMLUserConfig::SaveConfigRelatedMapFile(const char *filename, rs3::RToolCamera* _pToolCamera)
{
	//파일이름으로부터 확장자 바꾸기
	std::string reName(filename);
	int n_ext = reName.rfind(".xml");
	int n_name = reName.rfind("\\");
	reName = "user\\" + reName.substr(n_name, n_ext - n_name) + ".config";

	MXml kXml;
	MXmlDeclaration* pDecl = kXml.CreateDeclaration("1.0", "UTF-8", "");
	kXml.Doc()->LinkEndChild(pDecl);

	MXmlElement *pRootElement = new MXmlElement("MAP_CONFIG");
	kXml.Doc()->LinkEndChild(pRootElement);
	_SetAttribute(pRootElement, "wb", m_nWbState_);

	//CameraInfomation
	MXmlElement* pElementLastCamer = new MXmlElement( "LAST_CAMERA" );
	pRootElement->LinkEndChild(pElementLastCamer);

	MXmlElement* pElementPos = new MXmlElement( "POS" );
	MXmlElement* pElementDir = new MXmlElement( "DIR" );
	pElementLastCamer->LinkEndChild(pElementPos);
	pElementLastCamer->LinkEndChild(pElementDir);

	_SetAttribute(pElementPos, "x", m_lastCameraInfo_.vPos.x);
	_SetAttribute(pElementPos, "y", m_lastCameraInfo_.vPos.y);
	_SetAttribute(pElementPos, "z", m_lastCameraInfo_.vPos.z);

	_SetAttribute(pElementDir, "x", m_lastCameraInfo_.vDir.x);
	_SetAttribute(pElementDir, "y", m_lastCameraInfo_.vDir.y);
	_SetAttribute(pElementDir, "z", m_lastCameraInfo_.vDir.z);

	for( int i = 0; i < rs3::RToolCamera::DEFAULT_CAMERA; ++i)
	{
		const rs3::RToolCamera::LOCATION_INFO* pLocationInfo = _pToolCamera->GetLocation(i);
		if( pLocationInfo )
		{
			MXmlElement* pElementSavedCam = new MXmlElement( "SAVED_CAMERA" );
			pRootElement->LinkEndChild(pElementSavedCam);
			_SetAttribute(pElementSavedCam, "index", i);

			MXmlElement* pElementPos = new MXmlElement( "POS" );
			MXmlElement* pElementDir = new MXmlElement( "DIR" );
			pElementSavedCam->LinkEndChild(pElementPos);
			pElementSavedCam->LinkEndChild(pElementDir);

			_SetAttribute(pElementPos, "x", pLocationInfo->m_vPos.x);
			_SetAttribute(pElementPos, "y", pLocationInfo->m_vPos.y);
			_SetAttribute(pElementPos, "z", pLocationInfo->m_vPos.z);

			_SetAttribute(pElementDir, "x", pLocationInfo->m_vDir.x);
			_SetAttribute(pElementDir, "y", pLocationInfo->m_vDir.y);
			_SetAttribute(pElementDir, "z", pLocationInfo->m_vDir.z);
		}
	}

	//Last Env Set
	MXmlElement* pElementLastEnvSet = new MXmlElement("LAST_ENV_SET");
	pRootElement->LinkEndChild(pElementLastEnvSet);
	_SetAttribute(pElementLastEnvSet, "time", m_lastEnvInfo_.m_nTime);
	_SetAttribute(pElementLastEnvSet, "weather", m_lastEnvInfo_.m_nWeather);
	_SetAttribute(pElementLastEnvSet, "name", m_lastEnvInfo_.m_strEnvSetName);

	if(m_lastEnvInfo_.m_bVisible) _SetAttribute(pElementLastEnvSet, "visible", "true");
	else _SetAttribute(pElementLastEnvSet, "visible", "false");

	//최종적으로 저장
	kXml.SaveFile(reName.c_str());
}

void CXMLUserConfig::SetLastCameraInfo(const rs3::RVector &_vPos, const rs3::RVector &_vDir)
{
	m_lastCameraInfo_.vPos = _vPos;
	m_lastCameraInfo_.vDir = _vDir;
}

void CXMLUserConfig::SetEnvironmentInfo(bool _bVisible, int _time, int _weather, std::string& _strEnvSet)
{
	m_lastEnvInfo_.m_nTime = _time;
	m_lastEnvInfo_.m_nWeather = _weather;
	m_lastEnvInfo_.m_strEnvSetName = _strEnvSet;
	m_lastEnvInfo_.m_bVisible = _bVisible;
}

void CXMLUserConfig::GetEnvironmentInfo(bool& _bVisible, int& _time, int& _weather, std::string& _strEnvSet)
{
	_time = m_lastEnvInfo_.m_nTime;
	_weather = m_lastEnvInfo_.m_nWeather;
	_strEnvSet = m_lastEnvInfo_.m_strEnvSetName;
	_bVisible = m_lastEnvInfo_.m_bVisible;
}

bool CXMLUserConfig::GetLastCameraInfo(rs3::RVector &_vPos, rs3::RVector &_vDir) const
{
	rs3::RVector vecPosNULL(0);
	if(m_lastCameraInfo_.vPos == vecPosNULL)
		return false;

	_vPos = m_lastCameraInfo_.vPos;
	_vDir = m_lastCameraInfo_.vDir;
	return true;
}

void CXMLUserConfig::SetLastWork(int _lastWork)
{
	m_nWbState_ = _lastWork;
}

bool CXMLUserConfig::GetLastWork(int& _lastWork)
{
	if( m_nWbState_ < 0 || m_nWbState_ >= WORKBENCH_STATE::STATE_NUM )
		return false;
	_lastWork = m_nWbState_;
	return true;
}

//프로그램이 시작되거나 종료될때(프로그램과 같이 움직임)
//user directroy에 user.config.xml 로 저장
void CXMLUserConfig::LoadConfigRelatedUserTool(CControlMediator* pControlMediator)
{
	m_nCenterPivot_ = 0;
	m_nRememberWork_ = 1;

	MXml kXml;
	if ( !kXml.LoadFile("user\\user.config.xml"))
		return;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("USER_CONFIG");
	if ( pRootElement == NULL )
		return;

	//user_config 파일 초기화
	m_strRefMesh_.clear();

	int nSnapToGrid = 0;
	int nAngleSnap = 0;

	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement();
		pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		const char* pValue = pChildElement->Value();
		if ( _stricmp(pValue, "LAST_WORK") == 0)
		{
			int brush(1); //area
			double height(1), aver(1), flat(1), splat(1), relax(1), grass(1); //strength
			int grass_space(1);

			MXmlElement* pTerrainElement = pChildElement->FirstChildElement("TERRAIN");
			if(pTerrainElement != NULL)
			{
				pTerrainElement->Attribute("height", &height);
				T_SetMapStrengthControl_(
					pControlMediator->GetViewPtr<CRollupTerrainHeight>(), height, 0.5f);

				pTerrainElement->Attribute("aver", &aver);
				T_SetMapStrengthControl_(
					pControlMediator->GetViewPtr<CRollupTerrainAverage>(), aver, 1.0f);

				pTerrainElement->Attribute("flat", &flat);
				T_SetMapStrengthControl_(
					pControlMediator->GetViewPtr<CRollupTerrainFlat>(), flat, 1.0f);

				pTerrainElement->Attribute("splat", &splat);
				T_SetMapStrengthControl_(
					pControlMediator->GetViewPtr<CRollupTerrainSplat>(), splat, 0.5f);

				pTerrainElement->Attribute("relax", &relax);
				T_SetMapStrengthControl_(
					pControlMediator->GetViewPtr<CRollupTerrainRelax>(), relax, 0.2f);

				pTerrainElement->Attribute("grass", &grass);
				T_SetMapStrengthControl_(
					pControlMediator->GetViewPtr<CRollupTerrainGrass>(), grass, 1.0f);
				if(grass == 0.f) grass = 15.f;
				pTerrainElement->Attribute("grass_space", &grass_space);
				//grass space
				pControlMediator->GetViewPtr<CRollupTerrainGrass>()->m_nSpace = grass_space;
				pControlMediator->GetViewPtr<CRollupTerrainGrass>()->m_sliderCtrlSpace.SetPos(grass_space);

				//if set brush work area, then update brush windows, so do this job lastly
				pTerrainElement->Attribute("brush", &brush);
				CMFCHelper::SetBrushWorkAreaSize(brush);
			}

			//camera
			double wasd(0), wheel(0), rotate(0), pan(0);
			MXmlElement* pCameraElement = pChildElement->FirstChildElement("CAMERA");
			if(pCameraElement != NULL)
			{
				pCameraElement->Attribute("wasd", &wasd);
				pCameraElement->Attribute("wheel", &wheel);
				pControlMediator->m_cameraInfo.KEY_STEP = float(wasd);
				pControlMediator->m_cameraInfo.WHEEL_ZOOM_STEP = float(wheel);

				pCameraElement->Attribute("rotate", &rotate);
				pCameraElement->Attribute("pan", &pan);
				if( rotate != 0 && pan != 0)
				{
					pControlMediator->m_cameraInfo.ROTATE_STEP = float(rotate);
					pControlMediator->m_cameraInfo.MOVE_STEP = float(pan);
				}
			}
		}
		else if ( _stricmp(pValue, "REF_MESH") == 0)
		{
			const char* pRefMesh = pChildElement->GetText();
			if(pRefMesh != NULL)
				m_strRefMesh_ = pRefMesh;
		}
		else if ( _stricmp(pValue, "TextEditor")==0 )
		{
			const char* pTextEditor = pChildElement->GetText();
			SetTextEditor(pTextEditor);
		}
		else if ( _stricmp(pValue,"LoginID")==0 )
		{
			const char* pTextLoginID = pChildElement->GetText();
			SetTextLoginID(pTextLoginID);
		}
		else if ( _stricmp(pValue,"LoginIP")==0 )
		{
			const char* pTextLoginIP = pChildElement->GetText();
			SetTextLoginIP(pTextLoginIP);
		}
		else if ( _stricmp(pValue,"RememberWork")==0 )
			_Contents(&m_nRememberWork_, pChildElement);
		else if ( _stricmp(pValue,"CenterPivot")==0 )
			_Contents(&m_nCenterPivot_, pChildElement);
		else if ( _stricmp(pValue,"SnapToGrid")==0 )
			_Contents(&nSnapToGrid, pChildElement);
		else if ( _stricmp(pValue,"AngleSnap")==0 )
			_Contents(&nAngleSnap, pChildElement);
	}

	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	pMainFrame->SetSnapToGridSize( nSnapToGrid );
	pMainFrame->SetAngleSnapSize( nAngleSnap );
}

void CXMLUserConfig::SaveConfigRelatedUserTool(CControlMediator* pControlMediator)
{
	MXml kXml;
	MXmlDeclaration* pDecl = kXml.CreateDeclaration("1.0", "UTF-8", "");
	kXml.Doc()->LinkEndChild(pDecl);

	MXmlElement *pRootElement = new MXmlElement("USER_CONFIG");
	kXml.Doc()->LinkEndChild(pRootElement);

	//CameraInfomation
	MXmlElement* pElementLastWork = new MXmlElement( "LAST_WORK" );
	pRootElement->LinkEndChild(pElementLastWork);

	MXmlElement* pElementTerrain = new MXmlElement( "TERRAIN" );
	pElementLastWork->LinkEndChild(pElementTerrain);

	int brush = CMFCHelper::GetBrushWorkAreaSize();
	_SetAttribute(pElementTerrain, "brush", brush);

	float value;
	T_GetMapStrengthControl_(pControlMediator->GetViewPtr<CRollupTerrainHeight>(), value);
	_SetAttribute(pElementTerrain, "height", value);

	T_GetMapStrengthControl_(pControlMediator->GetViewPtr<CRollupTerrainAverage>(), value);
	_SetAttribute(pElementTerrain, "aver", value);

	T_GetMapStrengthControl_(pControlMediator->GetViewPtr<CRollupTerrainFlat>(), value);
	_SetAttribute(pElementTerrain, "flat", value);

	T_GetMapStrengthControl_(pControlMediator->GetViewPtr<CRollupTerrainSplat>(), value);
	_SetAttribute(pElementTerrain, "splat", value);

	T_GetMapStrengthControl_(pControlMediator->GetViewPtr<CRollupTerrainRelax>(), value);
	_SetAttribute(pElementTerrain, "relax", value);

	T_GetMapStrengthControl_(pControlMediator->GetViewPtr<CRollupTerrainGrass>(), value);
	_SetAttribute(pElementTerrain, "grass", value);

	int grass_space = pControlMediator->GetViewPtr<CRollupTerrainGrass>()->m_nSpace;
	_SetAttribute(pElementTerrain, "grass_space", grass_space);

	MXmlElement* pElementCamera = new MXmlElement( "CAMERA" );
	pElementLastWork->LinkEndChild(pElementCamera);
	_SetAttribute(pElementCamera, "wasd", pControlMediator->m_cameraInfo.KEY_STEP);
	_SetAttribute(pElementCamera, "wheel", pControlMediator->m_cameraInfo.WHEEL_ZOOM_STEP);
	_SetAttribute(pElementCamera, "rotate", pControlMediator->m_cameraInfo.ROTATE_STEP);
	_SetAttribute(pElementCamera, "pan", pControlMediator->m_cameraInfo.MOVE_STEP);

	//ref mesh
	MXmlElement* pElementLastRefMesh = new MXmlElement( "REF_MESH" );
	pRootElement->LinkEndChild(pElementLastRefMesh);
	_SetContents(pElementLastRefMesh, m_strRefMesh_);

	//text editor
	MXmlElement* pElementTextEditor = new MXmlElement( "TextEditor" );
	pRootElement->LinkEndChild(pElementTextEditor);
	_SetContents(pElementTextEditor, m_strTextEditor_);

	// login id
	MXmlElement* pElementTextLoginID = new MXmlElement( "LoginID" );
	pRootElement->LinkEndChild(pElementTextLoginID);
	_SetContents(pElementTextLoginID, m_strTextLoginID_);

	// login ip
	MXmlElement* pElementTextLoginIP = new MXmlElement( "LoginIP" );
	pRootElement->LinkEndChild(pElementTextLoginIP);
	_SetContents(pElementTextLoginIP, m_strTextLoginIP_);

	//Options
	MXmlElement* pElementRememberWork = new MXmlElement( "RememberWork" );
	pRootElement->LinkEndChild(pElementRememberWork);
	_SetContents(pElementRememberWork, m_nRememberWork_);

	MXmlElement* pElementCenterPivot = new MXmlElement( "CenterPivot" );
	pRootElement->LinkEndChild(pElementCenterPivot);
	_SetContents(pElementCenterPivot, m_nCenterPivot_);

	//////////////////////////////////////////////////////////////////////////
	// view menu

	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	int nSnapToGrid = pMainFrame->GetSnapToGridSize();

	MXmlElement* pElementSnapToGrid = new MXmlElement( "SnapToGrid" );
	pRootElement->LinkEndChild(pElementSnapToGrid);
	_SetContents(pElementSnapToGrid, nSnapToGrid);

	int nAngleSnapSize = pMainFrame->GetAngleSnapSize();

	MXmlElement* pElementAngleSnap = new MXmlElement( "AngleSnap" );
	pRootElement->LinkEndChild(pElementAngleSnap);
	_SetContents(pElementAngleSnap, nAngleSnapSize);

	//최종적으로 저장
	kXml.SaveFile("user\\user.config.xml");
}

void CXMLUserConfig::SetRefMeshName(const char* _pMeshName)
{
	m_strRefMesh_ = _pMeshName;
}
const char* CXMLUserConfig::GetRefMeshName()
{
	if(m_strRefMesh_.empty())
		return NULL;
	else
		return m_strRefMesh_.c_str();
}

void CXMLUserConfig::SetTextEditor(const char* _pSzTextEditor)
{
	if(GetFileAttributes(_pSzTextEditor) == INVALID_FILE_ATTRIBUTES)
	{
		if ( _pSzTextEditor != NULL && _stricmp("notepad.exe", _pSzTextEditor) != 0 )
			AfxMessageBox("잘못된 경로. 텍스트 에디터를 올바르게 설정했는지 확인해주세요.\n기본 편집기인 메모장을 텍스트 에디터로 설정합니다.");
		m_strTextEditor_ = "notepad.exe";
	}
	else
		m_strTextEditor_ = _pSzTextEditor;
}

void CXMLUserConfig::SetTextLoginID(const char* _pSzTextLoginID)
{
	m_strTextLoginID_ = _pSzTextLoginID;
}

void CXMLUserConfig::SetTextLoginIP(const char* _pSzTextLoginIP)
{
	m_strTextLoginIP_ = _pSzTextLoginIP;
}
