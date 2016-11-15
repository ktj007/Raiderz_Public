// Page2.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EffectTool.h"
#include "RParticle.h"
#include "PageMaterial.h"
#include "RDeviceD3D.h"
#include "RConfiguration.h"

using namespace Gdiplus;

extern CRView*		g_rView;


// CPage2 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPage2, CPPage)
CPage2::CPage2()
	: CPPage(CPage2::IDD)
	, m_strPathname(_T(""))
	, m_strDumy(_T(""))
	, m_bRadioOne(false)
	, m_bRadioTwo(false)
	, m_bRadioThree(false)
	, m_fTexU(1)
	, m_fTexV(1)
	, m_fTexX(0)
	, m_fTexY(0)
	, m_fAlphaRef(128)
{
	m_eSrcBlend = RBLEND_ONE;
	m_eDstBlend = RBLEND_ONE;

	TCHAR aStr[MAX_PATH];
	memset(aStr, 0, sizeof(aStr));
	GetCurrentDirectory(MAX_PATH, aStr);

	m_strCurrentPath = aStr;
	m_strCurrentPath.Replace(_T('\\'),_T('/'));
	if ( !ConfigFromXML())
	{
// 		m_strPathname = m_strCurrentPath + "/image";
// 		m_strTextureName = _T("");
	}
}

CPage2::~CPage2()
{
}

void CPage2::DoDataExchange(CDataExchange* pDX)
{
	CPPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FOLDERNAME, m_strPathname);
	DDX_Control(pDX, IDC_COMBO_DUMY, m_ctlDumy);
	DDX_CBString(pDX, IDC_COMBO_DUMY, m_strDumy);
	DDX_Control(pDX, IDC_LIST_FILES, m_ctlFiles);
	DDX_Control(pDX, IDC_COMBO_SRCBLEND, m_ctlSrcBlend);
	DDX_Control(pDX, IDC_COMBO_DESTBLEND, m_ctlDstBlend);
	DDX_Text(pDX, IDC_EDIT_TEXU, m_fTexU);
	DDX_Text(pDX, IDC_EDIT_TEXV, m_fTexV);
	DDX_Text(pDX, IDC_EDIT_TEXX, m_fTexX);
	DDX_Text(pDX, IDC_EDIT_TEXY, m_fTexY);
	DDX_Text(pDX, IDC_EDIT_ALPHAREF, m_fAlphaRef);
}


BEGIN_MESSAGE_MAP(CPage2, CPPage)
	ON_CBN_SELCHANGE(IDC_COMBO_DUMY, OnCbnSelchangeComboDumy)
	ON_LBN_SELCHANGE(IDC_LIST_FILES, OnLbnSelchangeListFiles)
	ON_EN_CHANGE(IDC_EDIT_TEXU, OnEnChangeEditTex)
	ON_EN_CHANGE(IDC_EDIT_TEXV, OnEnChangeEditTex)
	ON_EN_CHANGE(IDC_EDIT_TEXX, OnEnChangeEditTex)
	ON_EN_CHANGE(IDC_EDIT_TEXY, OnEnChangeEditTex)
	ON_BN_CLICKED(IDC_RADIO_ONE, OnBnClickedRadioOne)
	ON_BN_CLICKED(IDC_RADIO_TWO, OnBnClickedRadioTwo)
	ON_BN_CLICKED(IDC_RADIO_THREE, OnBnClickedRadioThree)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_COMBO_SRCBLEND, OnCbnSelchangeComboSrcblend)
	ON_CBN_SELCHANGE(IDC_COMBO_DESTBLEND, OnCbnSelchangeComboDestblend)
	ON_BN_CLICKED(IDC_BUTTON_FOLDER, OnBnClickedButtonFolder)
	ON_EN_CHANGE(IDC_EDIT_ALPHAREF, &CPage2::OnEnChangeEditAlpharef)
END_MESSAGE_MAP()


// CPage2 메시지 처리기입니다.

