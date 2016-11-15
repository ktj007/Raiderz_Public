#include "stdafx.h"
#include "XObjectCollisionDebug.h"

//#ifdef _DEBUG
#include "RDebugPassRenderer.h"
#include "RCameraSceneNode.h"
//#endif

//////////////////////////////////////////////////////////////////////////

XObjectCollisionDebug::XObjectCollisionDebug()
: m_bCollisionInvestigate(false)
, m_bRenderCollisionInvestigate(false)
, m_nRenderCollisionInvestigateIndex(0)
, m_pOwner(NULL)
{

}

XObjectCollisionDebug::~XObjectCollisionDebug()
{
	m_vecObjectCollisionInvestigate.clear();
}

void XObjectCollisionDebug::Init( XObject* pOwner )
{
	m_pOwner = pOwner;
}

void XObjectCollisionDebug::Render()
{
	if(m_pOwner == NULL || 
		m_pOwner->GetUID() != XGetMyUID())
		return;

	if(m_bRenderCollisionInvestigate)
	{
		int nCount = m_vecObjectCollisionInvestigate.size();
		if(m_bRenderCollisionInvestigate == false ||
			m_nRenderCollisionInvestigateIndex >= nCount ||
			m_nRenderCollisionInvestigateIndex < 0)
			return;

		RRenderHelper::RenderCapsule(m_vecObjectCollisionInvestigate[m_nRenderCollisionInvestigateIndex].capStart, 0x660000ff);
		RRenderHelper::RenderCapsule(m_vecObjectCollisionInvestigate[m_nRenderCollisionInvestigateIndex].capEnd, 0x6600ff00);

		if(m_vecObjectCollisionInvestigate[m_nRenderCollisionInvestigateIndex].capColCapsule.radius != 0)
			RRenderHelper::RenderCapsule(m_vecObjectCollisionInvestigate[m_nRenderCollisionInvestigateIndex].capColCapsule, 0x6600ff00);
		if(m_vecObjectCollisionInvestigate[m_nRenderCollisionInvestigateIndex].capTargetColCapsule.radius != 0)
			RRenderHelper::RenderCapsule(m_vecObjectCollisionInvestigate[m_nRenderCollisionInvestigateIndex].capTargetColCapsule, 0x66ffff00);

		RMatrix mat;
		mat.MakeIdentity();
		mat.SetTranslation(m_vecObjectCollisionInvestigate[m_nRenderCollisionInvestigateIndex].vNear);
		RRenderHelper::RenderBox(mat, 5, 0xff0000ff);
		mat.SetTranslation(m_vecObjectCollisionInvestigate[m_nRenderCollisionInvestigateIndex].vNear2);
		RRenderHelper::RenderBox(mat, 5, 0xff00ff00);
		mat.SetTranslation(m_vecObjectCollisionInvestigate[m_nRenderCollisionInvestigateIndex].vColPos);
		RRenderHelper::RenderBox(mat, 5, 0xffff0000);
		mat.SetTranslation(m_vecObjectCollisionInvestigate[m_nRenderCollisionInvestigateIndex].vResultPos);
		RRenderHelper::RenderBox(mat, 5, 0xffffffff);
	}
}

void XObjectCollisionDebug::SetCollisionInvestigateData( MCapsule colCapsule, MCapsule colTargetCapsule, vec3 vNear1, vec3 vNear2, vec3 vResultPos, float fDist )
{
	if(m_bCollisionInvestigate == false)
		return;

	m_TempData.capColCapsule = colCapsule;
	m_TempData.capTargetColCapsule = colTargetCapsule;
	m_TempData.vNear = vNear1;
	m_TempData.vNear2 = vNear2;
	m_TempData.vColPos = vResultPos;
	m_TempData.fDist = fDist;
}

void XObjectCollisionDebug::SetCollisionInvestigateData( stObjectCollisionInvestigateData& setData )
{
	if(m_bCollisionInvestigate == false)
		return;

	m_TempData = setData;
}

void XObjectCollisionDebug::Show( bool bInvestigate, int nIndex )
{
	if(m_pOwner == NULL || 
		m_pOwner->GetUID() != XGetMyUID())
		return;

	ShowCollisionInvestigate(bInvestigate);

	m_nRenderCollisionInvestigateIndex = nIndex;

	m_TempData.Init();
}

void XObjectCollisionDebug::ShowCollisionInvestigate( bool bShow )
{
	if(bShow == m_bCollisionInvestigate)
		return;

	m_bRenderCollisionInvestigate = false;

	if(bShow)
		m_vecObjectCollisionInvestigate.clear();
	if(bShow == false)
		m_bRenderCollisionInvestigate = true;

	m_bCollisionInvestigate = bShow;
}

void XObjectCollisionDebug::AddData()
{
	if(m_bCollisionInvestigate == false)
		return;

	m_vecObjectCollisionInvestigate.push_back(m_TempData);
}

void XObjectCollisionDebug::SetCollisionInvestigateData_ResultPos( vec3& vStart, vec3& vTo, vec3& vImpactDir, float fImpactDist )
{
	if(m_bCollisionInvestigate == false)
		return;

	vec3 vTestDir = vImpactDir;
	vTestDir.z = 0.0f;
	vTestDir.Normalize();
	vec3 vMoveDir = vImpactDir * fImpactDist;

	m_TempData.vResultPos = vStart + vTo + vMoveDir;
}
