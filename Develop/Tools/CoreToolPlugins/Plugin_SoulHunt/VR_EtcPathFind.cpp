// V_EditEtcPathFind.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VR_EtcPathFind.h"
#include "VR_EtcCollision.h"
#include "VR_EnvWeather.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperResource.h"

// path finding 시에 보여줄 오브젝트
#include "VL_SceneNodeList.h"

#include "M_PathFinder.h"
#include "RRenderHelper.h"

#include "MXml.h"

// CRollupEtcPathFind 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupEtcPathFind, CDialog)

CRollupEtcPathFind::CRollupEtcPathFind(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupEtcPathFind::IDD, pParent)
	, m_nFindMode(0)
	, m_strSeedsFilename("")
	, m_bEnablePickingArea(FALSE)
	, m_nPickingMinX(0)
	, m_nPickingMaxX(0)
	, m_nPickingMinY(0)
	, m_nPickingMaxY(0)
{
	m_fStartX_ = 0.f;
	m_fStartY_ = 0.f;
	m_fStartZ_ = 0.f;

	m_fEndX_ = 0.f;
	m_fEndY_ = 0.f;
	m_fEndZ_ = 0.f;
}

CRollupEtcPathFind::~CRollupEtcPathFind()
{
}

void CRollupEtcPathFind::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ACTIVE_NAVI, m_btnCheckNaviActivity);

	DDX_Text(pDX, IDC_EDIT_START_X, m_fStartX_);
	DDX_Text(pDX, IDC_EDIT_START_Y, m_fStartY_);
	DDX_Text(pDX, IDC_EDIT_START_Z, m_fStartZ_);

	DDX_Text(pDX, IDC_EDIT_END_X, m_fEndX_);
	DDX_Text(pDX, IDC_EDIT_END_Y, m_fEndY_);
	DDX_Text(pDX, IDC_EDIT_END_Z, m_fEndZ_);

	DDX_Radio(pDX, IDC_RADIO_FIND_TEST, m_nFindMode);
	DDX_Control(pDX, IDC_LIST_SEED_LIST, m_ctlSeedList);
	DDX_Check(pDX, IDC_CHECK_PICKING_AREA, m_bEnablePickingArea);
	DDX_Text(pDX, IDC_EDIT_PICK_MINX, m_nPickingMinX);
	DDX_Text(pDX, IDC_EDIT_PICK_MAXX, m_nPickingMaxX);
	DDX_Text(pDX, IDC_EDIT_PICK_MINY, m_nPickingMinY);
	DDX_Text(pDX, IDC_EDIT_PICK_MAXY, m_nPickingMaxY);
}

BEGIN_MESSAGE_MAP(CRollupEtcPathFind, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PATH_FIND, &CRollupEtcPathFind::OnBnClickedButtonPathFind)
	ON_BN_CLICKED(IDC_CHECK_ACTIVE_NAVI, &CRollupEtcPathFind::OnBnClickedCheckActiveNavi)
	ON_BN_CLICKED(IDC_BUTTON_PATH_RESET, &CRollupEtcPathFind::OnBnClickedButtonPathReset)
	ON_BN_CLICKED(IDC_BUTTON_PATH_CREATE, &CRollupEtcPathFind::OnBnClickedButtonPathCreate)
	ON_BN_CLICKED(IDC_RADIO_FIND_TEST, &CRollupEtcPathFind::OnBnClickedRadioFindTest)
	ON_BN_CLICKED(IDC_RADIO_FIND_SEED, &CRollupEtcPathFind::OnBnClickedRadioFindSeed)
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDC_LIST_SEED_LIST, &CRollupEtcPathFind::OnLbnDblclkListSeedList)
	ON_BN_CLICKED(IDC_BUTTON_DEL_SEL_SEED, &CRollupEtcPathFind::OnBnClickedButtonDelSelSeed)
	ON_BN_CLICKED(IDC_CHECK_PICKING_AREA, &CRollupEtcPathFind::OnBnClickedCheckPickingArea)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_PICKINGZ, &CRollupEtcPathFind::OnBnClickedButtonPathSavePickingz)
