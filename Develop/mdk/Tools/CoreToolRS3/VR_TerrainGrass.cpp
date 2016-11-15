// V_EditTerrainGrass.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VR_TerrainGrass.h"

#include "RGrassSystem.h"
#include "VD_DlgGrassListOpen.h"

#include "C_ControlMediator.h"
#include "C_XMLConfig.h"
#include "C_XMLXConst.h"
#include "C_XMLEditInfoFile.h"
#include "C_XMLResourceList.h"

#include "V_MFCHelper.h"

#include <algorithm>

// CRollupTerrainGrass 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRollupTerrainGrass, CDialog)

CRollupTerrainGrass::CRollupTerrainGrass(CWnd* pParent /*=NULL*/)
	: CDialog(CRollupTerrainGrass::IDD, pParent)
{
	m_currentTreeitemHandle_ = NULL;
	m_beforeTreeitemHandle_ = NULL;

	m_pTerrain_     = NULL;
	m_pGrassSystem_ = NULL;

	m_nEditPieceNum_ = 0;
	m_nEditPieceSize_ = 0;
	m_nCheck_ = 0;
}

CRollupTerrainGrass::~CRollupTerrainGrass()
{
}

BOOL CRollupTerrainGrass::OnInitDialog()
{
	CDialog::OnInitDialog();

	//맵 영역
	CMFCHelper::RegisterBrushWorkArea(&m_sliderCtrlSize, &m_fEditSize, &m_cstrViewSize);
	CMFCHelper::SetMFCSlider(&m_sliderCtrlStr, 1, 10, 1);
	m_fStr = (float)m_sliderCtrlStr.GetPos();

	CMFCHelper::SetMFCSlider(&m_sliderCtrlSpace, 1, 50, 1);
	m_nSpace = m_sliderCtrlSpace.GetPos();

	//Radio
	//m_nAddOrDelete = 0;

	//spinBox
	m_ctrlSpinSize_.SetBuddy(GetDlgItem(IDC_EDIT_GRASS_SIZE));
	m_ctrlSpinSize_.SetRange(1, 1000);

	m_ctrlSpinNum_.SetBuddy(GetDlgItem(IDC_EDIT_GRASS_NUM));
	m_ctrlSpinNum_.SetRange(1, 1000);

	UpdateData(FALSE);

	return TRUE;  // return TRUE
}

void CRollupTerrainGrass::ResetTreeWithResource_()
{
	//HTREEITEM hParent = m_ctrlGrassBrushTree_.GetNextItem(NULL, TVGN_FIRSTVISIBLE);
	HTREEITEM hParent = m_ctrlGrassBrushTree_.GetRootItem();
	while (hParent != NULL)
	{
		HTREEITEM hChild = m_ctrlGrassBrushTree_.GetNextItem(hParent, TVGN_CHILD);
		while (hChild != NULL)
		{
			SPieceGrassInfo* pPieceGrass = (SPieceGrassInfo*)m_ctrlGrassBrushTree_.GetItemData(hChild);
			SAFE_DELETE(pPieceGrass);
			hChild = m_ctrlGrassBrushTree_.GetNextSiblingItem(hChild);
		}
		hParent = m_ctrlGrassBrushTree_.GetNextSiblingItem(hParent);
	}
	m_ctrlGrassBrushTree_.DeleteAllItems();
}

void CRollupTerrainGrass::OnDestroy()
{
	ResetTreeWithResource_();
	CDialog::OnDestroy();
}

