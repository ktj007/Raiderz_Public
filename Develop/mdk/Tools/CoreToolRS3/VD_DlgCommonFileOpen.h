/// @file VD_DlgCommonFileOpen.h
#pragma once
// from MSDN Magazine -- November 2004, by Paul DiLascia
// These command IDs were obtained by spelunking the File Open dialog with Spy++.
enum {
	ODM_UNDEFINED = 0,
	ODM_VIEW_ICONS = 0x7029,
	ODM_VIEW_LIST  = 0x702b,
	ODM_VIEW_DETAIL= 0x702c,
	ODM_VIEW_THUMBS= 0x702d,
	ODM_VIEW_TILES = 0x702e,
};

//////////////////
// This class is used to subclass the "SHELLDLL_DefView" window in the file
// open dialog.
//
class CListViewShellWnd : public CWnd
{
public:
	// These are the WM_COMMAND IDs to send to the shell window to set the
	// different folder views. These were obtained using Spy++ to examine
	// which messages are sent to the shell window.
	//
	DWORD m_lastViewMode;

	CListViewShellWnd() : m_lastViewMode(0)
	{
	}

	~CListViewShellWnd()
	{
	}

	BOOL SetViewMode(int cmd);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_DYNAMIC(CListViewShellWnd)
	DECLARE_MESSAGE_MAP()
};

/////////////////
// Customized file open dialog remembers state of folder view.
//

/** @brief 파일 열기 다이어로그 */
class CPersistOpenDlg : public CFileDialog {
public:
	CPersistOpenDlg();
	~CPersistOpenDlg();

protected:
	CListViewShellWnd m_wndListViewShell;

	virtual BOOL OnInitDialog();			 // handle init dialog
	afx_msg LRESULT OnPostInit(WPARAM wp, LPARAM lp); // the REAL initialization

	DECLARE_DYNAMIC(CPersistOpenDlg)
	DECLARE_MESSAGE_MAP()
};

/////////////
// 새로운 형식의 다이어로그를 추가시킬때는 여기서부터
//
/** @brief 일반적인 리스트 뷰 파일열기*/
class CListViewOpenDlg : public CPersistOpenDlg {
public:
	CListViewOpenDlg(const char* _lpstr) {
		m_ofn.Flags |= OFN_NOCHANGEDIR;
		m_ofn.lpstrInitialDir = _lpstr;
	}

	void SetTitle(const char* _szTitle){ m_ofn.lpstrTitle = _szTitle; }
	void SetFilter(const char* _szFilter){ m_ofn.lpstrFilter = _szFilter; }
	void AddStyle(DWORD _dwStyle){ m_ofn.Flags |= _dwStyle ; }
	~CListViewOpenDlg(){}

protected:
	afx_msg LRESULT OnPostInit(WPARAM wp, LPARAM lp); // the REAL initialization

	DECLARE_DYNAMIC(CListViewOpenDlg)
	DECLARE_MESSAGE_MAP()
};

/** @brief xml 파일열기*/
class CXMLOpenDlg : public CPersistOpenDlg {
public:
	CXMLOpenDlg(const char* _initDir, const char* _szFilter){
		m_ofn.lpstrTitle = _T("CoreToolRS - XML 파일을 선택해 주세요");
		//화면에 보일 필터명\0실제필터 / 추가할때는 \0를 붙여서 앞에것 반복
		m_ofn.lpstrFilter = _szFilter;
		//파일을 연다고 해서 현재위치 안바꾸기
		m_ofn.Flags |= OFN_NOCHANGEDIR;
		m_ofn.lpstrInitialDir = _initDir;
	}
	~CXMLOpenDlg(){}

protected:
	afx_msg LRESULT OnPostInit(WPARAM wp, LPARAM lp); // the REAL initialization

	DECLARE_DYNAMIC(CXMLOpenDlg)
	DECLARE_MESSAGE_MAP()
};

/** @brief bmp 타일 파일열기*/
class CBMPOpenDlg : public CPersistOpenDlg {
public:
	CBMPOpenDlg(const char* _lpstr, const char *_lpfilter = "비트맵 파일\0*.bmp") {
		m_ofn.lpstrTitle = _T("CoreToolRS - 비트맵 파일을 선택해 주세요");
		//화면에 보일 필터명\0실제필터
		m_ofn.lpstrFilter = _lpfilter;
		//파일을 연다고 해서 현재위치 안바꾸기
		m_ofn.Flags |= OFN_NOCHANGEDIR;
		m_ofn.lpstrInitialDir = _lpstr;
	}
	~CBMPOpenDlg(){}

protected:
	afx_msg LRESULT OnPostInit(WPARAM wp, LPARAM lp); // the REAL initialization

	DECLARE_DYNAMIC(CBMPOpenDlg)
	DECLARE_MESSAGE_MAP()
};