END_MESSAGE_MAP()


// CRollupEtcPathFind 메시지 처리기입니다.

void CRollupEtcPathFind::Listen(CoreMessage& _message)
{
	switch( _message.nID )
	{
		// 활성
		case VIEW_MESSAGE::ACTIVE_VIEW :
		{
			m_eWBType = SEL_NONE;
			m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::ETC_PATH_SELECT);
		}
		break;

		// 필드 / 존 로딩
		case VIEW_MESSAGE::LOGIC_LOADED :
		case VIEW_MESSAGE::SCENE_LOADED :
		{
			ClearSeedList();

			// 네비게이션 메시를 활성화 할수 있도록
			GetDlgItem(IDC_CHECK_ACTIVE_NAVI)->EnableWindow(TRUE);

			// 일단 비활성으로 시작
			m_btnCheckNaviActivity.SetCheck(FALSE);
			OnBnClickedCheckActiveNavi();

			// seed loading
			{
				string strZonefilename = m_pControlMediator_->m_mapInfo.m_strScenePath + m_pControlMediator_->m_mapInfo.m_strSceneFile;
				string strExt = ".zone.xml";
				size_t findpos = strZonefilename.find(strExt);
				if( findpos != string::npos )
					m_strSeedsFilename = strZonefilename.replace(findpos, strExt.length(), ".seeds.xml");
				else
					m_strSeedsFilename = "";

				if (!m_strSeedsFilename.empty())
					LoadSeeds(m_strSeedsFilename.c_str());
			}
			
			// pickingz loading
			{
				string strZonefilename = m_pControlMediator_->m_mapInfo.m_strScenePath + m_pControlMediator_->m_mapInfo.m_strSceneFile;
				string strExt = ".zone.xml";
				size_t findpos = strZonefilename.find(strExt);
				if( findpos != string::npos )
					m_strPickzFilename = strZonefilename.replace(findpos, strExt.length(), ".pickz.xml");
				else
					m_strPickzFilename = "";

				if (!m_strPickzFilename.empty())
					LoadPickz(m_strPickzFilename.c_str());

				OnBnClickedCheckPickingArea();
			}



			// collision 설정, TODO: zone / field WS 에 있는게 맞을듯
			CString strFilename = m_pControlMediator_->m_mapInfo.m_strSceneFile.c_str();
			strFilename.Replace("zone.xml", "cl2");

			std::string strFullCl2Name = m_pControlMediator_->m_mapInfo.m_strScenePath + strFilename.GetString();
			CRollupEtcCollision* pRollupCollsion = m_pControlMediator_->GetViewPtr<CRollupEtcCollision>();
			pRollupCollsion->SetCollisionFileName( strFullCl2Name.c_str() );

			// 눈, 비등의 파티클 충돌에 CollisionTree 가 필요하므로 다시 생성해준다.
			CRollupEnvWeather* pRollupWeather = m_pControlMediator_->GetViewPtr<CRollupEnvWeather>();
			pRollupWeather->UpdateEnvironment();

			m_nPickingMinX = (int)m_rtPickZArea.left;
			m_nPickingMinY = (int)m_rtPickZArea.top;
			m_nPickingMaxX = (int)m_rtPickZArea.right;
			m_nPickingMaxY = (int)m_rtPickZArea.bottom;
			m_bEnablePickingArea = false;
			if (m_nPickingMaxX > 0 ||
				m_nPickingMaxY > 0 ||
				m_nPickingMinX > 0 ||
				m_nPickingMinY > 0)
			{
				m_bEnablePickingArea = true;
			}
			UpdateData(FALSE);
		}
		break;
	}
}

void CRollupEtcPathFind::OnZoneSaved()
{
	if (!m_strSeedsFilename.empty())
		SaveSeeds(m_strSeedsFilename.c_str());

	if (!m_strPickzFilename.empty())
		SavePickz(m_strPickzFilename.c_str());
}

BOOL CRollupEtcPathFind::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_btnCheckNaviActivity.SetCheck(FALSE);
	GetDlgItem(IDC_CHECK_ACTIVE_NAVI)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_FIND_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_FIND_SEED)->EnableWindow(FALSE);

	UpdateData(FALSE);
	return TRUE;
}