void CRollupTerrainGrass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Radio(pDX, IDC_RADIO_GRASS_ADD, m_nAddOrDelete);
	DDX_Check(pDX, IDC_CHECK_GRASS_LOD, m_nCheck_);

	DDX_Control(pDX, IDC_SLIDER_GRASS_AREA, m_sliderCtrlSize);
	DDX_Control(pDX, IDC_SLIDER_GRASS_DENSITY, m_sliderCtrlStr);
	DDX_Control(pDX, IDC_SLIDER_GRASS_SPACE, m_sliderCtrlSpace);

	DDX_Text(pDX, IDC_EDIT_GRASS_AREA, m_cstrViewSize);
	DDX_Text(pDX, IDC_EDIT_GRASS_DENSITY, m_fStr);
	DDX_Text(pDX, IDC_EDIT_GRASS_SPACE, m_nSpace);

	DDX_Text(pDX, IDC_EDIT_GRASS_SIZE, m_nEditPieceSize_);
	DDX_Text(pDX, IDC_EDIT_GRASS_NUM, m_nEditPieceNum_);

	//spin
	DDX_Control(pDX, IDC_SPIN_GRASS_SIZE, m_ctrlSpinSize_);
	DDX_Control(pDX, IDC_SPIN_GRASS_NUM, m_ctrlSpinNum_);
	
	DDX_Control(pDX, IDC_TREE_GRASS_BRUSH, m_ctrlGrassBrushTree_);
}

BEGIN_MESSAGE_MAP(CRollupTerrainGrass, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_GRASS_LIST_EDIT, &CRollupTerrainGrass::OnBnClickedButtonGrassListEdit)
	ON_BN_CLICKED(IDC_BUTTON_GRASS_ON_TERRAIN, &CRollupTerrainGrass::OnBnClickedButtonGrassOnTerrain)
	//ON_BN_CLICKED(IDC_RADIO_GRASS_ADD, &CRollupTerrainGrass::OnBnClickedRadioGrassAdd)
	//ON_BN_CLICKED(IDC_RADIO_GRASS_DELETE, &CRollupTerrainGrass::OnBnClickedRadioGrassDelete)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_GRASS_BRUSH, &CRollupTerrainGrass::OnTvnSelchangedTreeGrassBrush)
	ON_BN_CLICKED(IDC_BUTTON_GRASS_ADD, &CRollupTerrainGrass::OnBnClickedButtonGrassAdd)
	ON_BN_CLICKED(IDC_BUTTON_P_GRASS_ADD, &CRollupTerrainGrass::OnBnClickedButtonPGrassAdd)
	ON_BN_CLICKED(IDC_BUTTON_GRASS_DELETE, &CRollupTerrainGrass::OnBnClickedButtonGrassDelete)
	//ON_BN_CLICKED(IDC_BUTTON_P_GRASS_DELETE, &CRollupTerrainGrass::OnBnClickedButtonPGrassDelete)

	ON_EN_CHANGE(IDC_EDIT_GRASS_NUM, OnEnChangeEditNumberSize)
	ON_EN_CHANGE(IDC_EDIT_GRASS_SIZE, OnEnChangeEditNumberSize)

	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_GRASS_SYNC, &CRollupTerrainGrass::OnBnClickedButtonGrassSync)
	ON_BN_CLICKED(IDC_CHECK_GRASS_LOD, &CRollupTerrainGrass::OnBnClickedCheckGrassLod)
END_MESSAGE_MAP()

// CRollupTerrainGrass 메시지 처리기입니다.

void CRollupTerrainGrass::Listen(CoreMessage& _message)
{
	if(_message.nID == VIEW_MESSAGE::ACTIVE_VIEW)
	{
		m_pControlMediator_->SetCurrentWBState(WORKBENCH_STATE::TERRAIN_GRASS);
	}
	else if (_message.nID == VIEW_MESSAGE::SCENE_LOADED)
	{
		DisableControl_(); //편집 가능하지 않도록 control 막기
		OnBnClickedCheckGrassLod(); //LOD 초기화

		//reset tree ctrl
		ResetTreeWithResource_();
		CXMLEditInfoFile::GetReference().LoadGrassInfo(&m_ctrlGrassBrushTree_);
		
		m_pTerrain_ = (rs3::RToolTerrain*) m_pControlMediator_->GetToolSceneManager()->GetCurrentTerrain();
		if(m_pTerrain_ == NULL)
		{
			m_pGrassSystem_ = NULL;
			return;
		}

		m_pGrassSystem_ = m_pTerrain_->GetGrassSystem();
		if(m_pGrassSystem_ == NULL) return;

		//grl 로딩
		STRING_CONTAINER vecStrGrassList;
		std::string zonePath(m_pControlMediator_->m_mapInfo.m_strScenePath + m_pControlMediator_->m_mapInfo.m_strSceneFile);
		int zone_ext_finename_pos = zonePath.find(".zone.xml");
		if( zone_ext_finename_pos == std::string::npos ) return;

		zonePath.replace(zone_ext_finename_pos, 9 /* length of .zone.xml */, "_grass.grl");
		if (CXMLResourceList::GetReference().LoadGrassList(zonePath.c_str(), vecStrGrassList))
		{
			//xml load test는 통과한 상태
			MXml kXml;
			kXml.LoadFile(zonePath.c_str());
			MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("ROOT");
			m_pGrassSystem_->LoadGrassListFromXML(*pRootElement, NULL, NULL);

			//Set Tree Control with Grass Info File
			VerifyTreeInfoAndGrassListInfo_(m_ctrlGrassBrushTree_, vecStrGrassList);

			//로딩된후에 활성화 될 control 활성화
			EnableControlAfterResourceSetted_();
		}
	}
}

