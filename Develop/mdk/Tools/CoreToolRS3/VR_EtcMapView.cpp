// V_EditEtcMapView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VR_EtcMapView.h"

#include "VL_ResourceInfo.h"

#include "C_XMLUserConfig.h"
#include "C_XMLResourceList.h"
#include "C_XMLXConst.h"

#include "C_ControlMediatorHelperAction.h"
#include "C_ControlMediatorHelperResource.h" // for minimap creation
#include "C_ControlMediatorHelperRender.h"
#include "RWaterPassRenderer.h"

// CEditEtcMapView 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupEtcMapView, CDialog)

CRollupEtcMapView::CRollupEtcMapView(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupEtcMapView::IDD, pParent)
	, m_bTryCreateMinimap_(false)
{
}

CRollupEtcMapView::~CRollupEtcMapView()
{
}

void CRollupEtcMapView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// numeric edit box
	DDX_Control(pDX, IDC_EDIT_CAM_WSAD, m_ctrEditWSAD_);
	DDX_Control(pDX, IDC_EDIT_CAM_WHEEL, m_ctrEditWheel_);
	DDX_Control(pDX, IDC_EDIT_CAM_FOV, m_ctrEditFOV_);
	DDX_Control(pDX, IDC_EDIT_CAM_FARZ, m_ctrEditFarZ_);

	// numeric spin box
	DDX_Control(pDX, IDC_SPIN_CAM_WSAD, m_ctrlSpinWSAD_);
	DDX_Control(pDX, IDC_SPIN_CAM_WHEEL, m_ctrlSpinWheel_);
	DDX_Control(pDX, IDC_SPIN_CAM_FOV, m_ctrlSpinFOV_);
	DDX_Control(pDX, IDC_SPIN_CAM_FARZ, m_ctrlSpinFarZ_);
	DDX_Control(pDX, IDC_CHECK_CENTER_PIVOT, m_ctlCheckCenterPivot);
	DDX_Control(pDX, IDC_LIST_CAM_POS, m_ctlListCamLocation_);
	DDX_Control(pDX, IDC_CHECK_VIEW_TERRAIN_INFO, m_ctlCheckTerrainInfo_);
	DDX_Control(pDX, IDC_CHECK_VIEW_PIVOT, m_ctlVeiwCameraPivot);
	DDX_Control(pDX, IDC_CHECK_THUMBNAME_CURRENT, m_ctlCheckKeepCurrentCamera);
	DDX_Control(pDX, IDC_1ST_3RD_CAMERA, m_ctl1st3rdPerson);
}

BEGIN_MESSAGE_MAP(CRollupEtcMapView, CDialog)
	ON_EN_CHANGE(IDC_EDIT_CAM_WSAD, &CRollupEtcMapView::OnEnChangeEditCamSpeed)
	ON_EN_CHANGE(IDC_EDIT_CAM_WHEEL, &CRollupEtcMapView::OnEnChangeEditCamSpeed)
	ON_EN_CHANGE(IDC_EDIT_CAM_FOV, &CRollupEtcMapView::OnEnChangeEditCamFov)
	ON_EN_CHANGE(IDC_EDIT_CAM_FARZ, &CRollupEtcMapView::OnEnChangeEditCamFarZ)
	ON_BN_CLICKED(IDC_CHECK_CENTER_PIVOT, &CRollupEtcMapView::OnBnClickedCheckCenterPivot)
	ON_BN_CLICKED(IDC_BUTTON_CAM_SAVE, &CRollupEtcMapView::OnBnClickedButtonCamSave)
	ON_BN_CLICKED(IDC_BUTTON_CAM_DELETE, &CRollupEtcMapView::OnBnClickedButtonCamDelete)
	ON_LBN_DBLCLK(IDC_LIST_CAM_POS, &CRollupEtcMapView::OnLbnDblclkListCamPos)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_MINIMAP, &CRollupEtcMapView::OnBnClickedButtonUpdateMinimap)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_RES_MGR, &CRollupEtcMapView::OnBnClickedButtonOpenResMgr)
	ON_BN_CLICKED(IDC_CHECK_VIEW_TERRAIN_INFO, &CRollupEtcMapView::OnBnClickedCheckViewTerrainInfo)
	ON_BN_CLICKED(IDC_BUTTON_PROFILE, &CRollupEtcMapView::OnBnClickedButtonProfile)
	ON_BN_CLICKED(IDC_BUTTON_FOV_TOOLVALUE, &CRollupEtcMapView::OnBnClickedButtonFovToolvalue)
	ON_BN_CLICKED(IDC_BUTTON_FOV_GAMEVALUE, &CRollupEtcMapView::OnBnClickedButtonFovGamevalue)
	ON_BN_CLICKED(IDC_BUTTON_FARZ_TOOLVALUE, &CRollupEtcMapView::OnBnClickedButtonFarzToolvalue)
	ON_BN_CLICKED(IDC_BUTTON_FARZ_GAMEVALUE, &CRollupEtcMapView::OnBnClickedButtonFarzGamevalue)
	ON_BN_CLICKED(IDC_1ST_3RD_CAMERA, &CRollupEtcMapView::OnBnClicked1st3rdPerson)
	ON_BN_CLICKED(IDC_BUTTON_TOPVIEW_NAVI, &CRollupEtcMapView::OnBnClickedButtonTopviewNavi)
