#include "StdAfx.h"
#include "GGameGuard.h"
#include "GConfig.h"

//////////////////////////////////////////////////////////////////////////
//
// Callbacks
//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// GGameGuard
//
//////////////////////////////////////////////////////////////////////////

GGameGuard::GGameGuard(void)
: m_bInit(false)
, m_bEnable(true)
{
}

GGameGuard::~GGameGuard(void)
{
	Fini();
}

bool GGameGuard::Init()
{
	m_bInit = true;
	mlog("GameGuard Init success.\n");
	return true;
}

void GGameGuard::Fini()
{
	if (!m_bInit)
		return;

	m_bInit = false;
	mlog("GameGuard finalized success.\n");
}

bool GGameGuard::IsEnable() const
{
	return m_bEnable;
}

void GGameGuard::Enable( bool bEnable )
{
	m_bEnable = bEnable;
}