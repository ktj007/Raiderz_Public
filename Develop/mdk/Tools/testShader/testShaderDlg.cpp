// testShaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testShader.h"
#include "testShaderDlg.h"
#include <io.h>
#include "../../RealSpace3/RShaderComposer.h"
#include <sstream>
#include <vector>
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CtestShaderDlg dialog




CtestShaderDlg::CtestShaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CtestShaderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestShaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ALL, m_lbAllComponents);
	DDX_Control(pDX, IDC_LIST_ADDED, m_lbAddedComponents);
}

BEGIN_MESSAGE_MAP(CtestShaderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST_ALL, &CtestShaderDlg::OnLbnDblclkListAll)
	ON_BN_CLICKED(IDOK, &CtestShaderDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CtestShaderDlg message handlers

BOOL CtestShaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	RefreshAllComponents();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CtestShaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CtestShaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CtestShaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtestShaderDlg::RefreshAllComponents()
{
	struct _finddata_t fileInfo;
	intptr_t hFile;
	
	char szFileSpec[256] = "*.rsc\0";
	if ( (hFile = _findfirst( szFileSpec, &fileInfo)) != -1 )
	{
		m_lbAllComponents.AddString( fileInfo.name );

		while ( _findnext( hFile, &fileInfo) == 0 )
		{
			m_lbAllComponents.AddString( fileInfo.name );
		}
	_findclose( hFile );
	}
}

void CtestShaderDlg::OnLbnDblclkListAll()
{
	int nSelected = m_lbAllComponents.GetCurSel();

	CString strSelected;
	m_lbAllComponents.GetText(nSelected, strSelected);

	m_lbAddedComponents.AddString(strSelected);

}

void CtestShaderDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();

	using namespace rs3;
	using namespace std;

	RShaderComposer compositor;

	vector<RShaderComponent> components;
	int nComponent = m_lbAddedComponents.GetCount();
	components.resize( nComponent );
	for(int i=0; i<nComponent; ++i)
	{
		CString strComponent;
		m_lbAddedComponents.GetText(i, strComponent);

		if(!components[i].Create( (LPCSTR)strComponent, "" ))
		{
			MessageBox( components[i].GetErrorString().c_str() );
			return;
		}

		if(!compositor.AddComponent(&components[i]))
		{
			MessageBox( compositor.GetErrorString().c_str() );
			return;
		}
	}

	compositor.Make();
	string strShaderString = compositor.GetShaderString();

	const char* OUTPUTFILENAME = "output.txt";

	FILE* file;
	file = fopen(OUTPUTFILENAME,"w+");
	fprintf( file, strShaderString.c_str());
	fclose(file);

//	WinExec("notepad.exe output.fx", SW_SHOW);
	ShellExecute( NULL, "open", OUTPUTFILENAME , NULL, NULL, SW_SHOW);
}
