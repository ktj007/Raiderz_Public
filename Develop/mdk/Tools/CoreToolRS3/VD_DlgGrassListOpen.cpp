// VD_DlgGrassListOpen.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VD_DlgGrassListOpen.h"

#include "V_MFCHelper.h"
#include "d3dx9tex.h"
#include "RGrassSystem.h"
#include "RSpeedGrass.h"

#include "VD_DlgCommonFileOpen.h"
#include "C_XMLConfig.h"
#include "C_XMLResourceList.h"

// CDlgGrassListOpen 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgGrassListOpen, CDialog)

CDlgGrassListOpen::CDlgGrassListOpen(OPEN_TYPE _type, const char* _pszZonePath, rs3::RGrassSystem* _pGrassSystem, rs3::RDeviceD3D* _pDevice)
	: CDialog(CDlgGrassListOpen::IDD, NULL), m_cstrGrlPath_(_pszZonePath)
{
	m_eOpenType_ = _type;
	m_pDevice_ = _pDevice;
	m_pGrassSystem_ = _pGrassSystem;
	m_cstrGrlPath_.Replace(".zone.xml", "_grass.grl");
	m_bChanged = false;
}

CDlgGrassListOpen::~CDlgGrassListOpen()
{
}

//사전조건 : 변경된 m_vecGrassList_ 를 사용한다.
void CDlgGrassListOpen::UpdateListBox_()
{
	m_ctlGrassList_.ResetContent();
	int vec_size = m_vecGrassList_.size();
	for(int i = 0; i < vec_size; ++i)
		m_ctlGrassList_.InsertString(i, m_vecGrassList_[i].c_str());
}

//사전조건 : 변경된 m_vecGrassList_ 를 사용한다.
void CDlgGrassListOpen::EditAndMakeGrl_(const char* _szFrom, const char* _szTo)
{
	if(m_pGrassSystem_ == NULL)
		return;

	if(m_vecGrassList_.empty())
	{
		if(GetFileAttributes(m_cstrGrlPath_) != INVALID_FILE_ATTRIBUTES)
			DeleteFile(m_cstrGrlPath_);

		MXml grlXml;
		MXmlDeclaration* pGrlDecl = grlXml.CreateDeclaration("1.0", "UTF-8", "");
		grlXml.Doc()->LinkEndChild(pGrlDecl);

		std::string filename(m_cstrGrlPath_.GetString());
		std::string strExt(".grl");
		int ext_start = filename.find(strExt);
		filename.replace(ext_start, strExt.length(), ".dds");
		int r_token_slash = filename.rfind("\\");
		filename = filename.substr(r_token_slash + 1, filename.length());
		MXmlElement* pElementRoot = new MXmlElement( "ROOT" );
		grlXml.Doc()->LinkEndChild(pElementRoot);
		_SetAttribute(pElementRoot, "filename", filename.c_str());
		grlXml.SaveFile(m_cstrGrlPath_.GetString());

		m_pGrassSystem_->LoadGrassListFromXML(*pElementRoot, NULL, NULL);
	}
	else
	{
		int left_index = m_cstrGrlPath_.Find(".grl");

		CString cstrParam("-width 2048 -o ");
		cstrParam = cstrParam + "\"" + m_cstrGrlPath_.Left(left_index) + "\"";
		for(GRASS_LIST::iterator it = m_vecGrassList_.begin(); it != m_vecGrassList_.end(); ++it)
		{
			std::string fullPath(m_strGrassResPath_ + *it);
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
			CString cstrDDSFilename = m_cstrGrlPath_.Left(left_index) + ".dds";
			
			BOOL bConvertResult = CMFCHelper::ConvertImageToDXT(cstrDDSFilename);
			_ASSERT(bConvertResult);
		}

		MXml kXml;
		kXml.AfterCreateFileRead( m_cstrGrlPath_ );		
		if ( !kXml.LoadFile(m_cstrGrlPath_))
			return;
	   
		MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("ROOT");

		if(pRootElement == NULL) return;
	
		if(_szFrom != NULL)
			m_pGrassSystem_->LoadGrassListFromXML(*pRootElement, _szFrom, _szTo);
		else
			m_pGrassSystem_->LoadGrassListFromXML(*pRootElement, NULL, NULL);
	}

	//set change flag to true
	m_bChanged = true;

	//vector copy
	m_vecGrassPieceName.clear();
	m_vecGrassPieceName.assign(m_vecGrassList_.begin(), m_vecGrassList_.end());

	UpdateListBox_();
}