void CRollupEtcPathFind::OnCancel()
{
}

void CRollupEtcPathFind::OnOK()
{
}

void CRollupEtcPathFind::SetViewNavigationMesh(BOOL flag)
{
	// 리셋
	OnBnClickedButtonPathReset();

	if (flag)
	{
		if (m_nFindMode == 0)
			UpdateFinderModeUI(UI_TEST);
		else
			UpdateFinderModeUI(UI_SEED);

		g_PathfinderTest.SetRenderableNavigationMesh(true);
		m_eWBType = SEL_START;
	}
	else
	{
		UpdateFinderModeUI(UI_DISABLE);
		g_PathfinderTest.SetRenderableNavigationMesh(false);
		m_eWBType = SEL_NONE;
	}
}

void CRollupEtcPathFind::OnBnClickedCheckActiveNavi()
{
	// 모래시계 시작
	HCURSOR hOld = SetCursor(LoadCursor(NULL, IDC_WAIT));
	{
		// 초기 보여줄 객체들 설정
		BOOL bFlag = m_btnCheckNaviActivity.GetCheck();
		if ( bFlag )
		{
			CDlgSceneNodeList* pObjectList = m_pControlMediator_->GetViewPtr<CDlgSceneNodeList>();
			if( pObjectList->GetViewState( rs3::RSID_WATERPLANE ) )	pObjectList->ViewStateToggle( rs3::RSID_WATERPLANE );
			if( pObjectList->GetViewState( rs3::RSID_TREE ) )		pObjectList->ViewStateToggle( rs3::RSID_TREE );
		}

		SetViewNavigationMesh(bFlag);
	}
	//모래시계 끝
	SetCursor(hOld);
}

void CRollupEtcPathFind::OnBnClickedButtonPathFind()
{
	// 길찾기 테스트

	UpdateData(TRUE);

	RVector st, en;

	st.Set(m_fStartX_, m_fStartY_, m_fStartZ_);
	en.Set(m_fEndX_, m_fEndY_, m_fEndZ_);

	st.z += 100.0f;
	en.z += 100.0f;

	g_PathfinderTest.FindPath(st, en);
}

void CRollupEtcPathFind::OnBnClickedButtonPathReset()
{
	g_PathfinderTest.ReleasePath();

	m_fStartX_ = m_fStartY_ = m_fStartZ_ = 0.f;
	m_fEndX_ = m_fEndY_ = m_fEndZ_ = 0.f;
	m_eWBType = SEL_START;
	UpdateData(FALSE);
}

void CRollupEtcPathFind::OnBnClickedButtonPathCreate()
{
	using namespace rs3;

	vector<MVector3> arraySeed;
	GetSeeds(arraySeed);

	CreateNavigationFile(arraySeed);

	// seed save
	SaveSeeds(m_strSeedsFilename.c_str());
}

void CRollupEtcPathFind::OnBnClickedRadioFindTest()
{
	m_nFindMode = 0;
	UpdateFinderModeUI(UI_TEST);
}

void CRollupEtcPathFind::OnBnClickedRadioFindSeed()
{
	m_nFindMode = 1;
	UpdateFinderModeUI(UI_SEED);
}

void CRollupEtcPathFind::UpdateFinderModeUI( FINDER_UI_MODE _eFinderMode )
{
	GetDlgItem(IDC_RADIO_FIND_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_FIND_SEED)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_START_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_START_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_START_Z)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_END_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_END_Y)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_END_Z)->EnableWindow(FALSE);

	GetDlgItem(IDC_BUTTON_PATH_RESET)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PATH_FIND)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_PATH_HRANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PATH_VRANGE)->EnableWindow(FALSE);

	GetDlgItem(IDC_LIST_SEED_LIST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DEL_SEL_SEED)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PATH_CREATE)->EnableWindow(TRUE);

	if (_eFinderMode == UI_TEST)
	{
		GetDlgItem(IDC_RADIO_FIND_TEST)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_FIND_SEED)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_START_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_START_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_START_Z)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_END_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_END_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_END_Z)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUTTON_PATH_RESET)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_PATH_FIND)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_PATH_HRANGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PATH_VRANGE)->EnableWindow(TRUE);

	}
	else if (_eFinderMode == UI_SEED)
	{
		GetDlgItem(IDC_RADIO_FIND_TEST)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_FIND_SEED)->EnableWindow(TRUE);

		GetDlgItem(IDC_LIST_SEED_LIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_DEL_SEL_SEED)->EnableWindow(TRUE);
	}
}

