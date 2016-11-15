
#pragma once

// 안티 핵 모드는 배포용 한국 버젼일 때 동작함
#if defined( _PUBLISH)  &&  defined( _KOREA)
//#define XANTIHACK
#endif


class XAntiHack;
class XAntiHackModule;
class XAntiHackAuthInfo;




// XAntiHackAuthInfo
class XAntiHackAuthInfo
{
/// Member functions
public:
	XAntiHackAuthInfo()					{}
	virtual ~XAntiHackAuthInfo()		{}
};





/// class XAntiHackModule
class XAntiHackModule
{
	friend class XAntiHack;


/// Member functions
public:
	XAntiHackModule()												{}
	virtual ~XAntiHackModule()										{}

	virtual DWORD GetLastErrorCode()								{ return 0;						}
	virtual const wchar_t* GetLastErrorMsg()						{ return NULL;					}



/// Event handler
protected:
	virtual bool OnCreate()											{ return false;					}	// 모듈을 생성해야 할 때 호출됩니다
	virtual bool OnDestroy()										{ return false;					}	// 모듈을 삭제해야 할 때 호출됩니다
	virtual void OnCreatedWindow( HWND hWnd)						{}									// 윈도우가 생성되었을 때 호출됩니다
	virtual void OnDestroyedWindow()								{}									// 윈도우가 삭제되었을 때 호출됩니다.
	virtual bool OnPlayerLogin( const wchar_t* szID)				{ return false;					}	// 플레이어가 로그인 했을 때 호출됩니다
	virtual bool OnPlayerLogout( const wchar_t* szID)				{ return false;					}	// 플레이어가 로그아웃 했을 때 호출됩니다
	virtual bool OnUpdate()											{ return false;					}	// 모듈을 업데이트 할 때 호출됩니다.
	virtual bool OnCheckAuthority( XAntiHackAuthInfo* pAuthInfo)	{ return false;					}	// 인증 확인을 요청할 때마다 호출됩니다
	virtual bool OnCheckedAuthority( XAntiHackAuthInfo* pAuthInfo)	{ return false;					}	// 인증 확인이 완료되었을 때마다 호출됩니다
	virtual void OnShowErrorMessageBox()							{}									// 에러 메시지 박스를 띄울 때 호출됩니다
};





/// class : XAntiHack
class XAntiHack
{
/// Member variables
protected:
	HWND				m_hWnd;
	XAntiHackModule*	m_pAntiHackModule;



/// Member functions
public:
	// Constructor/Destructor
	XAntiHack();
	virtual ~XAntiHack();

	// Create/Remove anti-hack module
	bool CreateModule();
	bool RemoveModule();


	// Get singleton instance
	static XAntiHack* GetInstance();


	// Show error message box
	void ShowErrorMessageBox();


	// Created/Destroyed window
	void CreatedWindow( HWND hWnd);
	void DestroyedWindow();

	// Player login/logout
	void PlayerLogin( const wchar_t* szID);
	void PlayerLogout( const wchar_t* szID);

	// Check authority
	bool CheckAuthority( XAntiHackAuthInfo* pInfo);
	bool CheckedAuthority( XAntiHackAuthInfo* pInfo);

	// Update
	void Update();
};





/// inline : Get anti-hack instance
inline XAntiHack* XGetAntiHack()
{
	return XAntiHack::GetInstance();
}
