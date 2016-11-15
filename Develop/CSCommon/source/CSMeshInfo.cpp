#include "stdafx.h"
#include "CSMeshInfo.h"
#include "MXml.h"
#include "CSStrings.h"

//////////////////////////////////////////////////////////////////////////
// 판정
void CSHitCapsule::SetActiveTime( float fBeginTime, float fEndTime )
{
	if (fBeginTime > 0.001f || fEndTime > 0.001f)
	{
		m_bAlwaysActive = false;
	}
	else
	{
		m_bAlwaysActive = true;
	}

	m_fActiveTimeBegin = fBeginTime;
	m_fActiveTimeEnd = fEndTime;
}

bool CSHitCapsule::CheckActiveTime( float fTime ) const
{
	if (m_bAlwaysActive == true) return true;

	if (fTime < m_fActiveTimeBegin || fTime > m_fActiveTimeEnd) return false;

	return true;
}

CSHitCapsule * CSHitGroup::GetHitCapsuleByCount( int nIndex )
{
	int nCount = 0;
	vector<CSHitCapsule>::iterator itCapsule = m_vecHitCapsules.begin();
	while(itCapsule != m_vecHitCapsules.end())
	{
		if(nCount == nIndex)
		{
			return &(*itCapsule);
		}
		nCount++;
		itCapsule++;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 충돌

//////////////////////////////////////////////////////////////////////////
// 메쉬정보
int CSMeshInfo::FindHitEffectCapsuleIndex( int nGroupID, int nCapsuleID )
{
	int nGroupCount = 0;

	if(nGroupID == -1 ||
		nCapsuleID == -1)
		return -1;

	map<int, CSHitGroup>::iterator itFind = m_mapHitGroupInfo.find(nGroupID);
	if(itFind != m_mapHitGroupInfo.end())
	{
		if((int)itFind->second.m_vecHitCapsules.size() > nCapsuleID)
		{
			return itFind->second.m_vecHitCapsules[nCapsuleID].m_nHitEffectCapsuleID;
		}
	}

	return -1;
}

int CSMeshInfo::FindPartsHitCapsuleIndex( int nGroupID, int nIndex )
{
	map<int, CSHitGroup>::iterator itFind = m_mapHitGroupInfo.find(nGroupID);
	if(itFind != m_mapHitGroupInfo.end())
	{
		int nCpasuleCount	= 0;

		vector<CSHitCapsule>::iterator itCapsule = itFind->second.m_vecHitCapsules.begin();
		while(itCapsule != itFind->second.m_vecHitCapsules.end())
		{
			if((*itCapsule).m_nHitEffectCapsuleID == nIndex)
			{
				return nCpasuleCount;
			}

			itCapsule++;
			nCpasuleCount++;
		}
	}

	return -1;
}

CSHitGroup * CSMeshInfo::FineHitGroup( int nGroupID )
{
	map<int, CSHitGroup>::iterator itFind = m_mapHitGroupInfo.find(nGroupID);
	if(itFind != m_mapHitGroupInfo.end())
	{
		return &(itFind->second);
	}

	return NULL;
}

CSAniInfo* CSMeshInfo::FindAniInfo( const tstring& aniName )
{
	for(vector<CSAniInfo>::iterator it = m_vecAniInfo.begin(); it != m_vecAniInfo.end(); it++)
	{
		if(aniName == (*it).m_strAnimationName)
		{
			return &(*it);
		}
	}

	return NULL;
}
//
//bool CSMeshInfo::IsExistNotReActionAnimation( const tstring& aniName )
//{
//	set<tstring>::iterator itor = m_setNotReActionAnimationList.find(aniName);
//	if (itor != m_setNotReActionAnimationList.end()) return true;
//
//	return false;
//}

CSMeshInfo* CSMeshInfoMgr::GetInfo( const tstring& key )
{
	tstring strLowerKey = CSStrings::StringToLower(key);

	map<tstring, CSMeshInfo>::iterator i = m_mapMeshInfo.find(strLowerKey);

	if (i == m_mapMeshInfo.end())
		return NULL;
	else
	{
		return &((*i).second);
	}
}