void CRollupTerrainGrass::DisableControl_()
{
	GetDlgItem(IDC_EDIT_GRASS_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GRASS_NUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_GRASS_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_GRASS_NUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_P_GRASS_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GRASS_ADD)->EnableWindow(FALSE);

	GetDlgItem(IDC_BUTTON_GRASS_ON_TERRAIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GRASS_SYNC)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_GRASS_DELETE)->EnableWindow(FALSE);
}

void CRollupTerrainGrass::EnableControlAfterResourceSetted_()
{
	GetDlgItem(IDC_BUTTON_GRASS_ON_TERRAIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_GRASS_SYNC)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_GRASS_ADD)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_GRASS_DELETE)->EnableWindow(TRUE);
}

void CRollupTerrainGrass::OnCancel()
{
}
void CRollupTerrainGrass::OnOK()
{
}

void CRollupTerrainGrass::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar){

	//맵 편집 영역
	int size = m_sliderCtrlSize.GetPos();
	CMFCHelper::SetBrushWorkAreaSize(size);

	//세기
	m_fStr = (float)m_sliderCtrlStr.GetPos();

	//간격
	m_nSpace = m_sliderCtrlSpace.GetPos();

	UpdateData(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRollupTerrainGrass::OnBnClickedButtonGrassListEdit()
{
	rs3::RDeviceD3D* pRDevice = m_pControlMediator_->GetCurrentDevice();
	if(pRDevice)
	{
		if(m_pTerrain_ == NULL)
		{
			AfxMessageBox("로딩된 지형이 없습니다.");
			return; 
		}

		if(m_pGrassSystem_ == NULL)
		{
			AfxMessageBox("생성된 풀 관리자 시스템이 없습니다.");
			return;
		}
		
		//LPDIRECT3DDEVICE9 pDevice = pRDevice->GetD3DDevice();
		std::string zonePath(m_pControlMediator_->m_mapInfo.m_strScenePath + m_pControlMediator_->m_mapInfo.m_strSceneFile);
		CDlgGrassListOpen dlg(CDlgGrassListOpen::GRL_MAKE_OPEN, zonePath.c_str(), m_pGrassSystem_, pRDevice);
		dlg.DoModal();

		//변경여부 확인
		if(dlg.m_bChanged)
		{
			EnableControlAfterResourceSetted_();
			VerifyTreeInfoAndGrassListInfo_(m_ctrlGrassBrushTree_, dlg.m_vecGrassPieceName);
		}
	}
}

void CRollupTerrainGrass::OnBnClickedButtonGrassOnTerrain()
{
	if(m_pTerrain_ == NULL) return;
	if(m_pGrassSystem_ == NULL) return;

	m_pTerrain_->UpdateGrassOnTerrain();
}

void CRollupTerrainGrass::OnTvnSelchangedTreeGrassBrush(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hItem = m_ctrlGrassBrushTree_.GetSelectedItem();
	HTREEITEM hParent = m_ctrlGrassBrushTree_.GetNextItem(hItem, TVGN_PARENT);

	//init button
	GetDlgItem(IDC_BUTTON_P_GRASS_ADD)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_GRASS_DELETE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_GRASS_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_GRASS_SIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GRASS_NUM)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_GRASS_NUM)->EnableWindow(FALSE);

	if( hParent != NULL ) //리프라면, (자식이라면 이지만, 깊이가 2단계 밖에 없다.)
	{
		m_beforeTreeitemHandle_ = m_currentTreeitemHandle_;
		m_currentTreeitemHandle_ = hParent;

		GetDlgItem(IDC_EDIT_GRASS_SIZE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_GRASS_SIZE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_GRASS_NUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_SPIN_GRASS_NUM)->EnableWindow(TRUE);

		SPieceGrassInfo* pPieceGrassInfo = (SPieceGrassInfo*)m_ctrlGrassBrushTree_.GetItemData(hItem);
		ASSERT(pPieceGrassInfo);
		m_nEditPieceSize_ = pPieceGrassInfo->m_nSize;
		m_nEditPieceNum_ = pPieceGrassInfo->m_nNumber;
		UpdateData(FALSE);

		//조각풀 프리뷰
		rs3::RDeviceD3D* pRDevice = m_pControlMediator_->GetCurrentDevice();
		if(pRDevice == NULL) return;
		/*LPDIRECT3DDEVICE9 pD3DDevice = pRDevice->GetD3DDevice();
		if(pD3DDevice == NULL) return;*/

		char szfull[256];
		_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
		std::string pieceGrassPath(szfull);
		pieceGrassPath = pieceGrassPath + "Texture\\SpeedGrass\\" + pPieceGrassInfo->m_strPieceGrassName;

		LPDIRECT3DTEXTURE9 pTexture = NULL;
		D3DXIMAGE_INFO texInfo;
		/*HRESULT hr = D3DXCreateTextureFromFileEx( pD3DDevice, pieceGrassPath.c_str() , D3DX_DEFAULT, D3DX_DEFAULT,
			1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0 , &texInfo, NULL,&pTexture);*/
		HRESULT hr = pRDevice->D3DXCreateTextureFromFileEx(pieceGrassPath.c_str() , D3DX_DEFAULT, D3DX_DEFAULT,
			1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0 , texInfo, NULL, &pTexture);

		if(SUCCEEDED(hr))
		{
			CWnd* pWnd = GetDlgItem(IDC_PREVIEW_PIECE_GRASS);
			ASSERT(pWnd);
			CMFCHelper::FillWnd(pWnd, 0, 0, 0);

			CDC *pDC = GetDlgItem(IDC_PREVIEW_PIECE_GRASS)->GetDC();
			ASSERT(pDC);

			HDC hDC = pDC->GetSafeHdc();
			ASSERT(hDC);

			RECT rt;
			pWnd->GetClientRect(&rt);

			UINT uBaseLength = max(texInfo.Width, texInfo.Height);
			float w = (float)texInfo.Width / (float)uBaseLength;
			float h = (float)texInfo.Height / (float)uBaseLength;

			int nW = (rt.right - rt.left)*(1.0f-w);
			int nH = (rt.bottom - rt.top)*(1.0f-h);

			rt.top += nH/2;
			rt.bottom -= nH/2;
			rt.left += nW/2;
			rt.right -= nW/2;

			CMFCHelper::D3DTextureToHDC(hDC, pTexture,&rt);
			pWnd->ReleaseDC(pDC);
			pTexture->Release();
		}
	}
	else //선택한것이 부모라면
	{
		m_beforeTreeitemHandle_ = m_currentTreeitemHandle_;
		m_currentTreeitemHandle_ = hItem;

		// 프리뷰 무효화
		CWnd *pWnd = GetDlgItem(IDC_PREVIEW_PIECE_GRASS);
		CClientDC dc(pWnd);
		CRect rect;
		pWnd->GetClientRect(&rect);
		CBrush brush, *pOldBrush;

		brush.CreateSysColorBrush(COLOR_BTNFACE);
		pOldBrush = (CBrush*)dc.SelectObject(&brush);
		dc.Rectangle(&rect);
		dc.SelectObject(pOldBrush);

		HTREEITEM hChild = m_ctrlGrassBrushTree_.GetNextItem(hItem, TVGN_CHILD);
		if(hChild != NULL)
		{
			GetDlgItem(IDC_EDIT_GRASS_SIZE)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_GRASS_NUM)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_GRASS_SIZE)->EnableWindow(TRUE);
			GetDlgItem(IDC_SPIN_GRASS_NUM)->EnableWindow(TRUE);

			SPieceGrassInfo* pPieceGrassInfo = (SPieceGrassInfo*)m_ctrlGrassBrushTree_.GetItemData(hChild);
			ASSERT(pPieceGrassInfo);
			m_nEditPieceSize_ = pPieceGrassInfo->m_nSize;
			m_nEditPieceNum_ = pPieceGrassInfo->m_nNumber;
			UpdateData(FALSE);
		}
	}

	*pResult = 0;
}

