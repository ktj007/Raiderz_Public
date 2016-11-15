
#pragma once


#ifdef _UNICODE
#define _STRING		wstring
#define _CHAR		wchar_t
#else
#define _STRING		string
#define _CHAR		char
#endif

// Type of errors
enum
{
	ERRORTYPE_NONE				= 0,
	ERRORTYPE_APPLICATION,
	ERRORTYPE_SYSTEM,
	ERRORTYPE_RESOURCE,
	ERRORTYPE_ANTIHACK
};





/// class : XAppErrorCollector
class XAppErrorCollector
{
/// Member variables
protected:
	HWND		m_hWnd;
	int			m_nLastErrorType;
	_STRING		m_strLastErrorMsg;
	bool		m_bExitApp;
	DWORD		m_dwExitTimer;



/// Member functions
public:
	XAppErrorCollector();
	virtual ~XAppErrorCollector();

	void SetWindowHandle( HWND hWnd);

	void SetLastError( int nType, const _CHAR* szMsg);
	void ExitApplication( DWORD dwDelay);

	int GetLastErrorType() const;
	const _CHAR* GetLastErrorMsg();

	static XAppErrorCollector* GetInstance();

	void Update();


protected:
	void _exit_application();
};



// inline : XGetAppErrorCollector
inline XAppErrorCollector* XGetAppErrorCollector()
{
	return XAppErrorCollector::GetInstance();
}