END_MESSAGE_MAP()


// CEditEtcMapView 메시지 처리기입니다.


void CRollupEtcMapView::Listen(CoreMessage& _message)
{
	switch( _message.nID )
	{
		case VIEW_MESSAGE::DEVICE_CREATED :
		{
			m_ctrlSpinFOV_.SetPos( XCONST::CAMERA_FOV );
			m_ctrlSpinFarZ_.SetPos( XCONST::CAMERA_FAR_Z );
		}
		break;

		// 활성
		case VIEW_MESSAGE::ACTIVE_VIEW :
		{
			m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::ETC_MAP_VIEW);
		}
		break;

		// 존 로딩
		case VIEW_MESSAGE::SCENE_LOADED :
		case VIEW_MESSAGE::LOGIC_LOADED :
		{
			// update saved camera location
			UpdateCameraLocationList();

			// 미니맵 비우기
			m_bTryCreateMinimap_ = false;
			m_miniMapDib_.Invalidate();

			// 정보 채우기
			char szBuffer[128];
			szBuffer[0] = '\0';

			sprintf(szBuffer,"맵 크기 : %dM x %dM", (int)(m_pControlMediator_->m_mapInfo.m_width/100), (int)(m_pControlMediator_->m_mapInfo.m_height/100) );
			GetDlgItem( IDC_STATIC_INFO_SIZE )->SetWindowText( szBuffer );

			sprintf(szBuffer,"맵 디테일 : %d x %d (Quad)", m_pControlMediator_->m_mapInfo.m_rowFace, m_pControlMediator_->m_mapInfo.m_colFace);
			GetDlgItem( IDC_STATIC_INFO_DETAIL )->SetWindowText( szBuffer );

			if (CXMLUserConfig::GetReference().IsCenterPivot())
			{
				m_ctlCheckCenterPivot.SetCheck(TRUE);
			}

			UpdateCameraControlInfo();
		}
		break;
	}
}

BOOL CRollupEtcMapView::OnInitDialog()
{
	CDialog::OnInitDialog();

	//spinBox
	m_ctrlSpinWSAD_.SetBuddy(&m_ctrEditWSAD_);
	m_ctrlSpinWSAD_.SetRange(0, 50000);
	m_ctrlSpinWSAD_.SetDelta(100);

	m_ctrlSpinWheel_.SetBuddy(&m_ctrEditWheel_);
	m_ctrlSpinWheel_.SetRange(0, 1000);
	m_ctrlSpinWheel_.SetDelta(1);

	m_ctrlSpinFOV_.SetBuddy(&m_ctrEditFOV_);
	m_ctrlSpinFOV_.SetRange(10, 120);
	m_ctrlSpinFOV_.SetDelta(1);

	m_ctrlSpinFarZ_.SetBuddy(&m_ctrEditFarZ_);
	m_ctrlSpinFarZ_.SetRange(1000, 300000);
	m_ctrlSpinFarZ_.SetDelta(10000);

	CString cstrPosName;
	for( int i = 0; i < rs3::RToolCamera::DEFAULT_CAMERA; ++i)
	{
		cstrPosName.Format(" %d 비어있음", i + 1);
		m_ctlListCamLocation_.AddString(cstrPosName);
	}
	m_ctlListCamLocation_.AddString("기본 카메라 위치");
	return TRUE;
}