//전제 : 매개변수 컨테이너는 empty 일것.
void CRollupTerrainGrass::GetCurrentPieceGrassVector(std::vector<SPieceGrassInfo*>& _refPieceGrassVec)
{
	if(m_currentTreeitemHandle_ == NULL)
		return;

	HTREEITEM hChild = m_ctrlGrassBrushTree_.GetNextItem(m_currentTreeitemHandle_, TVGN_CHILD);
	while (hChild != NULL)
	{
		SPieceGrassInfo* pPieceGrassInfo = (SPieceGrassInfo*)m_ctrlGrassBrushTree_.GetItemData(hChild);
		_refPieceGrassVec.push_back(pPieceGrassInfo);
		hChild = m_ctrlGrassBrushTree_.GetNextSiblingItem(hChild);
	}
}

void CRollupTerrainGrass::OnBnClickedButtonGrassAdd()
{
	CXMLEditInfoFile::GetReference().AddGrassBrush();
	OnBnClickedButtonPGrassAdd();
}

void CRollupTerrainGrass::OnBnClickedButtonPGrassAdd()
{
	if(m_pTerrain_ == NULL || m_pGrassSystem_ == NULL) return;

	//프리뷰를 위한 장치
	rs3::RDeviceD3D* pRDevice = m_pControlMediator_->GetCurrentDevice();
	if(pRDevice == NULL) return;
	//LPDIRECT3DDEVICE9 pD3DDevice = pRDevice->GetD3DDevice();
	//if(pD3DDevice == NULL) return;

	std::string zonePath(m_pControlMediator_->m_mapInfo.m_strScenePath + m_pControlMediator_->m_mapInfo.m_strSceneFile);
	CDlgGrassListOpen dlg(CDlgGrassListOpen::GRL_SELECT_OPEN, zonePath.c_str(), m_pGrassSystem_, pRDevice);
	if(dlg.DoModal() == IDOK)
	{
		//비어있을 경우
		if(dlg.m_vecGrassPieceName.empty()) return;

		for(STRING_CONTAINER::iterator itr = dlg.m_vecGrassPieceName.begin();
			itr != dlg.m_vecGrassPieceName.end(); ++itr)
		{
			const char* pPieceGrassName = itr->c_str();
			if(pPieceGrassName == NULL) return;

			//더할 조각풀의 중복 검사
			HTREEITEM hChild = m_ctrlGrassBrushTree_.GetNextItem(m_currentTreeitemHandle_, TVGN_CHILD);
			while (hChild != NULL)
			{
				SPieceGrassInfo* pPieceGrass = (SPieceGrassInfo*)m_ctrlGrassBrushTree_.GetItemData(hChild);
				if(pPieceGrass->m_strPieceGrassName == pPieceGrassName)
				{
					AfxMessageBox("이미 같은 종류의 풀이 있습니다.");
					return;
				}
				hChild = m_ctrlGrassBrushTree_.GetNextSiblingItem(hChild);
			}

			//add to TreeControl
			HTREEITEM hItem = m_ctrlGrassBrushTree_.InsertItem( pPieceGrassName, m_currentTreeitemHandle_, TVI_LAST);
			//add PicecGrassInfo
			SPieceGrassInfo* pPieceGrassInfo = new SPieceGrassInfo;
			pPieceGrassInfo->m_strPieceGrassName = pPieceGrassName;
			m_ctrlGrassBrushTree_.SetItemData(hItem, (DWORD_PTR)pPieceGrassInfo);
		}

		//set change flag and TreeControl Set
		CXMLEditInfoFile::GetReference().ChangeGrassInfo();
		m_ctrlGrassBrushTree_.Expand(m_currentTreeitemHandle_, TVE_EXPAND);
	}
}

