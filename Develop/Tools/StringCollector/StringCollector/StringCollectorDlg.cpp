
// StringCollectorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "StringCollector.h"
#include "StringCollectorDlg.h"
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStringCollectorDlg 대화 상자



CStringCollectorDlg::CStringCollectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStringCollectorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStringCollectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStringCollectorDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SEARCH, &CStringCollectorDlg::OnBnClickedSearch)
	ON_BN_CLICKED(IDC_SEARCH2, &CStringCollectorDlg::OnBnClickedSearch2)
	ON_BN_CLICKED(IDC_START, &CStringCollectorDlg::OnBnClickedStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CStringCollectorDlg 메시지 처리기

BOOL CStringCollectorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
//	setlocale( LC_ALL, "kor");

	GetDlgItem( IDC_PATH)->SetWindowText( AfxGetApp()->GetProfileString( "Option", "input path", ""));
	GetDlgItem( IDC_PATH2)->SetWindowText( AfxGetApp()->GetProfileString( "Option", "output path", ""));
	GetDlgItem( IDC_EXTENSIONS)->SetWindowText( AfxGetApp()->GetProfileString( "Option", "Include extensions", "c,cpp,xml,lua"));
	((CButton*)GetDlgItem( IDC_INCLUDESUBPATH))->SetCheck( AfxGetApp()->GetProfileInt( "Option", "Include sub path", 0));
	((CButton*)GetDlgItem( IDC_EXCLUDEHIDEFOLDER))->SetCheck( AfxGetApp()->GetProfileInt( "Option", "Exclude hidden path", 1));
	GetDlgItem( IDC_OUTEXTENSION)->SetWindowText( AfxGetApp()->GetProfileString( "Option", "Out extension", "txt"));
	((CButton*)GetDlgItem( IDC_INCLUDEORGSENT))->SetCheck( AfxGetApp()->GetProfileInt( "Option", "Include original sentence", 1));
	((CButton*)GetDlgItem( IDC_CHECKMBWORD))->SetCheck( AfxGetApp()->GetProfileInt( "Option", "Check multibyte word", 1));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


void CStringCollectorDlg::OnDestroy()
{
	CDialog::OnDestroy();

	CString str;
	GetDlgItem( IDC_PATH)->GetWindowText( str);
	AfxGetApp()->WriteProfileString( "Option", "input path", str);
	GetDlgItem( IDC_PATH2)->GetWindowText( str);
	AfxGetApp()->WriteProfileString( "Option", "output path", str);
	GetDlgItem( IDC_EXTENSIONS)->GetWindowText( str);
	AfxGetApp()->WriteProfileString( "Option", "Include extensions", str);
	AfxGetApp()->WriteProfileInt( "Option", "Include sub path", ((CButton*)GetDlgItem( IDC_INCLUDESUBPATH))->GetCheck());
	AfxGetApp()->WriteProfileInt( "Option", "Exclude hidden path", ((CButton*)GetDlgItem( IDC_EXCLUDEHIDEFOLDER))->GetCheck());
	GetDlgItem( IDC_OUTEXTENSION)->GetWindowText( str);
	AfxGetApp()->WriteProfileString( "Option", "Out extension", str);
	AfxGetApp()->WriteProfileInt( "Option", "Include original sentence", ((CButton*)GetDlgItem( IDC_INCLUDEORGSENT))->GetCheck());
	AfxGetApp()->WriteProfileInt( "Option", "Check multibyte word", ((CButton*)GetDlgItem( IDC_CHECKMBWORD))->GetCheck());
}



// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CStringCollectorDlg::OnPaint()
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
HCURSOR CStringCollectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


static int CALLBACK BrowserCallbackProc( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch ( uMsg)
	{
	case BFFM_INITIALIZED :
		SendMessage( hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData);
		break;
	}

	return 0;
}


void CStringCollectorDlg::OnBnClickedSearch()
{
	CString strPath;
	GetDlgItem( IDC_PATH)->GetWindowText( strPath);

	char pszPathName[ MAX_PATH];
	BROWSEINFO BrInfo;
	memset( &BrInfo, 0, sizeof(BrInfo));
	BrInfo.hwndOwner = m_hWnd;
	BrInfo.pidlRoot = NULL;
	BrInfo.pszDisplayName = pszPathName;
	BrInfo.lpszTitle = "시작 경로를 선택하시기 바랍니다.";
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	BrInfo.lpfn = BrowserCallbackProc;
	BrInfo.lParam = (LPARAM)strPath.GetBuffer();

	ITEMIDLIST *pidlBrowse; 
	pidlBrowse = ::SHBrowseForFolder( &BrInfo);
	if ( pidlBrowse != NULL)
	{
		::SHGetPathFromIDList( pidlBrowse, pszPathName);
		
		GetDlgItem( IDC_PATH)->SetWindowText( pszPathName);
	}
}


void CStringCollectorDlg::OnBnClickedSearch2()
{
	CString strPath;
	GetDlgItem( IDC_PATH2)->GetWindowText( strPath);

	if ( strPath.IsEmpty() == true)
		GetDlgItem( IDC_PATH)->GetWindowText( strPath);

	char pszPathName[ MAX_PATH];
	BROWSEINFO BrInfo;
	memset( &BrInfo, 0, sizeof(BrInfo));
	BrInfo.hwndOwner = m_hWnd;
	BrInfo.pidlRoot = NULL;
	BrInfo.pszDisplayName = pszPathName;
	BrInfo.lpszTitle = "출력 경로를 선택하시기 바랍니다.";
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	BrInfo.lpfn = BrowserCallbackProc;
	BrInfo.lParam = (LPARAM)strPath.GetBuffer();

	ITEMIDLIST *pidlBrowse; 
	pidlBrowse = ::SHBrowseForFolder( &BrInfo);
	if ( pidlBrowse != NULL)
	{
		::SHGetPathFromIDList( pidlBrowse, pszPathName);

		GetDlgItem( IDC_PATH2)->SetWindowText( pszPathName);
	}
}


struct PathFileInfo
{
	CString				strPathName;
	vector< CString>	vFileList;
};
typedef vector< PathFileInfo>	PATHFILEINFO;

void GetFileList( const char* szPath, PATHFILEINFO* pPathList, const char* szExtensions, bool bIncludeSubFolder, bool bExcludeHideFolder)
{
	PathFileInfo info;
	info.strPathName = szPath;

	// Get file
	{
		char* _extensions = _strdup( szExtensions ? szExtensions : "*");
		char* _next_tok, *_token = strtok_s( _extensions, ";, ", &_next_tok);
		while ( _token != NULL)
		{
			char _path[ MAX_PATH];
			sprintf_s( _path, "%s/*.%s", szPath, _token);

			long hFile;
			struct _finddata_t file_t;
			if ( (hFile = _findfirst( _path, &file_t)) != -1L)
			{
				do
				{
					if ( file_t.attrib & _A_SUBDIR)
						continue;

					char _pathfilename[ MAX_PATH];
					sprintf_s( _pathfilename, "%s/%s", szPath, file_t.name);
					info.vFileList.push_back( _pathfilename);

				}	while( _findnext( hFile, &file_t) == 0);

				_findclose( hFile);
			}

			_token = strtok_s( NULL, ";, ", &_next_tok);
		}

		free( _extensions);
	}

	if ( info.vFileList.empty() == false)
		pPathList->push_back( info);


	// Get sub directory
	if ( bIncludeSubFolder == true)
	{
		char _path[ MAX_PATH];
		sprintf_s( _path, "%s/*.*", szPath);

		long hFile;
		struct _finddata_t file_t;
		if ( (hFile = _findfirst( _path, &file_t)) != -1L)
		{
			do
			{
				if ( (file_t.attrib & _A_SUBDIR) == 0  ||  strcmp( file_t.name, ".") == 0  ||  strcmp( file_t.name, "..") == 0)
					continue;

				if ( bExcludeHideFolder == true  &&  file_t.attrib & _A_HIDDEN)
					continue;

				char _pathname[ MAX_PATH];
				sprintf_s( _pathname, "%s/%s", szPath, file_t.name);
				GetFileList( _pathname, pPathList, szExtensions, bIncludeSubFolder, bExcludeHideFolder);

			}	while( _findnext( hFile, &file_t) == 0);

			_findclose( hFile);
		}
	}
}


size_t ParseBracket( wstring& in_str, size_t p, wstring* out_str)
{
	size_t arg = 0;
	size_t len = in_str.length();
	wstring s;
	bool stop = false;
	for ( p;  p < len;  p++)
	{
		wchar_t c = in_str.at( p);
		if ( !stop  &&  c == L'(')
		{
			p = ParseBracket( in_str, p + 1, out_str);
			wchar_t tmp[ 32];	swprintf_s( tmp, L"$%d", arg++);
			s += tmp;
			stop = true;
		}
		else if ( c == L')')	break;
		else if ( !stop  &&  c == L'"')
		{
			size_t end = p + 1;
			while ( in_str.at( end) != L'"'  ||  ( in_str.at( end - 1) == L'\\'  &&  in_str.at( end - 2) != L'\\'))	end++;
			s += in_str.substr( p + 1, end - p - 1);
			p = end;
			stop = true;
		}
		else if ( !stop  &&  c == L'$')
		{
			wchar_t tmp[ 32];	swprintf_s( tmp, L"$%d", arg++);
			s += tmp;
			stop = true;
		}
		else if ( c == L'+')
		{
			stop = false;
			continue;
		}
		else
		{
			p--;
			break;
		}
	}

	if ( s.empty() == false)
	{
		static wstring comp( L"$0$1$2$3$4$5$6$7$8$9$10$11$12$13$14$15$16$17$18$19$20");
		size_t l = s.length();
		if ( comp.compare( 0, s.length(), s) != 0)
		{
			if ( out_str->empty() == false)	*out_str += L"\r\n";
			*out_str += s;
		}
	}
	return p;
}


bool GetString( wstring& _in, wstring& _out, CString& _ext)
{
#define _TRIM(s)			{	wstring::size_type n;																			\
								n = s.find_first_not_of( L" ");		if ( n != wstring::npos)  s = s.substr( n, s.length());		\
								n = s.find_first_not_of( L"\t");	if ( n != wstring::npos)  s = s.substr( n, s.length());		\
							}
#define _PICKSTR(s,t1,t2)	{	size_t len = wcslen( t1), st, ed;									\
								if ( (st = s.find( t1, 0)) != wstring.npos)  {						\
									if ( (ed = s.find( t2, st + len)) != wstring.npos)  {			\
										wstring __str = _in.substr( st + len, ed - st - len);		\
										if ( __str.empty() == false  &&  __str.at( 0) != L'#')  {	\
											if ( str.empty() == false)  str += L"\r\n";				\
											str += _in.substr( st + len, ed - st - len);			\
							}	}	}	}

	wstring str;
	if ( _ext == ".c"  ||  _ext == ".cpp"  ||  _ext == ".lua")
	{
		wstring _text( _in);
		_TRIM( _text);
		
		size_t p, len = _text.length();
		for ( size_t i = 0;  i < len;  i++)
		{
			wchar_t c = _text.at( i);
			if ( c == L'"')
				do	i++;	while ( _text.at( i) != L'"'  ||  ( _text.at( i - 1) == L'\\'  &&  _text.at( i - 2) != L'\\'));
			else if ( c == '/'  &&  len > 2  &&  i < ( len - 1)  &&  _text.at( i + 1) == '/'  &&  ( _ext == ".c"  ||  _ext == ".cpp"))
			{
				_text.erase( i, len - i);
				break;
			}
			else if ( c == L'\'')
			{
				int del = ( _text.at( i + 1) == L'\\') ? 4 : 3;
				_text.erase( i, del);
				len -= del;
				i--;
			}
			else if ( c == L'[')	_text.replace( i, 1, L"(");
			else if ( c == L']')	_text.replace( i, 1, L")");
			else if ( c == L'.'  &&  i < (len - 1)  &&  _text.at( i + 1) == L'.')	_text.replace( i, 2, L"+ ");
			else if ( c != L' '  &&  c != L'+'  &&   c != L'('  &&  c != L')')		_text.replace( i, 1, L"_");

			if ( i > 0  &&  _text.at( i) == L'('  &&  _text.at( i - 1) == L'_'  &&  (p = _text.find_last_not_of( L"_", i - 1)) != wstring.npos)
			{
				_text.erase( p + 1, i - p - 1);
				len = _text.length();
				i = p + 1;
			}
		}

		len = _text.length();
		for ( size_t i = 0;  i < len;  i++)
		{
			wchar_t c = _text.at( i);
			if ( c == L'"')
				do	i++;	while ( _text.at( i) != L'"'  ||  ( _text.at( i - 1) == L'\\'  &&  _text.at( i - 2) != L'\\'));
			else if ( c == L'_')
			{
				size_t end = _text.find_first_not_of( L"_", i);
				if ( end == wstring.npos)	end = len;
				size_t next = _text.find_first_not_of( L"\t ", end);
				if ( next == wstring.npos)	next = len;

				if ( i > 0  &&  _text.at( i - 1) == L'+')
				{
					_text.replace( i, end - i, L"$");
					len = _text.length();
				}
				else if ( next < len  &&  _text.at( next) == L'+')
				{
					_text.replace( i, next - i, L"$");
					len = _text.length();
					i++;
				}
				else
				{
					_text.erase( i, next - i);
					len = _text.length();
					i--;
				}
			}
			else if ( c == L' ')
			{
				_text.erase( i, 1);
				len--;
				i--;
			}
		}

		len = _text.length();
		for ( size_t i = 0;  i < len;  i++)
			i = ParseBracket( _text, i, &str);
	}

	else if ( _ext == ".xml")
	{
		wstring _text( _in);
		transform( _text.begin(), _text.end(), _text.begin(), tolower);

		_PICKSTR( _text, L"<text>", L"</text>");
		_PICKSTR( _text, L"<tip>", L"</tip>");
		_PICKSTR( _text, L"<desc>", L"</desc>");
		_PICKSTR( _text, L"name=\"", L"\"");
		_PICKSTR( _text, L"title=\"", L"\"");
		_PICKSTR( _text, L"desc=\"", L"\"");
		_PICKSTR( _text, L"comment=\"", L"\"");
		_PICKSTR( _text, L"category=\"", L"\"");
		_PICKSTR( _text, L"string=\"", L"\"");
		_PICKSTR( _text, L"text=\"", L"\"");
		_PICKSTR( _text, L"usage=\"", L"\"");
		_PICKSTR( _text, L"help=\"", L"\"");
	}

	if ( str.empty() == false)
	{
		_TRIM( _in);
		replace( _in.begin(), _in.end(), L'\t', L' ');

		_out = str;
		return true;
	}

	return false;
}

