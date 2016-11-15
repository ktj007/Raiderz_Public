#include "stdafx.h"
#include "XController.h"

XController::XController()
: m_bCreated(false)
{
}


XController::~XController()
{
	Destroy();
}


bool XController::Create()
{
	m_bCreated = true;

	return true;
}


void XController::Destroy()
{
	if ( m_bCreated == false)
		return;

	m_bCreated = false;
}

void XController::OnUpdate( float fDelta)
{
	PFC_THISFUNC;

	PFI_BLOCK_THISFUNC( 2);
}


void XController::OnActive( bool bActive)
{
	ClearKeyBuffer( true);

	if ( bActive == true)
		m_bActiveJustNow = true;
}

bool XController::IsVirtualKeyPressed( XVirtualKey nVKey)
{
	// 키 사용 가능 확인
	if ( CheckEnableKey( nVKey) == false)
		return false;

	return XGameInput::IsVirtualKeyPressed( nVKey);
}


bool XController::IsVirtualKeyPressedAsync( XVirtualKey nVKey)
{
	// 키 사용 가능 확인
	if ( CheckEnableKey( nVKey) == false)
		return false;

	return XGameInput::IsVirtualKeyPressedAsync( nVKey);
}