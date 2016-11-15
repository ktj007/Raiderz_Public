
// MakeCRCDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MakeCRC.h"
#include "MakeCRCDlg.h"
#include "XCheckSum.h"
#include "XCheckSumEncrypt.h"
//#include "vld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMakeCRCDlg 대화 상자




CMakeCRCDlg::CMakeCRCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeCRCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMakeCRCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMakeCRCDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_START, &CMakeCRCDlg::OnBnClickedStart)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMakeCRCDlg 메시지 처리기

BOOL CMakeCRCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CString strCmd = AfxGetApp()->m_lpCmdLine;
	if ( strCmd.IsEmpty() == FALSE)
	{
		CButton* pStartBtn = (CButton*)GetDlgItem( IDC_START);
		if ( pStartBtn != NULL)		pStartBtn->EnableWindow( FALSE);

		m_strPath = strCmd;
		m_strPath.Replace( "\"", "");

		SetTimer( 0, 2000, NULL);
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMakeCRCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMakeCRCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMakeCRCDlg::OnBnClickedStart()
{
	CButton* pStartBtn = (CButton*)GetDlgItem( IDC_START);
	if ( pStartBtn != NULL)		pStartBtn->EnableWindow( FALSE);


	// Read directories
	char szPath[ MAX_PATH];
	GetCurrentDirectory( sizeof( szPath), szPath);

	Run( szPath);


	if ( pStartBtn != NULL)		pStartBtn->EnableWindow( TRUE);
}


void CMakeCRCDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 0)
	{
		KillTimer( 0);

		Run( m_strPath.GetBuffer());

		SetTimer( 1, 2000, NULL);
	}
	else if ( nIDEvent == 1)
	{
		KillTimer( 1);

		SendMessage( WM_CLOSE, 0, 0);
	}

	CDialog::OnTimer(nIDEvent);
}


void CMakeCRCDlg::Run( const char* szPath)
{
	XCheckSum _crc;

	vector< _FILEINFO> vPathList;
	_crc.GetFileInfo( szPath, &vPathList);


	// Output listbox
	CListBox* pListBox = (CListBox*)GetDlgItem( IDC_LIST);
	if ( pListBox != NULL)
	{
		pListBox->ResetContent();


		for ( vector< _FILEINFO>::iterator itr = vPathList.begin();  itr != vPathList.end();  itr++)
		{
			char _buff[ 512];
			sprintf_s( _buff, "%05u,%03u,%010u,%s", (*itr).nCheckSum, (*itr).nCheckBit, (*itr).nFileSize, (*itr).strName.c_str());

			pListBox->AddString( _buff);
		}
	}



	// Output encrypted data
	string strCRC;
	for ( vector< _FILEINFO>::iterator itr = vPathList.begin();  itr != vPathList.end();  itr++)
	{
		char _buff[ 512];
		sprintf_s( _buff, "%05u,%03u,%010u,%s\n", (*itr).nCheckSum, (*itr).nCheckBit, (*itr).nFileSize, (*itr).strName.c_str());
		strCRC += _buff;
	}


	XCheckSumEncrypt Encryptor;
	size_t _size = strCRC.length() + 1;
	char* szEncrypt = new char[ _size];
	Encryptor.Encrypt( strCRC.c_str(), szEncrypt, _size);

	FILE* pFile;
	char szFileName[ MAX_PATH];
	sprintf_s( szFileName, "%s\\file_crc.msf", szPath);
	fopen_s( &pFile, szFileName, "wb");
	if ( pFile != NULL)
	{
		fwrite( szEncrypt, 1, _size, pFile);
		fclose( pFile);
	}

	delete [] szEncrypt;


/*
	{
		FILE* fp;
		fopen_s( &fp, szFileName, "rb");
		if ( fp != NULL)
		{
			fseek( fp, 0, SEEK_END);

			long _filesize = ftell( fp);
			fseek( fp, 0, SEEK_SET);

			char* _buff = new char[ _filesize];
			fread( _buff, 1 , _filesize, fp);

			char* szDecrypt = new char[ _filesize];
			Encryptor.Decrypt( _buff, szDecrypt, _filesize);

			delete [] _buff;
			delete [] szDecrypt;

			fclose( fp);
		}
	}
*/
}