void CRollupEtcMapView::OnEnChangeEditCamSpeed()
{
	m_pControlMediator_->m_cameraInfo.KEY_STEP = m_ctrlSpinWSAD_.GetPos();
	m_pControlMediator_->m_cameraInfo.WHEEL_ZOOM_STEP = m_ctrlSpinWheel_.GetPos();
}

void CRollupEtcMapView::OnBnClickedCheckCenterPivot()
{
	CXMLUserConfig::GetReference().SetCenterPivot( m_ctlCheckCenterPivot.GetCheck() );
}

void CRollupEtcMapView::UpdateCameraLocationList()
{
	rs3::RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();

	CString cstrPosName;
	m_ctlListCamLocation_.ResetContent();
	for( int i = 0; i < rs3::RToolCamera::DEFAULT_CAMERA; ++i)
	{
		const rs3::RToolCamera::LOCATION_INFO* pLocationInfo = pCamera->GetLocation(i);
		if( pLocationInfo )
		{
			cstrPosName.Format(" %d 기억되어 있음", i + 1);
			m_ctlListCamLocation_.AddString(cstrPosName);
		}
		else
		{
			cstrPosName.Format(" %d 비어있음", i + 1);
			m_ctlListCamLocation_.AddString(cstrPosName);
		}
	}
	m_ctlListCamLocation_.AddString("기본 카메라 위치");
}

void CRollupEtcMapView::OnBnClickedButtonCamSave()
{
	int nCurIndex = m_ctlListCamLocation_.GetCurSel();
	m_pControlMediator_->GetToolCamera()->RememberLocation(nCurIndex);
	UpdateCameraLocationList();
}

void CRollupEtcMapView::OnBnClickedButtonCamDelete()
{
	int nCurIndex = m_ctlListCamLocation_.GetCurSel();
	if(nCurIndex != -1)
	{
		m_pControlMediator_->GetToolCamera()->ResetRememberLocation(nCurIndex);
		UpdateCameraLocationList();
	}
}

void CRollupEtcMapView::OnLbnDblclkListCamPos()
{
	int nCurIndex = m_ctlListCamLocation_.GetCurSel();
	rs3::RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();
	const rs3::RToolCamera::LOCATION_INFO* pLocationInfo = pCamera->GetLocation(nCurIndex);
	pCamera->SetLocation(pLocationInfo);
}

LRESULT CRollupEtcMapView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CDialog::WindowProc(message, wParam, lParam);

	switch(message) 
	{
		case WM_ERASEBKGND:
			return FALSE;

		case WM_DESTROY:
		{
			m_miniMapDib_.Invalidate();
			return lResult;
		}

		case WM_PAINT:
		{
			if(m_pControlMediator_ != NULL && m_pControlMediator_->m_mapInfo.m_bLoaded )
			{
				if (m_miniMapDib_.IsValid() == FALSE)
				{
					if (m_bTryCreateMinimap_ == false)
					{
						CString cstrSaved(m_pControlMediator_->m_mapInfo.m_strSceneFile.c_str());
						cstrSaved.Replace(".zone.xml", ".bmp");
						CString cstrScreenshotPath(CXMLResourceList::GetReference().GetZoneUtilsPath());
						cstrSaved = cstrScreenshotPath + cstrSaved;
						CFile ImgFile;
						if(ImgFile.Open(cstrSaved, CFile::modeRead))
						{
							m_miniMapDib_.Read(ImgFile);
							ImgFile.Close();
							Invalidate();
						}
						m_bTryCreateMinimap_ = true;
					}
				}
				else
				{
					CRect DesRect, SrcRect;
					CWnd* pWnd = GetDlgItem(IDC_STATIC_MINIMAP_VIEW);
					pWnd->GetClientRect(&DesRect);

					int img_width = (int)m_miniMapDib_.m_pBMI->bmiHeader.biWidth;
					int img_height = (int)m_miniMapDib_.m_pBMI->bmiHeader.biHeight;

					float view_ratio = ((float)DesRect.Width())/((float)DesRect.Height());
					float img_ration = ((float)img_width)/((float)img_height);

					SrcRect.left = 0;
					SrcRect.top = 0;
					SrcRect.right = m_miniMapDib_.m_pBMI->bmiHeader.biWidth;
					SrcRect.bottom = m_miniMapDib_.m_pBMI->bmiHeader.biHeight;
					if(view_ratio < img_ration) //너비를 기준으로
					{
						float new_height = ((float)img_height) * ((float)DesRect.Width())/((float)img_width);
						float gap = ((float)(DesRect.Height() - new_height))/2;
						DesRect.top = DesRect.top + (int)gap;
						DesRect.bottom = DesRect.top + (int)(new_height);
						/*DesRect.right = DesRect.right;
						DesRect.left = DesRect.left;*/
					}
					else
					{
						float new_width = ((float)img_width) * ((float)DesRect.Height())/((float)img_height);
						float gap = ((float)(DesRect.Width() - new_width))/2;
						DesRect.left = DesRect.left + (int)gap;
						DesRect.right = DesRect.left + (int)(new_width);
						/*DesRect.top = DesRect.top;
						DesRect.bottom = DesRect.bottom;*/
					}
					CDC* pDC = pWnd->GetDC();
					m_miniMapDib_.Paint(pDC->GetSafeHdc(), &DesRect, &SrcRect);
				}
			}
		}
		return FALSE;
	}
	return lResult;
}

