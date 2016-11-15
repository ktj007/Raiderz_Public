// EventDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AddAnimationDlg.h"
#include "MFileSystem.h"

// CAddAnimationDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAddAnimationDlg, CDialog)

CAddAnimationDlg::CAddAnimationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddAnimationDlg::IDD, pParent)
	, m_bEditMode(false)
{
}

CAddAnimationDlg::~CAddAnimationDlg()
{
}

void CAddAnimationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_MSG_MAP(CAddAnimationDlg)
	DDX_Text(pDX, IDC_EDIT_ANINAME, m_strAniName);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDX_CBIndex(pDX, IDC_COMBO_LOOP, m_nLoop);
	DDX_Text(pDX, IDC_EDIT_SPEED, m_strSpeed);
	//}}AFX_MSG_MAP
}


BEGIN_MESSAGE_MAP(CAddAnimationDlg, CDialog)
	//{{AFX_MSG_MAP(CAddAnimationDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_FINDFILE, &CAddAnimationDlg::OnBnClickedButtonFindfile)
END_MESSAGE_MAP()


// CAddAnimationDlg 메시지 처리기입니다.
BOOL CAddAnimationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if ( m_strAniName.IsEmpty() == TRUE  &&  m_strFileName.IsEmpty() == FALSE)
	{
		char drive[ _MAX_DRIVE], dir[ _MAX_DIR], fname[ _MAX_FNAME], ext[ _MAX_EXT];
		_splitpath( m_strFileName.GetBuffer(), drive, dir, fname, ext);

		m_strAniName = fname;
		m_strAniName.Replace( ".elu", "");
	}


	GetDlgItem( IDC_EDIT_ANINAME)->EnableWindow( (m_bEditMode == true) ? TRUE : FALSE);
	GetDlgItem( IDC_EDIT_FILENAME)->EnableWindow( (m_bEditMode == true) ? TRUE : FALSE);
	GetDlgItem( IDC_BUTTON_FINDFILE)->EnableWindow( (m_bEditMode == true) ? TRUE : FALSE);

	return TRUE;
}

void CAddAnimationDlg::OnBnClickedButtonFindfile()
{
	CString t_str;
	char	t_filename[256];

	CFileDialog dlg( TRUE, _T("ani"), GetMainView()->GetProjectType()->GetPathRoot(), OFN_HIDEREADONLY, _T("Animation File (*.ani)|*.ani|"));

	if ( dlg.DoModal() == IDCANCEL)
		return;

	t_str = dlg.GetPathName();
	lstrcpy( (LPSTR)t_filename, (LPSTR)t_str.operator const char*());


	m_strFileName = t_filename;

	char drive[ _MAX_DRIVE], dir[ _MAX_DIR], fname[ _MAX_FNAME], ext[ _MAX_EXT];
	_splitpath( m_strFileName.GetBuffer(), drive, dir, fname, ext);

	m_strAniName = fname;
	m_strAniName.Replace( ".elu", "");

	UpdateData( FALSE);
}

void CAddAnimationDlg::SetEditMode( bool bEdit)
{
	m_bEditMode = bEdit;
}

void CAddAnimationDlg::SetAniName( const char* szAniName)
{
	m_strAniName = szAniName;
}

void CAddAnimationDlg::SetFileName( const char* szFileName)
{
	m_strFileName = szFileName;
}

void CAddAnimationDlg::SetLoop( bool bLoop)
{
	if ( bLoop == true)
		m_nLoop = 0;
	else
		m_nLoop = 1;
}

void CAddAnimationDlg::SetSpeed( float fSpeed)
{
	m_strSpeed.Format( "%.1f", fSpeed);
}

const char* CAddAnimationDlg::GetAniName()
{
	return m_strAniName.GetBuffer();
}

const char* CAddAnimationDlg::GetFileName()
{
	return m_strFileName.GetBuffer();
}

bool CAddAnimationDlg::GetLoop()
{
	if ( m_nLoop == 0)
		return true;

	return false;
}

float CAddAnimationDlg::GetSpeed()
{
	double speed = atof( m_strSpeed.GetBuffer());
	return (float)speed;
}