static CString gLastFile;
static int gLastLine;
static wstring gLastString;

bool CollectString( const char* szFilename, vector< wstring>* vStringList, bool bIncludeOrgStc, bool bCheckMbyteWord)
{
	gLastFile = szFilename;

	char _drive[ _MAX_DRIVE], _dir[ _MAX_DIR], _fname[ _MAX_FNAME], _ext[ _MAX_EXT];
	_splitpath_s( szFilename, _drive, _MAX_DRIVE, _dir, _MAX_DIR, _fname, _MAX_FNAME, _ext, _MAX_EXT);

	CString extension( _ext);
	extension.MakeLower();


	HANDLE hFile = ::CreateFile( szFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE)
		return false;

	size_t _size = ::GetFileSize( hFile, NULL) + 1;
	char* pData = new char[ _size];
	DWORD _bytes_read;
	if ( ::ReadFile( hFile, pData, _size - 1, &_bytes_read, NULL) == FALSE)
	{
		delete [] pData;
		return false;
	}
	pData[ _size - 1] = '\n';
	::CloseHandle( hFile);

	bool bUTF8 = false;
	if ( (unsigned char)pData[ 0] == 0xEF  &&  (unsigned char)pData[ 1] == 0xBB  &&  (unsigned char)pData[ 2] == 0xBF)
		bUTF8 = true;
	if ( bUTF8 == false  &&  extension == ".xml"  &&  ( strstr( pData, "encoding=\"UTF-8\"") != 0  ||  strstr( pData, "encoding=\"utf-8\"") != 0))
		bUTF8 = true;

	size_t nSize = MultiByteToWideChar( bUTF8 ? CP_UTF8 : CP_ACP, 0, pData, -1, NULL, NULL);
	wchar_t* wData = new wchar_t[ nSize];
	MultiByteToWideChar( bUTF8 ? CP_UTF8 : CP_ACP, 0, pData, _size, wData, nSize);
	delete [] pData;


	wstring sentence;
	int line = 1;
	for ( size_t i = 0;  i < nSize;  i++)
	{
		if ( i > 0  &&  wData[ i] == L'\n'  &&  wData[ i - 1] == L'\r')
			continue;

		if ( wData[ i] == L'\r'  ||  wData[ i] == L'\n')
		{
			wstring str;
			gLastLine = line;
			gLastString = sentence;
			if ( GetString( sentence, str, extension) == true)
			{
				bool bValide = false;
				if ( bCheckMbyteWord == true)
				{
					const wchar_t* p = str.c_str();
					while ( *p != NULL)
					{
						if ( *p & 0xFF00)
						{
							bValide = true;
							break;
						}
						*p++;
					}
				}
				else
					bValide = true;

				if ( bValide == true)
				{
					if ( bIncludeOrgStc == true)
					{
						size_t _size = sentence.size() + 64;
						wchar_t* tmp = new wchar_t[ _size];
						swprintf_s( tmp, _size, L">>[%d] %s", line, sentence.c_str());
						vStringList->push_back( tmp);
						delete [] tmp;
					}

					vStringList->push_back( str + (bIncludeOrgStc ? L"\r\n" : L""));
				}
			}

			sentence.clear();
			line++;
		}
		else
			sentence += wData[ i];
	}

	delete [] wData;

	return ( vStringList->empty() ? false : true);
}