void CRollupEtcMapView::OnBnClickedButtonUpdateMinimap()
{
	if(m_pControlMediator_->m_mapInfo.m_bLoaded == false)
		return;

	// 미니맵 찍기
	CControlMediatorHelperResource::MINI_MAP eMiniMapType = (m_ctlCheckKeepCurrentCamera.GetCheck() ? CControlMediatorHelperResource::MINI_NONE_CAMERA_SET : CControlMediatorHelperResource::MINI_CREATE);
	m_pControlMediator_->GetHelperResource()->CreateMiniMapAndSetCamera("_minimap.tmp", eMiniMapType);

	// 미니맵 로딩
	CFile ImgFile;
	CString cstrTemp(m_pControlMediator_->m_mapInfo.m_strScenePath.c_str());
	cstrTemp += "_minimap.tmp";
	if(ImgFile.Open(cstrTemp, CFile::modeRead))
	{
		m_miniMapDib_.Read(ImgFile);
		ImgFile.Close();
		Invalidate();
	}

	CString cstrSaved(m_pControlMediator_->m_mapInfo.m_strSceneFile.c_str());
	cstrSaved.Replace(".zone.xml", ".bmp");

	CString cstrScreenshotPath(CXMLResourceList::GetReference().GetZoneUtilsPath());

	cstrSaved = cstrScreenshotPath + cstrSaved;
	if(GetFileAttributes(cstrTemp) != INVALID_FILE_ATTRIBUTES)
		CopyFile(cstrTemp, cstrSaved, FALSE);
}

void CRollupEtcMapView::OnBnClickedButtonOpenResMgr()
{
	CDlgResourceInfo* pDlg = m_pControlMediator_->GetViewPtr<CDlgResourceInfo>();

	CDlgResourceInfo::MAP_RES_TYPE eResType = CDlgResourceInfo::RES_ALL;
	if( m_pControlMediator_->m_mapInfo.m_bLoaded )
	{
		eResType = m_pControlMediator_->m_mapInfo.m_bSceneEditableMap ? CDlgResourceInfo::RES_ZONE : CDlgResourceInfo::RES_FIELD;
	}

	pDlg->SetResourceType(eResType);
	pDlg->Listen( CoreMessage(VIEW_MESSAGE::ACTIVE_VIEW) );
}

void CRollupEtcMapView::OnBnClickedCheckViewTerrainInfo()
{
	bool bChecked = ( m_ctlCheckTerrainInfo_.GetCheck() == TRUE );
	if( bChecked != m_pControlMediator_->GetHelperRender()->GetMapDetailInfoState() )
	{
		m_pControlMediator_->GetHelperRender()->ToggleMapDetailInfo();
	}
}