void CRollupTerrainGrass::OnBnClickedButtonGrassDelete()
{
	HTREEITEM hItem = m_ctrlGrassBrushTree_.GetSelectedItem();

	//자식이 있고 없고에 따른 PicecGrass / Grass Brush 판별
	HTREEITEM hChild = m_ctrlGrassBrushTree_.GetNextItem(hItem, TVGN_CHILD);
	if(hChild != NULL) //자식이 있다면,
	{

		while (hChild != NULL)
		{
			SPieceGrassInfo* pPieceGrassInfo = (SPieceGrassInfo*)m_ctrlGrassBrushTree_.GetItemData(hChild);
			SAFE_DELETE(pPieceGrassInfo);
			hChild = m_ctrlGrassBrushTree_.GetNextSiblingItem(hChild);
		}

		m_ctrlGrassBrushTree_.DeleteItem(m_currentTreeitemHandle_);
		CXMLEditInfoFile::GetReference().ChangeGrassInfo();

		if(m_ctrlGrassBrushTree_.GetCount() == 0)
		{
			m_currentTreeitemHandle_ = NULL;
			GetDlgItem(IDC_BUTTON_GRASS_DELETE)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_P_GRASS_ADD)->EnableWindow(FALSE);
		}
	}
	else
	{
		SPieceGrassInfo* pPieceGrassInfo = (SPieceGrassInfo*)m_ctrlGrassBrushTree_.GetItemData(hItem);
		SAFE_DELETE(pPieceGrassInfo);
		m_ctrlGrassBrushTree_.DeleteItem(hItem);
		CXMLEditInfoFile::GetReference().ChangeGrassInfo();
	}
}