void CRollupEtcPathFind::AddSeed(rs3::RVector& _refSeedPos)
{

	rs3::RMatrix* pMat = new rs3::RMatrix;
	pMat->SetLocalMatrix(_refSeedPos, rs3::RVector::AXISY, rs3::RVector::AXISZ);

	char szItemName[256];
	sprintf(szItemName, "seed_%.1f_%.1f_%.1f", _refSeedPos.x, _refSeedPos.y, _refSeedPos.z);
	int num = m_ctlSeedList.AddString(szItemName);

	m_ctlSeedList.SetItemDataPtr(num, (void*)pMat);
}

void CRollupEtcPathFind::GetSeeds( std::vector< MVector3 >& _refVecArray )
{
	int nCount = m_ctlSeedList.GetCount();
	for (int i = 0; i < nCount; ++i )
	{
		rs3::RMatrix* pMat = static_cast<rs3::RMatrix*>( m_ctlSeedList.GetItemDataPtr(i) );
		_refVecArray.push_back( MVector3( pMat->_41, pMat->_42, pMat->_43 ) );
	}
}

void CRollupEtcPathFind::RenderSeed()
{
	int nCount = m_ctlSeedList.GetCount();
	int nSelected = m_ctlSeedList.GetCurSel();

	RBoundingBox aabb;
	aabb.vmin = RVector(-100,-100,-100);
	aabb.vmax = RVector( 100, 100, 100);

	for (int i = 0; i < nCount; ++i )
	{
		rs3::RMatrix* pMat = static_cast<rs3::RMatrix*>( m_ctlSeedList.GetItemDataPtr(i) );
		RRenderHelper::RenderSphere(*pMat, 100, D3DCOLOR_XRGB(250, 0, 250));

		if (i == nSelected)
			RRenderHelper::RenderBox(*pMat, aabb);
	}
}

bool CRollupEtcPathFind::LoadSeeds(const char *filename)
{
	MXml kXml;
	if ( !kXml.LoadFile(filename))
		return false;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("SEEDS");
	if (pRootElement == NULL)
		return false;

	for ( MXmlElement *pChild = pRootElement->FirstChildElement("POS");
		pChild != NULL; pChild = pChild->NextSiblingElement("POS"))
	{
		string strText;
		RVector vPos;
		if ( _Contents(strText, pChild))
		{
			sscanf(strText.c_str(),"%f %f %f",&vPos.x,&vPos.y,&vPos.z);
			AddSeed(vPos);
		}
	}

	return true;
}

void CRollupEtcPathFind::SaveSeeds(const char *filename)
{
	vector<MVector3> vSeeds;
	GetSeeds(vSeeds);
	
	if ( vSeeds.empty())
		return;

	MXml kXml;
	MXmlDeclaration* pDecl = kXml.CreateDeclaration("1.0", "UTF-8", "");
	kXml.Doc()->LinkEndChild(pDecl);

	MXmlElement *pRootElement = new MXmlElement("SEEDS");
	kXml.Doc()->LinkEndChild(pRootElement);

	for (size_t si = 0; si<vSeeds.size(); ++si)
	{
		char pcBuffer[128];

		MXmlElement *pChild = new MXmlElement("POS");
		sprintf(pcBuffer,"%f %f %f", vSeeds[si].x, vSeeds[si].y, vSeeds[si].z);
		_SetContents(pChild, pcBuffer);
		pRootElement->LinkEndChild(pChild);
	}

	bool bSaveResult = kXml.SaveFile(filename);
	_ASSERT(bSaveResult);
}