void CRollupEtcMapView::OnBnClickedButtonProfile()
{
	m_pControlMediator_->GetHelperRender()->SetVisibleProfile();
}

void CRollupEtcMapView::UpdateViewCheckerButton()
{
	// water
	rs3::RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();
	if(!pCamera) return;

	// terrain
	bool bTerrainInfo = m_pControlMediator_->GetHelperRender()->GetMapDetailInfoState();
	m_ctlCheckTerrainInfo_.SetCheck( bTerrainInfo ? TRUE : FALSE );
}

void CRollupEtcMapView::UpdateCameraControlInfo()
{
	float fKeyStep = m_pControlMediator_->m_cameraInfo.KEY_STEP;
	float fZoomStep = m_pControlMediator_->m_cameraInfo.WHEEL_ZOOM_STEP;
	float fFarZ = m_pControlMediator_->m_cameraInfo.FAR_Z;
	m_ctrlSpinWSAD_.SetPos( fKeyStep );
	m_ctrlSpinWheel_.SetPos( fZoomStep );
	m_ctrlSpinFarZ_.SetPos(fFarZ);

	OnEnChangeEditCamFov();
	OnEnChangeEditCamFarZ();
}

void CRollupEtcMapView::OnEnChangeEditCamFov()
{
	float fFov = m_ctrlSpinFOV_.GetPos();
	if (fFov <= 0.f)
		return;

	rs3::RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();
	if (pCamera)
	{
		float fFovX = DegreeToRadian( fFov );

		// 상수로 기술된 4:3 기준 FOV 를 Y 기준으로 구한다
		float fFovY = atanf(tanf(fFovX/2.0f)/ 1.33333f )*2.0f;

		// 해당 FOVY 에 맞는 새 FOV 를 구한다
		int nWidth = rs3::REngine::GetConfig().m_nWidth;
		int nHeight = rs3::REngine::GetConfig().m_nHeight;

		float fAspectRatio = float(nWidth) / float(nHeight);
		float fNewFovX = atanf(tanf(fFovY/2.0f)*fAspectRatio)*2.0f;

		pCamera->SetFov(fNewFovX);
	}
}

void CRollupEtcMapView::OnEnChangeEditCamFarZ()
{
	float fFarZ = m_ctrlSpinFarZ_.GetPos();
	if (fFarZ <= 0.f)
		return;

	m_pControlMediator_->m_cameraInfo.FAR_Z = fFarZ;
	rs3::RToolCamera* pCamera = m_pControlMediator_->GetToolCamera();
	if (pCamera)
	{
		pCamera->SetNearFarZ( pCamera->GetNearZ(), m_pControlMediator_->m_cameraInfo.FAR_Z);
	}
}

void CRollupEtcMapView::OnBnClickedButtonFovToolvalue()
{
	m_ctrlSpinFOV_.SetPos( XCONST::CAMERA_FOV );
	OnEnChangeEditCamFov();
}

void CRollupEtcMapView::OnBnClickedButtonFovGamevalue()
{
	m_ctrlSpinFOV_.SetPos( XCONST::GAME_CAMERA_FOV );
	OnEnChangeEditCamFov();
}

void CRollupEtcMapView::OnBnClickedButtonFarzToolvalue()
{
	m_ctrlSpinFarZ_.SetPos( XCONST::CAMERA_FAR_Z );
	OnEnChangeEditCamFarZ();
}

void CRollupEtcMapView::OnBnClickedButtonFarzGamevalue()
{
	m_ctrlSpinFarZ_.SetPos( XCONST::GAME_CAMERA_FAR_Z );
	OnEnChangeEditCamFarZ();
}

void CRollupEtcMapView::OnBnClicked1st3rdPerson()
{
	CXMLUserConfig::GetReference().Set1st3rdPerson( m_ctl1st3rdPerson.GetCheck() == TRUE );
}

void CRollupEtcMapView::OnBnClickedButtonTopviewNavi()
{
	if(false == m_pControlMediator_->IsWBNavigation())
	{
		m_pControlMediator_->GetHelperAction()->SetNavigationModeToggle(true);
		m_pControlMediator_->GetHelperAction()->SetNavigationModeToggle(false);
	}
}
