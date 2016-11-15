#include "stdafx.h"
#include "XPEEdgeMgr.h"
#include "XCharacter.h"

XPEEdgeMgr::XPEEdgeMgr()
{

}

XPEEdgeMgr::~XPEEdgeMgr()
{

}

void XPEEdgeMgr::EnablePEEdge( MUID& uid, const RVector4& _color, float _fTickNess, float _fBlurFactor, bool _bProjection )
{
	// Diable에 등록되어있는가?
	CheckDiablePEEdge(uid);

	// 중복검사
	if(CheckDuplicationEnablePEEdeg(uid))
		return;

	stPEEdgeInfo info;
	info.uid			= uid;
	info.vColor			= _color;
	info.fTickNess		= _fTickNess;
	info.fBlurFactor	= _fBlurFactor;
	info.bProjection	= _bProjection;

	m_vecEnableList.push_back(info);
}

void XPEEdgeMgr::DiablePEEdge( MUID& uid )
{
	// 중복검사
	if(CheckDuplicationDiablePEEdeg(uid))
		return;

	m_vecDisableList.push_back(uid);
}

void XPEEdgeMgr::Update()
{
	// 비활성화부터 한다.
	for(vector<MUID>::iterator itDiable = m_vecDisableList.begin(); itDiable != m_vecDisableList.end(); ++itDiable)
	{
		XObject* pObject = gg.omgr->FindObject(*itDiable);
		if(pObject == NULL || pObject->GetActor() == NULL)
			continue;

		if (pObject->GetActor()->IsPEEdge())
		{
			pObject->GetActor()->DisablePEEdge();
		}
	}

	// 활성화한다.
	for(vector<stPEEdgeInfo>::iterator itEnable = m_vecEnableList.begin(); itEnable != m_vecEnableList.end(); ++itEnable)
	{
		XObject* pObject = gg.omgr->FindObject(itEnable->uid);
		if(pObject == NULL || pObject->GetActor() == NULL)
			continue;

		if (pObject->GetActor()->IsPEEdge() == false)
		{
			pObject->GetActor()->EnablePEEdge( RVector3(itEnable->vColor.x, itEnable->vColor.y, itEnable->vColor.z) );
			//pObject->GetActor()->EnablePEEdge( itEnable->vColor, itEnable->fTickNess, itEnable->fBlurFactor, itEnable->bProjection);
		}
	}

	m_vecDisableList.clear();
	m_vecEnableList.clear();
}

void XPEEdgeMgr::CheckDiablePEEdge( MUID& uid )
{
	// 비활성화에 등록되어있으면 삭제한다.
	vector<MUID>::iterator itDiable = m_vecDisableList.begin();
	while(itDiable != m_vecDisableList.end())
	{
		if(*itDiable == uid)
		{
			itDiable = m_vecDisableList.erase(itDiable);
			continue;
		}

		++itDiable;
	}
}

bool XPEEdgeMgr::CheckDuplicationEnablePEEdeg( MUID& uid )
{
	
	for(vector<stPEEdgeInfo>::iterator itEnable = m_vecEnableList.begin(); itEnable != m_vecEnableList.end(); ++itEnable)
	{
		if(itEnable->uid == uid)
		{
			return true;
		}
	}

	return false;
}

bool XPEEdgeMgr::CheckDuplicationDiablePEEdeg( MUID& uid )
{
	for(vector<MUID>::iterator itDiable = m_vecDisableList.begin(); itDiable != m_vecDisableList.end(); ++itDiable)
	{
		if(*itDiable == uid)
		{
			return true;
		}
	}

	return false;
}