void CRollupTerrainGrass::OnEnChangeEditNumberSize()
{
	UpdateData(TRUE);

	if(m_nEditPieceNum_ < 1) m_nEditPieceNum_ = 1;
	if(m_nEditPieceSize_ < 1) m_nEditPieceSize_ = 1;

	HTREEITEM hItem = m_ctrlGrassBrushTree_.GetSelectedItem();
	HTREEITEM hChild = m_ctrlGrassBrushTree_.GetNextItem(hItem, TVGN_CHILD);

	if(hChild == NULL) //리프일 경우
	{
		SPieceGrassInfo* pPieceGrassInfo = (SPieceGrassInfo*)m_ctrlGrassBrushTree_.GetItemData(hItem);
		ASSERT(pPieceGrassInfo);
		pPieceGrassInfo->m_nNumber = m_nEditPieceNum_;
		pPieceGrassInfo->m_nSize = m_nEditPieceSize_;
	}
	else //부모일 경우
	{
		//모든 자식들의 값 변화
		while (hChild != NULL)
		{
			SPieceGrassInfo* pPieceGrass = (SPieceGrassInfo*)m_ctrlGrassBrushTree_.GetItemData(hChild);
			pPieceGrass->m_nNumber = m_nEditPieceNum_;
			pPieceGrass->m_nSize = m_nEditPieceSize_;

			hChild = m_ctrlGrassBrushTree_.GetNextSiblingItem(hChild);
		}
	}

	UpdateData(FALSE);
}