bool CRollupEtcPathFind::LoadPickz(const char *filename)
{
	MXml kXml;
	if ( !kXml.LoadFile(filename))
		return false;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("AREA");
	if (pRootElement == NULL)
		return false;

	string strText;
	if ( _Contents(strText, pRootElement))
	{
		sscanf(strText.c_str(),"%d %d %d %d %d",
			&m_bEnablePickingArea, &m_rtPickZArea.left, &m_rtPickZArea.top, &m_rtPickZArea.right, &m_rtPickZArea.bottom);
	}

	UpdateData(FALSE);

	return true;
}

void CRollupEtcPathFind::SavePickz(const char *filename)
{
	UpdateData();

	MXml kXml;
	MXmlDeclaration* pDecl = kXml.CreateDeclaration("1.0", "UTF-8", "");
	kXml.Doc()->LinkEndChild(pDecl);

	MXmlElement *pRootElement = new MXmlElement("AREA");
	char pcBuffer[128];
	sprintf(pcBuffer,"%d %d %d %d %d", 
		m_bEnablePickingArea, m_rtPickZArea.left, m_rtPickZArea.top, m_rtPickZArea.right, m_rtPickZArea.bottom);
	_SetContents(pRootElement, pcBuffer);
	kXml.Doc()->LinkEndChild(pRootElement);
	bool bSaveResult = kXml.SaveFile(filename);
	_ASSERT(bSaveResult);
}


void CRollupEtcPathFind::ClearSeedList()
{
	int nCount = m_ctlSeedList.GetCount();
	for (int i = 0; i < nCount; ++i )
	{
		rs3::RMatrix* pMat = static_cast<rs3::RMatrix*>( m_ctlSeedList.GetItemDataPtr(i) );
		delete pMat;
	}
	m_ctlSeedList.ResetContent();
}

void CRollupEtcPathFind::OnDestroy()
{
	__super::OnDestroy();
	ClearSeedList();
}

void CRollupEtcPathFind::OnLbnDblclkListSeedList()
{
	rs3::RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();
	if( pCamera == NULL)
		return;

	int nSelected = m_ctlSeedList.GetCurSel();
	rs3::RMatrix* pMat = static_cast<rs3::RMatrix*>( m_ctlSeedList.GetItemDataPtr(nSelected) );

	rs3::RVector vecSelected(pMat->_41, pMat->_42, pMat->_43);
	pCamera->SetPositionToViewCenter( vecSelected, Tool3Const::CONST_WB_ENTITY_OBJ_TO_CENTER * 3 );
}

void CRollupEtcPathFind::OnBnClickedButtonDelSelSeed()
{
	int nSelected = m_ctlSeedList.GetCurSel();
	if (nSelected < 0)
	{
		AfxMessageBox("선택된 씨드가 없습니다.");
		return;
	}

	rs3::RMatrix* pMat = static_cast<rs3::RMatrix*>( m_ctlSeedList.GetItemDataPtr(nSelected) );
	m_ctlSeedList.DeleteString(nSelected);
	delete pMat;
}

