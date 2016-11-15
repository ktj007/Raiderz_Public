#include "stdafx.h"
#include "XModelAnimationInfo.h"

XModelAnimationInfo::XModelAnimationInfo()
{

}

XModelAnimationInfo::~XModelAnimationInfo()
{

}

XAnimationInfoMgr * XModelAnimationInfo::GetAnimationInfoMgr( const wstring& strMeshName )
{
	XAnimationInfoMgr* pInfoMgr = FindAnimationInfoMgr(strMeshName);
	
	// 없으면 만든다.
	if(pInfoMgr == NULL)
	{
		XAnimationInfoMgr infoMgr;
		m_mapAnimationInfoMgr.insert(map<wstring, XAnimationInfoMgr>::value_type(strMeshName, infoMgr));

		pInfoMgr = FindAnimationInfoMgr(strMeshName);
	}

	return pInfoMgr;
}

XAnimationInfoMgr * XModelAnimationInfo::FindAnimationInfoMgr( const wstring& strMeshName )
{
	map<wstring, XAnimationInfoMgr>::iterator it = m_mapAnimationInfoMgr.find(strMeshName);
	if(it != m_mapAnimationInfoMgr.end())
	{
		return &(it->second);
	}

	return NULL;
}