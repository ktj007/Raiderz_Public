#include "stdafx.h"
#include "VAnimationList.h"

VAnimationList::VAnimationList()
{

}

VAnimationList::~VAnimationList()
{

}

bool VAnimationList::LoadInfo( string& strMeshFileName )
{
	m_strFileName = strMeshFileName + ANIMATION_LIST_EXT;

	return VAnimationInfoLoader::GetAnimationInfoInXml(m_vecAnimationList, m_strFileName, NULL);
}

ANIMATION_INFO* VAnimationList::GetAnimationInfo( string& strAniName )
{
	for(vector<ANIMATION_INFO>::iterator it = m_vecAnimationList.begin(); it != m_vecAnimationList.end(); ++it)
	{
		if(it->strAnimationName == strAniName)
			return &(*it);
	}

	return NULL;
}