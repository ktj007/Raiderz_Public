
#include "stdafx.h"
#include "XUITray.h"
#include "Mint.h"
#include "MScriptManager.h"

#define UPDATE_TIMER_ELAPSED		200



// XUITray
XUITray::XUITray( const char* szName, MWidget* pParent, MListener* pListener)
: MWidget( szName, pParent, pListener)
{
	m_bEnableFocus = true;
	m_dwUpdateTimer = 0;
	m_nHotKey = 0;
}


// ~XUITray
XUITray::~XUITray()
{
	m_strScriptInstance.clear();
}


// GetState
const char* XUITray::GetState()
{
	if ( m_strScriptInstance == "luaTrayBase")			return MWS_DISABLE;
	else if ( IsMouseOver() == true)					return MWS_FOCUS;
	return MWS_NORMAL;
}


// GetStateNum
int XUITray::GetStateNum()
{
	if ( m_strScriptInstance == "luaTrayBase")			return MWSN_DISABLE;
	else if ( IsMouseOver() == true)					return MWSN_FOCUS;
	return MWSN_NORMAL;
}


// BindScriptInstance
void XUITray::BindScriptInstance( const char* szScriptInstance)
{
	if ( m_strScriptInstance == szScriptInstance)		return;

	
	m_strToolTip.clear();
	m_nHotKey = 0;


	if ( szScriptInstance == NULL  ||  szScriptInstance[ 0] == 0)
	{
		m_strScriptInstance.clear();
		return;
	}

	m_strScriptInstance = szScriptInstance;

	bool bRetVal = false;
	WMEMCALL1( MGetMint()->GetScriptManager()->GetLua(), m_strScriptInstance.c_str(), "OnInitialize", this, &bRetVal);
}


// GetScriptInstance
const char* XUITray::GetScriptInstance() const
{
	return m_strScriptInstance.c_str();
}


// OnQueryHotKey
bool XUITray::OnQueryHotKey( unsigned int nHotKey)
{
	return ( (m_nHotKey == nHotKey) ? true : false);
}


// OnHotKey
void XUITray::OnHotKey( unsigned int nHotKey)
{
	if ( m_strScriptInstance.empty() == true)		return;

	bool bRetVal = false;
	if ( MGetMint()->IsEnableCallScript() == true)
		WMEMCALL1( MGetMint()->GetScriptManager()->GetLua(), m_strScriptInstance.c_str(), "OnHotKey", this, &bRetVal);
}


// OnRun
void XUITray::OnRun( DWORD dwCurrTime)
{
	MWidget::OnRun( dwCurrTime);

	if ( m_strScriptInstance.empty() == false)
	{
		DWORD dwElapsed = dwCurrTime - m_dwUpdateTimer;
		if ( dwElapsed < UPDATE_TIMER_ELAPSED)		return;
		m_dwUpdateTimer = dwCurrTime;


		bool bRetVal = false;
		if ( MGetMint()->IsEnableCallScript() == true)
			WMEMCALL1( MGetMint()->GetScriptManager()->GetLua(), m_strScriptInstance.c_str(), "OnUpdate", this, &bRetVal);
	}
}


// OnDraw
void XUITray::OnDrawBorder( MDrawContext* pDC)
{
	if ( m_strScriptInstance.empty() == false)
	{
		bool bRetVal = false;
		if ( MGetMint()->IsEnableCallScript() == true)
			WMEMCALL1( MGetMint()->GetScriptManager()->GetLua(), m_strScriptInstance.c_str(), "OnDraw", this, &bRetVal);
	}

	MWidget::OnDrawBorder( pDC);
}


// OnClick
void XUITray::OnClick( MEvent* pEvent)
{
	if ( m_strScriptInstance.empty() == true)		return;

	MWidget::m_EventArgs.SetFromEvent( this, pEvent);

	bool bRetVal = false;
	if ( MGetMint()->IsEnableCallScript() == true)
		WMEMCALL1( MGetMint()->GetScriptManager()->GetLua(), m_strScriptInstance.c_str(), "OnClick", this, &bRetVal);
}


// OnRClick
void XUITray::OnRClick( MEvent* pEvent)
{
	if ( m_strScriptInstance.empty() == true)		return;

	MWidget::m_EventArgs.SetFromEvent( this, pEvent);

	bool bRetVal = false;
	if ( MGetMint()->IsEnableCallScript() == true)
		WMEMCALL1( MGetMint()->GetScriptManager()->GetLua(), m_strScriptInstance.c_str(), "OnRClick", this, &bRetVal);
}


// OnToolTip
MWidget* XUITray::OnToolTip()
{
	if ( m_strScriptInstance.empty() == true)		return NULL;

	bool bRetVal = false;
	if ( MGetMint()->IsEnableCallScript() == true)
		WMEMCALL1( MGetMint()->GetScriptManager()->GetLua(), m_strScriptInstance.c_str(), "OnToolTip", this, &bRetVal);

	return MWidget::OnToolTip();
}


// OnEnter
void XUITray::OnEnter()
{
	if ( m_strScriptInstance.empty() == true)		return;

	MWidget::m_EventArgs.SetFromEvent( this);

	bool bRetVal = false;
	if ( MGetMint()->IsEnableCallScript() == true)
		WMEMCALL1( MGetMint()->GetScriptManager()->GetLua(), m_strScriptInstance.c_str(), "OnEnter", this, &bRetVal);
}


// OnLeave
void XUITray::OnLeave()
{
	if ( m_strScriptInstance.empty() == true)		return;

	MWidget::m_EventArgs.SetFromEvent( this);

	bool bRetVal = false;
	if ( MGetMint()->IsEnableCallScript() == true)
		WMEMCALL1( MGetMint()->GetScriptManager()->GetLua(), m_strScriptInstance.c_str(), "OnLeave", this, &bRetVal);
}
