#include "StdAfx.h"
#include "RSPTManager.h"

namespace rs3
{

RSPTManager::RSPTManager( void )
{
	ClearCullingInfo();
}

void RSPTManager::SetCullingInfo(RCullingMgr *pCullingMgr,RRENDERPASS begin,RRENDERPASS end )
{
	m_pCullingMgr = pCullingMgr;
	m_BeginPass = begin;
	m_EndPass = end;
}

void RSPTManager::ClearCullingInfo( void )
{
	m_pCullingMgr = 0;
	m_BeginPass = m_EndPass = RRP_NONE;
}



}

