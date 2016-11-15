// V_EditTerrainSplat.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VR_TerrainSplat.h"

#include "C_ControlMediator.h"
#include "M_ToolCmdMgr.h"
#include "C_XMLConfig.h"
#include "C_XMLResourceList.h"

#include "VL_ResourceInfo.h"
#include "M_ToolTerrain.h"
#include "RDirectionalLightSceneNode.h"

#include "V_MFCHelper.h"
#include "ext_lib/WM_USER.h"
// CRollupTerrainSplat 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupTerrainSplat, CDialog)
CRollupTerrainSplat::CRollupTerrainSplat(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupTerrainSplat::IDD, pParent)
	, m_fTileU(0.0f)
	, m_fTileV(0.0f)
	, m_nCurrentLayer(-1)
	, m_dwLayerWidth(0)
	, m_dwLayerHeight(0)
	, m_bMarkingLayer(false)
{
	m_pTerrain_ = NULL;
}

CRollupTerrainSplat::~CRollupTerrainSplat()
{
}

BOOL CRollupTerrainSplat::OnInitDialog()
{
	CDialog::OnInitDialog();

	// UI Initialize
	CMFCHelper::RegisterBrushWorkArea(&m_sliderCtrlSize, &m_fEditSize, &m_cstrViewSize);

	CMFCHelper::SetMFCSlider(&m_sliderCtrlStr, 1, 128, 0);
	m_fStr = (float)m_sliderCtrlStr.GetPos()*0.5f;

	//spinBox
	m_ctrlSpinU_.SetBuddy(GetDlgItem(IDC_EDIT_U));
	m_ctrlSpinU_.SetRange(1, 1000);

	m_ctrlSpinV_.SetBuddy(GetDlgItem(IDC_EDIT_V));
	m_ctrlSpinV_.SetRange(1, 1000);

	m_ctlAlphaChannelView.SetCheck(FALSE);
	m_ctlTileInfo.SetWindowText("");

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CRollupTerrainSplat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SLIDER_SPLAT_AREA, m_sliderCtrlSize);
	DDX_Control(pDX, IDC_SLIDER_SPLAT_STR, m_sliderCtrlStr);
	DDX_Control(pDX, IDC_LIST_TILE, m_ctlLayer);

	DDX_Text(pDX, IDC_EDIT_SPLAT_AREA, m_cstrViewSize);
	DDX_Text(pDX, IDC_EDIT_SPLAT_STR, m_fStr);

	DDX_Text(pDX, IDC_EDIT_U, m_fTileU);
	DDX_Text(pDX, IDC_EDIT_V, m_fTileV);
	
	DDX_Control(pDX, IDC_SPIN_SPLAT_U, m_ctrlSpinU_);
	DDX_Control(pDX, IDC_SPIN_SPLAT_V, m_ctrlSpinV_);
	DDX_Text(pDX, IDC_EDIT_SPLAT_WIDTH, m_dwLayerWidth);
	DDX_Text(pDX, IDC_EDIT_SPLAT_HEIGHT, m_dwLayerHeight);
	DDX_Control(pDX, IDC_CHECK_USE_MARKING, m_ctlLayerMarking);

	DDX_Control(pDX, IDC_EDIT_TILEINFO, m_ctlTileInfo);
	DDX_Control(pDX, IDC_CHECK_ALPHAVIEW, m_ctlAlphaChannelView);
}