void CRollupTerrainGrass::VerifyTreeInfoAndGrassListInfo_(CTreeCtrl& _refTreeCtrl, STRING_CONTAINER& _refVecStr)
{
	//HTREEITEM hParent = _refTreeCtrl.GetNextItem(NULL, TVGN_FIRSTVISIBLE);
	HTREEITEM hParent = _refTreeCtrl.GetRootItem();
	while (hParent != NULL)
	{
		HTREEITEM hChild = _refTreeCtrl.GetNextItem(hParent, TVGN_CHILD);
		while (hChild != NULL)
		{
			SPieceGrassInfo* pPieceGrass = (SPieceGrassInfo*)_refTreeCtrl.GetItemData(hChild);

			//containing test
			bool bIsContainded = false;
			for(STRING_CONTAINER::iterator itr = _refVecStr.begin(); itr != _refVecStr.end(); ++itr)
			{
				if(pPieceGrass->m_strPieceGrassName == (*itr))
				{
					bIsContainded = true;
					break;
				}

			}

			HTREEITEM toDeleteItem = hChild;
			hChild = _refTreeCtrl.GetNextSiblingItem(hChild);

			if(bIsContainded == false)
			{
				SAFE_DELETE(pPieceGrass);
				_refTreeCtrl.DeleteItem(toDeleteItem);
			}
		}

		//expand tree
		_refTreeCtrl.Expand(hParent, TVE_EXPAND);
		hParent = _refTreeCtrl.GetNextSiblingItem(hParent);
	}
}

void CRollupTerrainGrass::OnBnClickedButtonGrassSync()
{
	if(m_pTerrain_ == NULL || m_pGrassSystem_ == NULL) return;

	STRING_CONTAINER vecStrGrassList;
	CString grlPath( (m_pControlMediator_->m_mapInfo.m_strScenePath + m_pControlMediator_->m_mapInfo.m_strSceneFile).c_str() );
	grlPath.Replace(".zone.xml", "_grass.grl");

	char szfull[256];
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	std::string grassResPath(szfull);
	grassResPath += "Texture\\SpeedGrass\\";

	if (CXMLResourceList::GetReference().LoadGrassList(grlPath.GetString(), vecStrGrassList))
	{
		int left_index = grlPath.Find(".grl");
		CString cstrParam("-width 2048 -o ");
		cstrParam = cstrParam + "\"" + grlPath.Left(left_index) + "\"";
		for(STRING_CONTAINER::iterator it = vecStrGrassList.begin(); it != vecStrGrassList.end(); ++it)
		{
			std::string fullPath(grassResPath + *it);
			if(GetFileAttributes(fullPath.c_str()) == INVALID_FILE_ATTRIBUTES)
			{
				fullPath = fullPath + "파일의 자원이 동기화 되어있지 않습니다. 동기화후 다시 시도하십시오.";
				AfxMessageBox(fullPath.c_str());
				return;
			}
			cstrParam = cstrParam + (" \"" + fullPath + "\"").c_str();
		}

		if ( CMFCHelper::Exec(_T("AtlasCreationTool.exe"), cstrParam, FALSE, INFINITE))
		{
			CString cstrDDSFilename = grlPath.Left(left_index) + ".dds";

			BOOL bConvertResult = CMFCHelper::ConvertImageToDXT(cstrDDSFilename);
			_ASSERT(bConvertResult);
		}

		//xml load test는 통과한 상태
		MXml kXml;
		kXml.LoadFile(grlPath.GetString());
		MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("ROOT");
		m_pGrassSystem_->LoadGrassListFromXML(*pRootElement, NULL, NULL);
	}
}

void CRollupTerrainGrass::OnBnClickedCheckGrassLod()
{
	UpdateData(TRUE);
	if(m_nCheck_)
		rs3::RSpeedGrass::SetLODParams(XCONST::GRASS_LOD_FAR, XCONST::GRASS_LOD_FAR - XCONST::GRASS_LOD_NEAR);
	else
		rs3::RSpeedGrass::SetLODParams(100000.0f, 0);
}

void CRollupTerrainGrass::ViewToggleGrass()
{
	m_nCheck_ = !m_nCheck_;
	UpdateData(FALSE);
	OnBnClickedCheckGrassLod();
}