BOOL CDlgGrassListOpen::OnInitDialog()
{
	CDialog::OnInitDialog();
	CXMLResourceList::GetReference().LoadGrassList(m_cstrGrlPath_, m_vecGrassList_);
	/*if(CXMLResourceList::GetReference().LoadGrassList(m_cstrGrlPath_, m_vecGrassList_) == false)
		AfxMessageBox("grl 파일이 존재하지 않거나 잘못된 파일입니다.");*/

	m_strGrassResPath_.clear();
	char szfull[256];
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	m_strGrassResPath_ = szfull;
	m_strGrassResPath_ += "Texture\\SpeedGrass\\";

	UpdateListBox_();
	if(m_eOpenType_ == GRL_MAKE_OPEN)
	{
		//사용할 버튼 활성화
		GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDCANCEL)->SetWindowText("확인");
	}
	else
	{
		GetDlgItem(ID_GRL_ADD)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_GRL_DELETE)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_GRL_CHANGE)->ShowWindow(SW_HIDE);
	}
	return TRUE;
}

void CDlgGrassListOpen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GRL_FILE, m_ctlGrassList_);
}


BEGIN_MESSAGE_MAP(CDlgGrassListOpen, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_GRL_FILE, &CDlgGrassListOpen::OnLbnSelchangeListGrlFile)
	ON_BN_CLICKED(ID_GRL_ADD, &CDlgGrassListOpen::OnBnClickedGrlAdd)
	ON_BN_CLICKED(ID_GRL_DELETE, &CDlgGrassListOpen::OnBnClickedGrlDelete)
	ON_BN_CLICKED(ID_GRL_CHANGE, &CDlgGrassListOpen::OnBnClickedGrlChange)
	ON_LBN_DBLCLK(IDC_LIST_GRL_FILE, &CDlgGrassListOpen::OnLbnDblclkListGrlFile)
END_MESSAGE_MAP()


// CDlgGrassListOpen 메시지 처리기입니다.


void CDlgGrassListOpen::OnLbnSelchangeListGrlFile()
{
	int sel = m_ctlGrassList_.GetCurSel();
	if ( sel < 0 )
		return;

	if(m_eOpenType_ == GRL_SELECT_OPEN)
	{
		m_vecGrassPieceName.clear();

		int nCount = m_ctlGrassList_.GetSelCount();
		CArray<int,int> aryListBoxSel;
		aryListBoxSel.SetSize(nCount);
		m_ctlGrassList_.GetSelItems(nCount, aryListBoxSel.GetData());

		for ( int i = 0; i < nCount; ++i )
			m_vecGrassPieceName.push_back(m_vecGrassList_[ aryListBoxSel[i] ]);
	}

	//to preview
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	D3DXIMAGE_INFO texInfo;

	/*HRESULT hr = D3DXCreateTextureFromFileEx( m_pDevice_, (m_strGrassResPath_ + m_vecGrassList_[sel]).c_str()
		, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0
		, &texInfo, NULL,&pTexture);*/
	HRESULT hr = m_pDevice_->D3DXCreateTextureFromFileEx( (m_strGrassResPath_ + m_vecGrassList_[sel]).c_str()
		, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0
		, texInfo, NULL, &pTexture);

	if(SUCCEEDED(hr))
	{
		CWnd *pWnd = GetDlgItem(IDC_GRL_PREVIEW);
		CMFCHelper::FillWnd(pWnd, 0, 0, 0);

		CDC *pDC = pWnd->GetDC();
		if ( pDC == NULL )
			return;

		HDC hDC = pDC->GetSafeHdc();
		if ( hDC == NULL )
			return;

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

		CMFCHelper::D3DTextureToHDC(hDC,pTexture,&rt);
		pWnd->ReleaseDC(pDC);
		pTexture->Release();
	}
}