BEGIN_MESSAGE_MAP(CRollupTerrainSplat, CDialog)
	ON_WM_HSCROLL()
	//ON_WM_PAINT()

	ON_BN_CLICKED(IDC_BUTTON_NEW_LAYER, &CRollupTerrainSplat::OnBnClickedButtonNewLayer)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, &CRollupTerrainSplat::OnBnClickedButtonChangeLayer)
	ON_BN_CLICKED(IDC_BUTTON_DEL,&CRollupTerrainSplat::OnBnClickedButtonDeleteLayer)

	ON_LBN_SELCHANGE(IDC_LIST_TILE, &CRollupTerrainSplat::OnLbnSelchangeListTile)
	ON_CLBN_CHKCHANGE(IDC_LIST_TILE, &CRollupTerrainSplat::OnCheckChangeListTile)
	ON_EN_CHANGE(IDC_EDIT_U, &CRollupTerrainSplat::OnEnChangeEditUV)
	ON_EN_CHANGE(IDC_EDIT_V, &CRollupTerrainSplat::OnEnChangeEditUV)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CRollupTerrainSplat::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CRollupTerrainSplat::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_RESIZE_UP, &CRollupTerrainSplat::OnBnClickedButtonResizeUp)
	ON_BN_CLICKED(IDC_BUTTON_RESIZE_DOWN, &CRollupTerrainSplat::OnBnClickedButtonResizeDown)
	ON_BN_CLICKED(IDC_CHECK_USE_MARKING, &CRollupTerrainSplat::OnBnClickedCheckUseMarking)
	ON_BN_CLICKED(IDC_LAYER_REBUILD, &CRollupTerrainSplat::OnBnClickedLayerRebuild)

	ON_BN_CLICKED(IDC_CHECK_ALPHAVIEW, &CRollupTerrainSplat::OnBnClickButtonAlphaView)
	ON_STN_CLICKED(IDC_STATIC_SPEC, &CRollupTerrainSplat::OnStnClickedStaticSpec)
END_MESSAGE_MAP()

using namespace rs3;

LRESULT CRollupTerrainSplat::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CDialog::WindowProc(message, wParam, lParam);

	switch(message) 
	{
		case WM_ERASEBKGND:
			return FALSE;

		case WM_PAINT:
		case WM_USER_SIDEPANEL_EXPANDED:
		case WM_USER_REBAR_EXPANDED:
		{
			OnLbnSelchangeListTile();
		}
		return FALSE;
	}
	
	return lResult;
}


int CRollupTerrainSplat::SelectLayer( int nLayer, bool bTileSelected)
{
	if( nLayer >= 0)
		return m_ctlLayer.SetCurSel( nLayer*2 + (bTileSelected ? 0 : 1));
	else
		return m_ctlLayer.SetCurSel(-1);
}

int CRollupTerrainSplat::GetSelectedLayer()
{
	int nSel = m_ctlLayer.GetCurSel();
	int nLayer = nSel/2;
	return nLayer;
}

bool CRollupTerrainSplat::IsTileSelected()
{
	int nSel = m_ctlLayer.GetCurSel();
	if( nSel % 2 == 0)
		return true;
	else
		return false;
}

void CRollupTerrainSplat::InsertLayerListTilemap( int nIdx, int nLayer)
{
	if( m_pTerrain_ == NULL)
		return;

	char strListName[256];
	_snprintf( strListName, 255, "%2d.T:%s", nLayer+1, m_pTerrain_->GetTilemapName(nLayer));
	m_ctlLayer.InsertString( nIdx, strListName);
	BOOL bCheck = m_pTerrain_->IsSpecularAlphaMask(nLayer) ? TRUE : FALSE;
	m_ctlLayer.SetCheck( nIdx, bCheck);

}

void CRollupTerrainSplat::InsertLayerListNormalmap( int nIdx, int nLayer)
{
	if( m_pTerrain_ == NULL)
		return;

	char strListName[256];
	_snprintf( strListName, 255, "---N:%s", m_pTerrain_->GetNormalmapName(nLayer));
	m_ctlLayer.InsertString( nIdx, strListName);
	BOOL bCheck = m_pTerrain_->GetNormalTexture(nLayer) ? TRUE : FALSE;
	m_ctlLayer.SetCheck( nIdx, bCheck);

}

void CRollupTerrainSplat::UpdateLayerList( int _nSelectIndexAfterUpdate, bool _bTilemapSelectedAfterUpdate)
{
	if(!m_pControlMediator_->m_mapInfo.m_bLoaded || m_pTerrain_ == NULL)
		return;

	m_ctlLayer.ResetContent();
	for ( int i = 0; i<m_pTerrain_->GetLayerNum(); ++i )
	{
		InsertLayerListTilemap( i*2, i);
		InsertLayerListNormalmap( i*2+1, i);
	}

	//선택이 되어 있지 않다면, 처음걸로 자동 선택
	if(_nSelectIndexAfterUpdate == -1)
		_nSelectIndexAfterUpdate = 0;
	SelectLayer( _nSelectIndexAfterUpdate, _bTilemapSelectedAfterUpdate);
	OnLbnSelchangeListTile();
}

