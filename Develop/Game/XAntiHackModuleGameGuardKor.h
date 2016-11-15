
#pragma once

#include "XAntiHack.h"

#if defined( XANTIHACK)  &&  defined( _KOREA)

#include "../Extern/nProtect/Korea/Client/NPGameLib.h"

#endif



// XAntiHackAuthInfoGameGuardKor
class XAntiHackAuthInfoGameGuardKor : public XAntiHackAuthInfo
{
/// Member variables
public:
#if defined( XANTIHACK)  &&  defined( _KOREA)

	GG_AUTH_DATA		m_AuthInfo;

#endif


/// Member functions
public:
	XAntiHackAuthInfoGameGuardKor();
	XAntiHackAuthInfoGameGuardKor( void* pAuthInfo);
	virtual ~XAntiHackAuthInfoGameGuardKor();
};





/// class XAntiHackModuleGameGuardKor
class XAntiHackModuleGameGuardKor : public XAntiHackModule
{
/// Member variables
protected:
	bool			m_bCreated;
	DWORD			m_dwErrorCode;
	wstring			m_strErrorMsg;
	DWORD			m_dwTimer;



/// Member functions
public:
	XAntiHackModuleGameGuardKor();
	virtual ~XAntiHackModuleGameGuardKor();

	virtual DWORD GetLastErrorCode() override;
	virtual const wchar_t* GetLastErrorMsg() override;

	void SetError( DWORD dwErrorCode);


/// Event handler
protected:
	virtual bool OnCreate() override;
	virtual bool OnDestroy() override;
	virtual void OnCreatedWindow( HWND hWnd) override;
	virtual bool OnPlayerLogin( const wchar_t* szID) override;
	virtual bool OnPlayerLogout( const wchar_t* szID) override;
	virtual bool OnUpdate() override;
	virtual bool OnCheckAuthority( XAntiHackAuthInfo* pAuthInfo) override;
	virtual bool OnCheckedAuthority( XAntiHackAuthInfo* pAuthInfo) override;
	virtual void OnShowErrorMessageBox() override;
};