void CRollupEtcPathFind::CreateNavigationFile(vector<MVector3> &seeds)
{
	using namespace rs3;

	RToolTerrain *pToolTerrain = static_cast<RToolTerrain*>(m_pControlMediator_->GetToolSceneManager()->GetCurrentTerrain());
	if(pToolTerrain)
	{
		// 물 아래 이동할수 없는 부분을 marking
		pToolTerrain->UpdateDescriptMapUnderWater();

#if 0
		// Seed를 이용하여 갈 수 없는 부분을 Making 한다.
		if (!seeds.empty())
		{
			pToolTerrain->UpdateDescriptMapMakedAll(false);
			for (size_t si = 0; si<seeds.size(); ++si)
				pToolTerrain->UpdateDescriptMapMakedFromSeed(seeds[si],0.7f);
		}
		else
		{
			// 전부 Making 한다.
			pToolTerrain->UpdateDescriptMapMakedAll(true);
			pToolTerrain->UpdateDescriptMapMakedBySlope(0.7f);
		}
#endif

	}

	if(m_pControlMediator_->m_mapInfo.m_bLoaded && m_pControlMediator_->m_mapInfo.m_bSceneEditableMap)
	{
		//모래시계 시작
		HCURSOR hOld = SetCursor(LoadCursor(NULL, IDC_WAIT));

		MPathEngineExporter	pathexporter;
		RSceneManager* pSM = m_pControlMediator_->GetToolSceneManager();

		CString fullPath((m_pControlMediator_->m_mapInfo.m_strScenePath).c_str());
		CString fileName((m_pControlMediator_->m_mapInfo.m_strSceneFile).c_str());

		fileName.Replace(".zone.xml", "");
		fullPath = fullPath + fileName;
		string strFileName = fullPath;

		g_PathfinderTest.ReleasePath();
		g_PathfinderTest.Release();

		if ( pathexporter.MakeNavMesh(&g_PathfinderTest.m_PathFinder, pSM, seeds))
		{
			pathexporter.SaveToFile(&g_PathfinderTest.m_PathFinder, strFileName.c_str());

			RTerrain* pTerrain = m_pControlMediator_->GetToolSceneManager()->GetCurrentTerrain();
			RHeightField *pHeightField = (pTerrain ? pTerrain->GetHeightFieldPtr() : NULL);

			g_PathfinderTest.LoadZone(strFileName, pHeightField);
			g_PathfinderTest.MakeInvalid(pathexporter.m_invaildExPosition);

			// 서버에도 저장
			{
				string strNaviFileName = strFileName + ".navmesh.pathengine";
				CControlMediatorHelperResource::CopyToServerZoneFolder(strNaviFileName.c_str());

				strNaviFileName = strFileName + ".small.collision.pathengine";
				CControlMediatorHelperResource::CopyToServerZoneFolder(strNaviFileName.c_str());

				strNaviFileName = strFileName + ".small.pathfind.pathengine";
				CControlMediatorHelperResource::CopyToServerZoneFolder(strNaviFileName.c_str());
			}

			int nFaceNumber = g_PathfinderTest.MakeNavMesh();

			//모래시계 끝
			SetCursor(hOld);

			char pcBuffer[128];
			pcBuffer[0] = 0;
			sprintf(pcBuffer,"Pathfinding Saved(%d faces)", nFaceNumber);
			MessageBox(pcBuffer,"Success", MB_OK);
		}
		else
		{
			//모래시계 끝
			SetCursor(hOld);
			const string &errorMessage = pathexporter.GetErrorMessage();
			if ( errorMessage.length() > 0 )
				MessageBox(errorMessage.c_str(), "Error Message", MB_ICONERROR | MB_OK );
		}
	}
	else
	{
		AfxMessageBox("존 형식의 맵이 로드되어 있지 않습니다.");
	}
}
void CRollupEtcPathFind::OnBnClickedCheckPickingArea()
{
	UpdateData();

	GetDlgItem(IDC_EDIT_PICK_MINX)->EnableWindow(m_bEnablePickingArea);
	GetDlgItem(IDC_EDIT_PICK_MINY)->EnableWindow(m_bEnablePickingArea);
	GetDlgItem(IDC_EDIT_PICK_MAXX)->EnableWindow(m_bEnablePickingArea);
	GetDlgItem(IDC_EDIT_PICK_MAXY)->EnableWindow(m_bEnablePickingArea);	
	GetDlgItem(IDC_BUTTON_SAVE_PICKINGZ)->EnableWindow(TRUE);	

	UpdateData(FALSE);
}

void CRollupEtcPathFind::OnBnClickedButtonPathSavePickingz()
{
	UpdateData();

	(int)m_rtPickZArea.left = m_nPickingMinX;
	(int)m_rtPickZArea.top = m_nPickingMinY;
	(int)m_rtPickZArea.right = m_nPickingMaxX;
	(int)m_rtPickZArea.bottom = m_nPickingMaxY;

	if (!m_strPickzFilename.empty())
		SavePickz(m_strPickzFilename.c_str());
}

CControlMediator::MAP_INFO& CRollupEtcPathFind::GetZoneInfo()
{
	return m_pControlMediator_->m_mapInfo;
}