bool CPage2::ConfigFromXML()
{
	MXml kXml;
	if ( !kXml.LoadFile(FILENAME_CONFIG))
		return false;

	MXmlElement *pElement = kXml.Doc()->FirstChildElement("texturepath");
	if ( !pElement )
		return false;

	TCHAR tStr[] = "..\\..\\Data\\Texture\\SFX";
	//_Contents(tStr, pElement,"",&kXml);

	if ( (tStr[0] >= _T('A') && tStr[0] <= _T('Z'))
		|| (tStr[0] >= _T('a') && tStr[0] <= _T('z')) )	// is alphabet ?
	{
		m_strPathname = tStr;
	}
	else
	{
		//m_strPathname = m_strCurrentPath;
		//if (tStr[0] != _T('/') && tStr[0] != _T('\\'))
		//	m_strPathname += _T('/');

		m_strPathname += tStr;
	}

	m_strPathname.Replace(_T('\\'),_T('/'));

	config.SetTexturePath(pElement->GetText());

	return true;
}

void CPage2::SetSrcBlendCtrl(RBLEND eBlend)
{
	m_eSrcBlend = eBlend;

	for ( int i = 0; tableBlendTypes[i].pcName != NULL; ++i )
	{
		if ( tableBlendTypes[i].eBlend == eBlend )
		{
			m_ctlSrcBlend.SetCurSel(i);
			break;
		}
	}
}

void CPage2::SetDstBlendCtrl(RBLEND eBlend)
{
	m_eDstBlend = eBlend;

	for ( int i = 0; tableBlendTypes[i].pcName != NULL; ++i )
	{
		if ( tableBlendTypes[i].eBlend == eBlend )
		{
			m_ctlDstBlend.SetCurSel(i);
			break;
		}
	}
}

void CPage2::InitUIState()
{
	m_ctlSrcBlend.Clear();
	m_ctlDstBlend.Clear();

	for ( int i = 0; tableBlendTypes[i].pcName ; ++i )
	{
		m_ctlSrcBlend.InsertString(i, tableBlendTypes[i].pcName);
		m_ctlDstBlend.InsertString(i, tableBlendTypes[i].pcName);
	}

	SetSrcBlendCtrl( m_eSrcBlend );
	SetDstBlendCtrl( m_eDstBlend );
}

