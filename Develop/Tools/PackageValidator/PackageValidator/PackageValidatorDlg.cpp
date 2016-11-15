
// PackageValidatorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "PackageValidator.h"
#include "PackageValidatorDlg.h"
#include "MMaietXmlTestReporter.h"
#include "MMaietTestResult.h"
#include <stdio.h>
#include <io.h>
#include <vector>
#include <string>
#include <fstream>
using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPackageValidatorDlg 대화 상자
enum
{
	TIMER_START		= 0,
	TIMER_END
};



CPackageValidatorDlg::CPackageValidatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPackageValidatorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPackageValidatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPackageValidatorDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_START, &CPackageValidatorDlg::OnBnClickedStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CPackageValidatorDlg 메시지 처리기

BOOL CPackageValidatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_nSuccess = 0;
	m_nFail = 0;

	// Set timer
	SetTimer( TIMER_START, 1000, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPackageValidatorDlg::OnPaint()
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
HCURSOR CPackageValidatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPackageValidatorDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch ( nIDEvent)
	{
	case TIMER_START :
		KillTimer( TIMER_START);
		Validate();
		break;

	case TIMER_END :
		KillTimer( TIMER_END);
		exit( (m_nFail > 0)  ?  1 : 0);
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CPackageValidatorDlg::OnBnClickedStart()
{
	Validate();
}


struct PATHINFO
{
	string		strName;
	string		strPath;

	PATHINFO()	{}
	PATHINFO( const char* szName, const char* szPath) : strName( szName), strPath( szPath)		{}
};

void GetDirectories( const char* szPath, vector< PATHINFO>* pPathList)
{
	char _path[ MAX_PATH];
	sprintf_s( _path, "%s\\*.*", szPath);

	long hFile;
	struct _finddata_t file_t;

	if ( (hFile = _findfirst( _path, &file_t)) != -1L ) 
	{
		do
		{
			if ( (file_t.attrib & _A_SUBDIR) == 0 )		continue;
			if ( strcmp( file_t.name, "." ) == 0)		continue;
			if ( strcmp( file_t.name, "..") == 0)		continue;
			if ( strcmp( file_t.name, ".svn") == 0)		continue;


			char _subdir[ MAX_PATH];
			sprintf_s( _subdir, "%s\\%s", szPath, file_t.name);

			pPathList->push_back( PATHINFO( file_t.name, _subdir));

			GetDirectories( _subdir, pPathList);

		}	while( _findnext( hFile, &file_t) == 0);

		_findclose( hFile);
	}
}

void CPackageValidatorDlg::Validate()
{
	// Reset UI
	GetDlgItem( IDC_START)->EnableWindow( FALSE);

	CListBox* pListBox = (CListBox*)GetDlgItem( IDC_INVALIDLIST);
	if ( pListBox == NULL)		return;
	pListBox->ResetContent();


	// Read packaging.bat
	FILE* pFile = NULL;
	fopen_s( &pFile, "pack_common_resource.bat", "r");
	if ( pFile == NULL)		return;

	fseek( pFile, 0, SEEK_END);
	size_t _size = ftell( pFile);
	fseek( pFile, 0, SEEK_SET);
	char* szPackaging = new char[ _size];
	memset( szPackaging, 0, _size);
	fread( (void*)szPackaging, 1, _size, pFile);
	fclose( pFile);

	string strPackaging( szPackaging);
	delete [] szPackaging;



	// Read directories
	char szCurPath[ MAX_PATH];
	GetCurrentDirectory( sizeof( szCurPath), szCurPath);

	char szPath[ MAX_PATH];
	sprintf_s( szPath, "%s\\Data\\Model", szCurPath);

	vector< PATHINFO> vPathList;
	GetDirectories( szPath, &vPathList);


	// Check validate
	std::ofstream strOutLog( "packagevalidator.xml");
	MMaietTestResultList testResulList;
	MMaietXmlTestReporter xmlReporter( strOutLog);

	for ( vector< PATHINFO>::iterator itr = vPathList.begin();  itr != vPathList.end();  itr++)
	{
		if ( strPackaging.find( (*itr).strName, 0) == string::npos)
		{
			// 예외 처리
			if ( (*itr).strName == string( "_tool"))		continue;
			

			// 리스트 박스 출력
			pListBox->AddString( (*itr).strPath.c_str());


			// 로그 남김
			xmlReporter.AddMessage( (*itr).strPath.c_str());


			m_nFail++;
		}
		else
		{
			m_nSuccess++;
		}
	}

	xmlReporter.ReportSummary( m_nSuccess + m_nFail, m_nFail, m_nFail, 0.0f, testResulList);


	// Restore UI
	GetDlgItem( IDC_START)->EnableWindow( TRUE);


	// Set timer
	SetTimer( TIMER_END, 2000, NULL);
}