void CRollupTerrainSplat::Listen(CoreMessage& _message)
{
	switch ( _message.nID )
	{
	case VIEW_MESSAGE::ACTIVE_VIEW :
		m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::TERRAIN_SPLAT);
	break;
	case VIEW_MESSAGE::SCENE_LOADED :
		m_pTerrain_ = (rs3::RToolTerrain*) m_pControlMediator_->GetToolSceneManager()->GetCurrentTerrain();
		UpdateLayerList();
	break;
	}	
}

// CRollupTerrainSplat 메시지 처리기입니다.

void CRollupTerrainSplat::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar){
	int size = m_sliderCtrlSize.GetPos();
	CMFCHelper::SetBrushWorkAreaSize(size);

	// 세기
	m_fStr = (float)m_sliderCtrlStr.GetPos() * 0.5f;

	UpdateData(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRollupTerrainSplat::OnBnClickedButtonNewLayer()
{
	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	ASSERT(pSceneManager!=NULL);
	rs3::RToolTerrain* pTerrain = (rs3::RToolTerrain* ) pSceneManager->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	int nSel = GetSelectedLayer();
	bool bTile = IsTileSelected();

	CDlgResourceInfo dlg(m_pControlMediator_, CDlgResourceInfo::RES_TILE);
	if (dlg.DoModal()==IDOK )
	{
		if(dlg.m_strSelectedTile.empty())
			return;

		pTerrain->AddLayer( dlg.m_strSelectedTile.c_str());
		//변경통보
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		//UI Update

		UpdateLayerList( nSel, bTile);
	}
}

void CRollupTerrainSplat::OnBnClickedButtonChangeLayer()
{
	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	ASSERT(pSceneManager!=NULL);
	RToolTerrain* pTerrain = (RToolTerrain*) pSceneManager->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	int nSel = GetSelectedLayer();
	bool bTile = IsTileSelected();
	if ( nSel < 0 )
		AfxMessageBox("레이어를 선택해주십시오.");

	CDlgResourceInfo dlg(m_pControlMediator_, CDlgResourceInfo::RES_TILE);
	if (dlg.DoModal()==IDOK )
	{
		if(dlg.m_strSelectedTile.empty())
			return;

		if( IsTileSelected())
			pTerrain->ChangeTileMap(nSel, dlg.m_strSelectedTile.c_str());
		else
			pTerrain->ChangeNormalMap(nSel, dlg.m_strSelectedTile.c_str());
		//변경통보
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		//UI Update
		UpdateLayerList( nSel, bTile);
	}
}


void CRollupTerrainSplat::DeleteLayer( int nLayer)
{
	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	ASSERT(pSceneManager!=NULL);
	RToolTerrain* pTerrain = (RToolTerrain*) pSceneManager->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	pTerrain->DeleteLayer(nLayer);
	//변경통보
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	//UI Update
	SelectLayer( nLayer, false);
	UpdateLayerList();
}

void CRollupTerrainSplat::DeleteNormalMap( int nLayer)
{
	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	ASSERT(pSceneManager!=NULL);
	RToolTerrain* pTerrain = (RToolTerrain*) pSceneManager->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	pTerrain->DeleteNormalMap(nLayer);
	//변경통보
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	//UI Update
	SelectLayer( nLayer, true);
	//노말맵을 없앴으므로 타일맵에 커서가 가게 한다.
	UpdateLayerList( nLayer, false);
}


void CRollupTerrainSplat::OnBnClickedButtonDeleteLayer()
{
	if ( GetSelectedLayer() < 0 )
	{
		AfxMessageBox("레이어를 선택해주십시오.");
		return;
	}

	if( IsTileSelected()) 
	{
		if ( AfxMessageBox("레이어를 삭제하시겠습니까?", MB_YESNO) == IDYES )
		{
			DeleteLayer( GetSelectedLayer());
		}
	}
	else
	{
		if ( AfxMessageBox("노말맵 사용을 안하시겠습니까?", MB_YESNO) == IDYES )
		{
			DeleteNormalMap( GetSelectedLayer());
		}
	}
}


bool CRollupTerrainSplat::IsAlphaTexture( const RTextureInfo* pTexInfo)
{
	if( !pTexInfo)
		return false;

	bool bAlphaTexture = false;
	switch ( pTexInfo->pixelFormat )
	{
	case RFMT_ARGB32:
	case RFMT_A8R8G8B8:
	case RFMT_A2R10G10B10:
		bAlphaTexture = true;
		break;
	default:
		bAlphaTexture = false;
	}

	return bAlphaTexture;
}

void CRollupTerrainSplat::SetTileInfoText( const RTextureInfo* pTexInfo)
{
	char strBuffer[128];
	strBuffer[0] = 0;
	if( pTexInfo)
		sprintf(strBuffer,"%d x %d x %c", pTexInfo->nTextureWidth, pTexInfo->nTextureHeight, IsAlphaTexture( pTexInfo) ? 'A' : 'N');
	else
		sprintf(strBuffer,"N/A");
	m_ctlTileInfo.SetWindowText(strBuffer);
}


void CRollupTerrainSplat::SelchangeListTilemap( int nLayer, RToolTerrain* pTerrain, CDC *pDC, HDC hDC, RECT rt, RDeviceD3D *pd3ddevice)
{
	RTexture* hTileTexture = pTerrain->GetTileTexture(nLayer);
	if( hTileTexture)
	{
		LPDIRECT3DBASETEXTURE9 lpTexture = pd3ddevice->GetD3DTexture( hTileTexture);
		_ASSERT(lpTexture->GetType()==D3DRTYPE_TEXTURE);
		const RTextureInfo* pTexInfo = &hTileTexture->GetInfo();

		// 텍스쳐 정보를 기록한다.
		SetTileInfoText( pTexInfo);

		// 알파채널 CheckBox에 따른 처리
		if( IsAlphaTexture( pTexInfo))
		{
			m_ctlAlphaChannelView.EnableWindow(TRUE);
			CMFCHelper::D3DTextureToHDC(hDC,(LPDIRECT3DTEXTURE9)lpTexture,&rt, m_ctlAlphaChannelView.GetCheck() ? true : false);
		}
		else
		{
			m_ctlAlphaChannelView.EnableWindow(FALSE);
			CMFCHelper::D3DTextureToHDC(hDC,(LPDIRECT3DTEXTURE9)lpTexture,&rt, false);
		}
	}
}


void CRollupTerrainSplat::SelchangeListNormalmap( int nLayer, RToolTerrain* pTerrain, CDC *pDC, HDC hDC, RECT rt, RDeviceD3D *pd3ddevice)
{
	RTexture* hNormalTexture = pTerrain->GetNormalTexture(nLayer);
	LPDIRECT3DBASETEXTURE9 lpTexture = NULL;
	if( hNormalTexture)	
	{
		lpTexture = pd3ddevice->GetD3DTexture( hNormalTexture);
		_ASSERT(lpTexture->GetType()==D3DRTYPE_TEXTURE);
		// 텍스쳐 정보를 기록한다.
		SetTileInfoText( &hNormalTexture->GetInfo());
	}
	else
	{
		/// 노말맵이 없을 시는 디폴트 텍스쳐를 뿌려주도록 하자.
		lpTexture = pd3ddevice->GetD3DTexture( pd3ddevice->GetDefaultTexture());
		// 텍스쳐 정보를 없는 것으로 기록한다.
		SetTileInfoText( NULL);
	}
	

	m_ctlAlphaChannelView.EnableWindow(FALSE);
	CMFCHelper::D3DTextureToHDC(hDC,(LPDIRECT3DTEXTURE9)lpTexture,&rt, false);
}

void CRollupTerrainSplat::OnLbnSelchangeListTile()
{
	int nSel = GetSelectedLayer();
	if ( nSel < 0 )		return;

	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	ASSERT(pSceneManager!=NULL);
	RToolTerrain* pTerrain = (RToolTerrain*) pSceneManager->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	// 리스트 박스를 선택하였을 때 ...
	CWnd *pWnd = GetDlgItem(IDC_PREVIEW);
	CDC *pDC = pWnd->GetDC();
	if ( pDC == NULL )
		return;

	HDC hDC = pDC->GetSafeHdc();
	if ( hDC == NULL )
		return;

	RECT rt;
	pWnd->GetClientRect(&rt);

	// Preview 창에 텍스쳐를 출력 위한 Device 겟
	RDeviceD3D *pd3ddevice = m_pControlMediator_->GetCurrentDevice();
	ASSERT(pd3ddevice);

	// 두줄이 한 쌍으로 레이어를 가리킨다.
	int nLayer = GetSelectedLayer();
	if( IsTileSelected())
		SelchangeListTilemap( nLayer, pTerrain, pDC, hDC, rt, pd3ddevice);
	else
		SelchangeListNormalmap( nLayer, pTerrain, pDC, hDC, rt, pd3ddevice);

	pWnd->ReleaseDC(pDC);

	// 마킹 레이어를 세팅한다.
	if (m_bMarkingLayer)
		m_pTerrain_->SetMarkingLayerNumber(nLayer);

	const RVector *pTileScale = pTerrain->GetTileScale(nLayer);
	const DWORD dwWidth = pTerrain->GetAlphaTextureWidth(nLayer);
	const DWORD dwHeight = pTerrain->GetAlphaTextureHeight(nLayer);


	// 여러가지 데이터를 넣는다.
	m_nCurrentLayer = nLayer;
	m_fTileU = pTileScale->x;
	m_fTileV = pTileScale->y;
	m_dwLayerWidth = dwWidth;
	m_dwLayerHeight = dwHeight;

	UpdateData(FALSE);
}

void CRollupTerrainSplat::OnEnChangeEditUV()
{
	UpdateData(TRUE);

	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	ASSERT(pSceneManager!=NULL);
	RToolTerrain* pTerrain = (RToolTerrain*) pSceneManager->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	int nSel = GetSelectedLayer();
	if ( nSel < 0 )		return;

	if ( m_fTileU < 0.01f )	m_fTileU = 0.01f;
	if ( m_fTileV < 0.01f ) m_fTileV = 0.01f;

	UpdateData(FALSE);

	RVector vTileScale(m_fTileU, m_fTileV, 0.0f);
	pTerrain->SetTiling(nSel, vTileScale);
	//변경통보
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
}

void CRollupTerrainSplat::OnBnClickedButtonUp()
{
	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	ASSERT(pSceneManager!=NULL);
	RToolTerrain* pTerrain = (RToolTerrain*) pSceneManager->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;
	
	bool bTileSelected = IsTileSelected();
	int iSelLayerA = GetSelectedLayer();
	int iSelLayerB = iSelLayerA - 1;
	if ( iSelLayerA < 0 )
		return;
	if ( iSelLayerA >= pTerrain->GetLayerNum())
		return;

	bool bResultSwapLayer = pTerrain->SwapLayer(iSelLayerA, iSelLayerB);
	if ( bResultSwapLayer == false )
	{
		OutputDebugString("레이어 교환 실패\n");
	}
	else
	{
		UpdateLayerList();
		SelectLayer( iSelLayerB, bTileSelected);
		OnLbnSelchangeListTile();
		//변경통보
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}
}

void CRollupTerrainSplat::OnBnClickedButtonDown()
{
	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	ASSERT(pSceneManager!=NULL);
	RToolTerrain* pTerrain = (RToolTerrain*) pSceneManager->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	bool bTileSelected = IsTileSelected();
	int iSelLayerA = GetSelectedLayer();
	int iSelLayerB = iSelLayerA + 1;
	if ( iSelLayerA < 0 )
		return;
	if ( iSelLayerA >= pTerrain->GetLayerNum())
		return;

	bool bResultSwapLayer = pTerrain->SwapLayer(iSelLayerA, iSelLayerB);
	if ( bResultSwapLayer == false )
	{
		OutputDebugString("레이어 교환 실패\n");
	}
	else
	{
		UpdateLayerList();
		SelectLayer( iSelLayerB, bTileSelected);
		OnLbnSelchangeListTile();
		//변경통보
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}
}

void CRollupTerrainSplat::OnCancel()
{
	return;
}
void CRollupTerrainSplat::OnOK()
{
	return;
}

void CRollupTerrainSplat::OnBnClickedButtonResizeUp()
{
	int nSel = GetSelectedLayer();
	if ( nSel < 0 )		return;

	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	ASSERT(pSceneManager!=NULL);
	RToolTerrain* pTerrain = (RToolTerrain*) pSceneManager->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	if ( pTerrain->ResizeLayerAlphaMul2X(nSel))
	{
		m_dwLayerWidth = pTerrain->GetAlphaTextureWidth(nSel);
		m_dwLayerHeight = pTerrain->GetAlphaTextureHeight(nSel);

		//변경통보
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		//UI Update
		UpdateData(FALSE);
	}
}

void CRollupTerrainSplat::OnBnClickedButtonResizeDown()
{
	int nSel = GetSelectedLayer();
	if ( nSel < 0 )		return;

	RToolSceneManager *pSceneManager = m_pControlMediator_->GetToolSceneManager();
	ASSERT(pSceneManager!=NULL);
	RToolTerrain* pTerrain = (RToolTerrain*) pSceneManager->GetCurrentTerrain();
	if ( pTerrain == NULL )
		return;

	if ( pTerrain->ResizeLayerAlphaDiv2X(nSel))
	{
		m_dwLayerWidth = pTerrain->GetAlphaTextureWidth(nSel);
		m_dwLayerHeight = pTerrain->GetAlphaTextureHeight(nSel);

		//변경통보
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		//UI Update
		UpdateData(FALSE);
	}
}


void CRollupTerrainSplat::CheckAlphaMaskForSpecular()
{
	if(!IsTileSelected())
		return;
	
	int nSel = m_ctlLayer.GetCurSel();
	if ( nSel<0)
		return;
	bool bCheck = m_ctlLayer.GetCheck(nSel) ? true : false;

	m_pTerrain_->SetAlphaMaskForSpecular( GetSelectedLayer(), bCheck);
}

void CRollupTerrainSplat::CheckUseNormalmap()
{
	if( IsTileSelected())
		return;

	int nSel = m_ctlLayer.GetCurSel();
	if ( nSel<0)		return;

	bool bCheck = m_ctlLayer.GetCheck(nSel) ? true : false;

	if( bCheck)
		/// 체크하면 사용하는 것이므로 파일을 선택하게 한다.
		OnBnClickedButtonChangeLayer();
	else
		/// 체크 풀면 사용 안하는 것.
		DeleteNormalMap( GetSelectedLayer());

	//m_pTerrain_->SetAlphaMaskForSpecular( GetSelectedLayer(), bCheck);
}


void CRollupTerrainSplat::OnCheckChangeListTile()
{
	if( IsTileSelected())
		CheckAlphaMaskForSpecular();
	else
		CheckUseNormalmap();
}

void CRollupTerrainSplat::OnBnClickedCheckUseMarking()
{
	if (m_pTerrain_ == NULL) return;
	int nSel = GetSelectedLayer();

	m_bMarkingLayer = m_ctlLayerMarking.GetCheck() ? true : false;

	if (m_bMarkingLayer)
		m_pTerrain_->SetMarkingLayerNumber(nSel);
	else
		m_pTerrain_->SetMarkingLayerNumber(-1);
}

void CRollupTerrainSplat::OnBnClickedLayerRebuild()
{
	if (m_pTerrain_ == NULL) return;

	int nCount = m_pTerrain_->RebuildAlphaMask();
	
	TCHAR szBuffer[128];
	szBuffer[0] = 0;
	_stprintf(szBuffer,"%d 개의 AlphaPixel이 없어졌습니다.\n", nCount);

	::MessageBox(NULL,szBuffer,"Information", MB_ICONINFORMATION | MB_OK);
}

void CRollupTerrainSplat::OnBnClickButtonAlphaView()
{
	OnLbnSelchangeListTile();
}
void CRollupTerrainSplat::OnStnClickedStaticSpec()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