void CDlgGrassListOpen::OnBnClickedGrlAdd()
{
	if( m_vecGrassList_.size() >= rs3::RSpeedGrass::GRASS_MAX_NUM )
	{
		char buff[16];
		itoa( rs3::RSpeedGrass::GRASS_MAX_NUM, buff, 10 );
		CString strMsg = "풀 텍스쳐를 " + CString(buff) + "개 이상 등록 할 수 없습니다.";
		AfxMessageBox(strMsg);
		return;
	}

	CListViewOpenDlg dlg(m_strGrassResPath_.c_str());
	dlg.SetTitle("조각풀의 그림파일을 선택하세요");
	dlg.SetFilter("Piece Grass 파일\0*.bmp;*.tga;*.jpg");
	dlg.AddStyle(OFN_ALLOWMULTISELECT);
	
	CString strFilename;
	if ( dlg.DoModal() == IDOK ) 
	{

		POSITION pos=dlg.GetStartPosition();
		while(pos!=NULL)
		{
			// Get the dir+filename one by one and show it in message box
			CString csFile(dlg.GetNextPathName(pos));

			int right_count = csFile.GetLength() - csFile.ReverseFind('\\') -1;
			csFile = csFile.Right(right_count);
			for(GRASS_LIST::iterator it = m_vecGrassList_.begin(); it != m_vecGrassList_.end(); ++it)
			{
				if( *it == csFile.GetString() )
				{
					strFilename += "은 이미 존재하는 파일입니다.";
					AfxMessageBox(csFile);
					return;
				}
			}
			m_vecGrassList_.push_back(csFile.GetString());
		}
		EditAndMakeGrl_();
	}
}

void CDlgGrassListOpen::OnBnClickedGrlChange()
{
	int selected_index = m_ctlGrassList_.GetCurSel();

	if( selected_index != -1)
	{
		//바꾸는것은 하나만 바꾸게
		m_ctlGrassList_.SetSel(-1, FALSE);
		m_ctlGrassList_.SetSel(selected_index, TRUE);

		CListViewOpenDlg dlg(m_strGrassResPath_.c_str());
		dlg.SetTitle("조각풀의 그림파일을 선택하세요");
		dlg.SetFilter("Piece Grass 파일\0*.bmp;*.tga;*.jpg");
		//CBMPOpenDlg dlg(m_strGrassResPath_.c_str(),"텍스쳐파일\0*.bmp;*.tga;*.jpg");
		
		CString strFilename;
		if ( dlg.DoModal() == IDOK ) 
		{
			strFilename = dlg.GetFileName();
			for(GRASS_LIST::iterator it = m_vecGrassList_.begin(); it != m_vecGrassList_.end(); ++it)
			{
				if( *it == strFilename.GetString() )
				{
					strFilename += "은 이미 존재하는 파일입니다.";
					AfxMessageBox(strFilename);
					return;
				}
			}
			std::string strFrom(m_vecGrassList_[selected_index]);
			std::string strTo(strFilename.GetString());

			m_vecGrassList_[selected_index] = strFilename.GetString();
			EditAndMakeGrl_(strFrom.c_str(), strTo.c_str());
		}
	}
	else
		AfxMessageBox("선택이 올바르게 되어있지 않습니다.");
}

void CDlgGrassListOpen::OnBnClickedGrlDelete()
{
	int nCount = m_ctlGrassList_.GetSelCount();
	if(nCount == 0)
	{
		AfxMessageBox("선택이 올바르게 되어있지 않습니다.");
		return;
	}

	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount);
	m_ctlGrassList_.GetSelItems(nCount, aryListBoxSel.GetData());

	//크악. 비효율의 극치 - grassList의 양이 적다고 가정
	std::vector<std::string> vecGrassList;
	for ( GRASS_LIST::iterator itr = m_vecGrassList_.begin(); itr != m_vecGrassList_.end(); ++itr)
	{
		//삭제 목록
		bool bInContainer = false;
		for(int i = 0; i < nCount; ++i )
		{
			if(itr == m_vecGrassList_.begin() + aryListBoxSel[i])
				bInContainer = true;
		}

		//삭제 목록에 없다면
		if(bInContainer == false)
			vecGrassList.push_back(itr->c_str());
	}

	//m_vecGrassList 재설정
	m_vecGrassList_.clear();
	m_vecGrassList_.assign(vecGrassList.begin(), vecGrassList.end());

	EditAndMakeGrl_();
}

void CDlgGrassListOpen::OnLbnDblclkListGrlFile()
{
	if(m_eOpenType_ == GRL_MAKE_OPEN)
	{
		OnBnClickedGrlChange();
	}
	else if(m_eOpenType_ == GRL_SELECT_OPEN)
	{
		CDialog::OnOK();
	}
}