void CStringCollectorDlg::OnBnClickedStart()
{
	CString strPath;
	GetDlgItem( IDC_PATH)->GetWindowText( strPath);
	if ( strPath.IsEmpty() == true)
	{
		MessageBox( "시작 위치를 지정하지 않았습니다.", "확인", MB_OK);
		return;
	}

	CString strOutPath;
	GetDlgItem( IDC_PATH2)->GetWindowText( strOutPath);
	if ( strOutPath.IsEmpty() == true)
	{
		MessageBox( "출력 위치를 지정하지 않았습니다.", "확인", MB_OK);
		return;
	}


	CString strExtensions;
	GetDlgItem( IDC_EXTENSIONS)->GetWindowText( strExtensions);
	if ( strExtensions.IsEmpty() == true)
	{
		MessageBox( "파일 확장자를 지정하지 않았습니다.", "확인", MB_OK);
		return;
	}

	CString strOutExtension;
	GetDlgItem( IDC_OUTEXTENSION)->GetWindowText( strOutExtension);
	if ( strOutExtension.IsEmpty() == true)
	{
		MessageBox( "출력 파일의 확장자를 지정하지 않았습니다.", "확인", MB_OK);
		return;
	}

	bool bIncludeSubPath = ((CButton*)GetDlgItem( IDC_INCLUDESUBPATH))->GetCheck() ? true : false;
	bool bExcludeHideFolder = ((CButton*)GetDlgItem( IDC_EXCLUDEHIDEFOLDER))->GetCheck() ? true : false;
	bool bIncludeOriginalSentence = ((CButton*)GetDlgItem( IDC_INCLUDEORGSENT))->GetCheck() ? true : false;
	bool bCheckMultibyteWord = ((CButton*)GetDlgItem( IDC_CHECKMBWORD))->GetCheck() ? true : false;


	// Get file list
	PATHFILEINFO vFileList;
	GetFileList( strPath.GetBuffer(), &vFileList, strExtensions.GetBuffer(), bIncludeSubPath, bExcludeHideFolder);

	// Collect strings
	for ( PATHFILEINFO::iterator itr = vFileList.begin();  itr != vFileList.end();  itr++)
	{
		for ( vector< CString>::iterator _fitr = (*itr).vFileList.begin(); _fitr != (*itr).vFileList.end();  _fitr++)
		{
			vector< wstring> vStringList;
			if ( CollectString( (*_fitr).GetBuffer(), &vStringList, bIncludeOriginalSentence, bCheckMultibyteWord) == false)
				continue;

			char _drive[ _MAX_DRIVE], _dir[ _MAX_DIR], _fname[ _MAX_FNAME], _ext[ _MAX_EXT];
			_splitpath_s( (*_fitr).GetBuffer(), _drive, _MAX_DRIVE, _dir, _MAX_DIR, _fname, _MAX_FNAME, _ext, _MAX_EXT);

			char szFilename[ MAX_PATH];
			sprintf_s( szFilename, "%s/%s%s.%s", strOutPath.GetBuffer(), _fname, _ext, strOutExtension.GetBuffer());

			HANDLE hFile = ::CreateFile( szFilename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if ( hFile == INVALID_HANDLE_VALUE)
				continue;

			for ( vector< wstring>::iterator _sitr = vStringList.begin();  _sitr != vStringList.end();  _sitr++)
			{
				size_t nSize = WideCharToMultiByte( CP_ACP, 0, (*_sitr).c_str(), -1, NULL, 0,NULL, NULL);
				char* pData = new char[ nSize];
				WideCharToMultiByte( CP_ACP, 0, (*_sitr).c_str(), -1, pData, nSize, 0, 0);

				DWORD _bytes_write;
				::WriteFile( hFile, pData, nSize, &_bytes_write, NULL);
				::WriteFile( hFile, "\r\n", 2, &_bytes_write, NULL);

				delete [] pData;
			}

			::CloseHandle( hFile);
		}
	}

	MessageBox( "작업을 완료하였습니다.", "확인", MB_OK);
}