void CPage2::OnBnClickedButtonFolder()
{
	UpdateData(TRUE);

	m_ctlFiles.ResetContent();

	HANDLE hFind;
	WIN32_FIND_DATA fd;

	BOOL bRet  = TRUE;
	int  index = 0;

	CString str = m_strPathname + _T("/*");

	CHAR aStr[MAX_PATH];
	memset(aStr, 0, MAX_PATH);

	hFind = FindFirstFile( str.GetString(),&fd);
	while ( hFind != INVALID_HANDLE_VALUE && bRet )
	{
		if (( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
		{
			// 모든 문자를 소문자로 변환
			strcpy(aStr, fd.cFileName);
			_tcslwr(aStr);

			if ( _tcsstr(aStr,_T(".bmp")) 
				|| _tcsstr(aStr,_T(".tga"))
				|| _tcsstr(aStr,_T(".jpg"))
				|| _tcsstr(aStr,_T(".png"))
				|| _tcsstr(aStr,_T(".dds"))
				)
			{
				m_ctlFiles.AddString( fd.cFileName);
			}
		}
		bRet = FindNextFile( hFind, &fd );
	}

	FindClose( hFind );
}

BOOL CPage2::OnInitDialog()
{
	CPPage::OnInitDialog();

	InitUIState();

	OnBnClickedButtonFolder();

	//REmitter::SetTexturePath(m_strPathname);
	//config.SetTexturePath(m_strPathname);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPage2::OnCbnSelchangeComboDumy()
{
	int iSel = m_ctlDumy.GetCurSel();
	if ( iSel < 0 )
		return;

	m_ctlDumy.GetLBText(iSel, m_strDumy);

	SendMessageDumy();
}

void CPage2::DrawTextureToPicbox()
{
	CWnd *pWnd = GetDlgItem(IDC_PICTURE_TEXTURE);
	CDC *pDC = pWnd->GetDC();
	HDC hDestDC = pDC->GetSafeHdc();

	CString strFullname;
	strFullname = m_strPathname + _T("\\") + m_strTextureName;

	RDeviceD3D* pDeviceD3D = (RDeviceD3D*)g_rView->GetDevice();
	LPDIRECT3DDEVICE9 pDevice = pDeviceD3D->GetD3DDevice();

	assert(pDevice != NULL);

	LPDIRECT3DTEXTURE9 pTexture = NULL;
	HRESULT hr;
	hr = D3DXCreateTextureFromFile(pDevice, strFullname.GetString(),&pTexture);

	// D3DXSaveTextureToFileInMemory를 이용하여, Memory상에 BMP로 저장하고 그것을 이용한다.
	LPD3DXBUFFER pBuffer = NULL;
	hr = D3DXSaveTextureToFileInMemory(&pBuffer, D3DXIFF_BMP, pTexture, NULL);

	if(pTexture == NULL)
	{
		mlog("pTexture == NULL, filename = %s \n", m_strTextureName);
		return;
	}

	// 헤더를 읽고
	BITMAPFILEHEADER *pHeader = (BITMAPFILEHEADER*)pBuffer->GetBufferPointer();
	BITMAPINFOHEADER *pInfo = (BITMAPINFOHEADER*)((BYTE*)pBuffer->GetBufferPointer() + sizeof(BITMAPFILEHEADER));
	BITMAPINFO bmpInfo;
	memset(&bmpInfo, 0, sizeof(bmpInfo));
	bmpInfo.bmiHeader = *pInfo;
	void *pData = (void*)((BYTE*)pBuffer->GetBufferPointer() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

	// 동일한 DIBSECTION을 만든다.
	HDC hMemDC = CreateCompatibleDC(hDestDC);
	BYTE *pPixel = NULL;

	UINT dibFormat = DIB_RGB_COLORS;
	HBITMAP hMemDCBitmap = CreateDIBSection(hMemDC,&bmpInfo, dibFormat, (void**)&pPixel, NULL, 0);
	SelectObject(hMemDC, hMemDCBitmap);

	// BITMAP의 내용을 복사
	memcpy(pPixel  , pData, pInfo->biWidth*pInfo->biHeight*pInfo->biBitCount/8);

	RECT rect;
	pWnd->GetWindowRect(&rect);
	int iWidth = rect.right - rect.left;
	int iHeight = rect.bottom - rect.top;

	// GDI 화면에 BitBlt로 복사하여 그리기
	// 늘려서 그리도록 고쳤는데 이상하면 그리는 부분 크기를 조절해서 100%로 찍도록 하기 [7/20/2006 madduck]
	// 그리드 그리는 부분에서 좀 에러가 생길수도....
	//BitBlt(hDestDC, 0, 0, pInfo->biWidth, pInfo->biHeight, hMemDC, 0, 0, SRCCOPY);
	//BitBlt(hDestDC, 0, 0, iWidth, iHeight, hMemDC, 0, 0, SRCCOPY);
	StretchBlt(hDestDC, 0, 0, iWidth, iHeight, hMemDC, 0, 0, pInfo->biWidth, pInfo->biHeight,SRCCOPY);

	//TODO : 진호씨 컴에서만 텍스쳐가 날아가버리네.ㅡ,ㅡ
	{
		RECT Crect;
		pWnd->GetClientRect(&Crect);

		CBrush brush(RGB(255, 0, 0));
		//hbrush = brush;
		//CBrush* pOldBrush = pDC->SelectObject(&brush);

		CRect crect;
		crect.SetRect(Crect.right*m_fTexX, Crect.bottom*m_fTexY, Crect.right*m_fTexU, Crect.bottom*m_fTexV);
		pDC->FrameRect(crect, &brush);
		//pDC->SelectObject(pOldBrush);
	}

	// DIBSection 제거
	pBuffer->Release();
	pTexture->Release();
	DeleteDC(hMemDC);
	DeleteObject(hMemDCBitmap);

	pWnd->ReleaseDC(pDC);
}

void CPage2::OnLbnSelchangeListFiles()
{
	int iSel = m_ctlFiles.GetCurSel();
	if ( iSel < 0 )
		return;

	CString str, strFullname;
	m_ctlFiles.GetText(iSel,m_strTextureName);
	strFullname = m_strPathname + _T("\\") + m_strTextureName;

	CWnd *pWnd = GetDlgItem(IDC_PICTURE_TEXTURE);
	Graphics g(pWnd->GetSafeHwnd());
	g.Clear(Color(255, 255, 255, 255));

	SendMessageTexture();
}

void CPage2::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DrawTextureToPicbox();

	// 그리기 메시지에 대해서는 CPPage::OnPaint()을(를) 호출하지 마십시오.
}

void CPage2::OnEnChangeEditTex()
{
	UpdateData(TRUE);

	if( m_fTexU > 1.0f ) m_fTexU = 1.0f;
	if( m_fTexV > 1.0f ) m_fTexV = 1.0f;

	SendMessageTexture();
}

void CPage2::OnBnClickedRadioOne()
{
	m_bRadioOne = true;
	m_bRadioTwo = false;
	m_bRadioThree = false;

	m_eSrcBlend = tableBlendTypes[1].eBlend;
	m_eDstBlend = tableBlendTypes[1].eBlend;

	SendMessageBlend();
}

void CPage2::OnBnClickedRadioTwo()
{
	m_bRadioOne = false;
	m_bRadioTwo = true;
	m_bRadioThree = false;

	m_eSrcBlend = tableBlendTypes[4].eBlend;
	m_eDstBlend = tableBlendTypes[5].eBlend;

	SendMessageBlend();
}

//alphatest
void CPage2::OnBnClickedRadioThree()
{
	m_bRadioOne = false;
	m_bRadioTwo = false;
	m_bRadioThree = true;

	m_eSrcBlend = tableBlendTypes[9].eBlend;
	m_eDstBlend = tableBlendTypes[9].eBlend;

	SendMessageBlend();
}

void CPage2::CheckRadioButton()
{
	m_bRadioOne = false;
	m_bRadioTwo = false;
	m_bRadioThree = false;

	if( m_eSrcBlend == tableBlendTypes[4].eBlend &&
		m_eDstBlend == tableBlendTypes[1].eBlend )
	{
		m_bRadioOne = true;

		// 라디오버튼 체크를 어케하지 ㅡ,ㅡ [8/11/2006 madduck]
// 		CWnd *pWnd = GetDlgItem(IDC_RADIO_ONE);
// 		pWnd->CheckRadioButton(IDC_RADIO_ONE, IDC_RADIO_THREE, IDC_RADIO_ONE);
	}

	if( m_eSrcBlend == tableBlendTypes[1].eBlend &&
		m_eDstBlend == tableBlendTypes[1].eBlend )
	{
		m_bRadioTwo = true;
	}

}

void CPage2::OnCbnSelchangeComboSrcblend()
{
	int iSel = m_ctlSrcBlend.GetCurSel();
	if ( iSel < 0 )		return;

	m_eSrcBlend = tableBlendTypes[iSel].eBlend;

	SendMessageBlend();
}

void CPage2::OnCbnSelchangeComboDestblend()
{
	int iSel = m_ctlDstBlend.GetCurSel();
	if ( iSel < 0 )		return;

	m_eDstBlend = tableBlendTypes[iSel].eBlend;

	SendMessageBlend();
}

void CPage2::OnEnChangeEditAlpharef()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPPage::OnInitDialog() 함수를 재지정하고  마스크에 OR 연산하여 설정된
	// ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출해야만
	// 해당 알림 메시지를 보냅니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	SendMessageBlend();
}
