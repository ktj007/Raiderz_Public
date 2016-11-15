#include "stdafx.h"
#include "XBaseResourceGrabber.h"
#include "XResourceRefMgr.h"

void XBaseAlwaysResourceGrabber::Grab()
{
}

void XBaseAlwaysResourceGrabber::ReserveAddRef( const TCHAR* szResName )
{
	m_pResourceRefMgr->ReserveAddRef(szResName);
}

XBaseAlwaysResourceGrabber::XBaseAlwaysResourceGrabber()
{
	m_pResourceRefMgr = new XResourceRefMgr();
}

XBaseAlwaysResourceGrabber::~XBaseAlwaysResourceGrabber()
{
	SAFE_DELETE(m_pResourceRefMgr);
}

void XBaseAlwaysResourceGrabber::Create()
{
	m_pResourceRefMgr->Create();
}

void XBaseAlwaysResourceGrabber::Destroy( RDevice* pDevice )
{
	m_pResourceRefMgr->Destroy(pDevice);
}

void XBaseAlwaysResourceGrabber::OnCreateResource( RResource* pResource )
{
	m_pResourceRefMgr->OnCreateResource(pResource);
}