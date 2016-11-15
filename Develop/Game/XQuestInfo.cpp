#include "stdafx.h"
#include "XQuestInfo.h"
#include "XStrings.h"
#include "XFieldInfo.h"
#include "XGame.h"

XQObjectiveInfo::XQObjectiveInfo()
{

}

XQObjectiveInfo::~XQObjectiveInfo()
{

}

vec3 XQObjectiveInfo::GetPos( int nIndex /*= 1*/ )
{
/*
	XFieldInfo* pFieldInfo = info.field->GetSimpleFieldInfo(nFieldID);
	if (NULL == pFieldInfo) return vec3::ZERO;

	MARKER_INFO_MAP_BY_ID::iterator iter = pFieldInfo->m_mapMarkerByID.find(nMarkerID);
	if (pFieldInfo->m_mapMarkerByID.end() == iter) return vec3::ZERO;

	return iter->second.vPoint;
*/

	// 주의 : 현재 캐릭터가 있는 필드만 허용
	XFieldInfo* pFieldInfo = NULL;
	if ( gg.game  &&  gg.game->GetCurrentGameState()  &&  gg.game->GetCurrentGameState()->GetWorld())
		pFieldInfo = gg.game->GetCurrentGameState()->GetWorld()->GetInfo();

	vec3 vPos = vec3::ZERO;
	if ( pFieldInfo != NULL)
	{
		MARKER_INFO_MAP_BY_ID::iterator itr = pFieldInfo->m_mapMarkerByID.find( nIndex == 1 ? nMarkerID : nMarkerID2);
		if ( itr != pFieldInfo->m_mapMarkerByID.end())
			vPos = itr->second.vPoint;
	}
	return vPos;
}

const TCHAR* XQObjectiveInfo::GetDesc( void )
{
	return XStringResMgr::GetInstance()->GetString(strDesc);
}

XQuestInfo::XQuestInfo()
{

}

XQuestInfo::~XQuestInfo()
{
}

XQObjectiveInfo* XQuestInfo::Get(int nID)
{
	return static_cast<XQObjectiveInfo*>(__super::Get(nID));
}

vector<XQObjectiveInfo*> XQuestInfo::GetAllQObjectiveInfo()
{
	return ToXQObjectiveInfo(__super::GetAllQObjectiveInfo());
}

vector<XQObjectiveInfo*> XQuestInfo::ToXQObjectiveInfo(vector<CSQObjectiveInfo*> vecInfo)
{
	vector<XQObjectiveInfo*> vecGQObjectiveInfo;

	for each (CSQObjectiveInfo* pInfo in vecInfo)
	{
		vecGQObjectiveInfo.push_back(static_cast<XQObjectiveInfo*>(pInfo));
	}

	return vecGQObjectiveInfo;
}

bool XQuestInfo::GetIndex( size_t nIndex, XQObjectiveInfo** ppOutInfo )
{
	MAP_QOBJECTIVEINFO::iterator itr = mapQObjectiveInfo.begin();
	for ( size_t i = 0;  i < nIndex;  i++, itr++)
	{
		if ( itr == mapQObjectiveInfo.end())	return false;
	}
	*ppOutInfo = static_cast<XQObjectiveInfo*>((*itr).second);
	return true;
}

int XQuestInfo::GetObjectiveIndex( size_t nObjectiveID )
{
	int nRetIndex = 0;

	for (MAP_QOBJECTIVEINFO::iterator itor = mapQObjectiveInfo.begin(); itor != mapQObjectiveInfo.end(); ++itor)
	{
		XQObjectiveInfo* pObjectiveInfo = static_cast<XQObjectiveInfo*>((*itor).second);
		if (pObjectiveInfo->nID == nObjectiveID)
		{
			return nRetIndex;
		}

		nRetIndex++;
	}
	return -1;
}

const TCHAR* XQuestInfo::GetDesc( void )
{
	return XStringResMgr::GetInstance()->GetString(strDesc);
}

const TCHAR* XQuestInfo::GetTitle( void )
{
	return XStringResMgr::GetInstance()->GetString(strTitle);
}

const TCHAR* XQuestInfo::GetCategory( void )
{
	return XStringResMgr::GetInstance()->GetString(strCategory);
}


XQuestInfoMgr::XQuestInfoMgr(void)
{

}

XQuestInfoMgr::~XQuestInfoMgr(void)
{

}

CSQuestInfo* XQuestInfoMgr::NewQuestInfo()
{
	return new XQuestInfo();
}

CSQObjectiveInfo* XQuestInfoMgr::NewQObjectiveInfo()
{
	return new XQObjectiveInfo();
}

XQuestInfo* XQuestInfoMgr::Get(int nQuestID)
{
	return static_cast<XQuestInfo*>(__super::Get(nQuestID));
}

void XQuestInfoMgr::Cooking()
{
	for each (const MAP_QUESTINFO::value_type& data in  m_mapQuestInfo)
	{
		m_vecAllQuestID.push_back(data.first);
		m_vecAllQuestInfo.push_back(static_cast<XQuestInfo*>(data.second));
	}
}

XQuestInfo* XQuestInfoMgr::GetRandomQuest()
{
	if (m_vecAllQuestID.empty()) return NULL;

	int nIndex = XMath::RandomNumber(0, m_vecAllQuestID.size()-1);
	int nQuestID = m_vecAllQuestID[nIndex];

	return Get(nQuestID);
}

vector<XQuestInfo*> XQuestInfoMgr::GetAllQuestInfo()
{
	return m_vecAllQuestInfo;
}

