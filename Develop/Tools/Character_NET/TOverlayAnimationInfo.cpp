#include "stdafx.h"
#include "TOverlayAnimationInfo.h"
#include "MXml.h"

TOverlayAnimationInfo::TOverlayAnimationInfo()
: XOverlayAnimationInfo()
{

}

TOverlayAnimationInfo::~TOverlayAnimationInfo()
{

}

void TOverlayAnimationInfo::InsertData( string strAni )
{
	OVERLAY_ANIMATION_LIST * pList = Get(strAni);
	if(pList)
		return;

	OVERLAY_ANIMATION_LIST aniList;
	m_mapOverlayAnimationList.insert(map<string, OVERLAY_ANIMATION_LIST>::value_type(strAni, aniList));
}

void TOverlayAnimationInfo::EraseData( string strAni )
{
	map<tstring, OVERLAY_ANIMATION_LIST>::iterator itFind = m_mapOverlayAnimationList.find(strAni);
	if(itFind != m_mapOverlayAnimationList.end())
	{
		m_mapOverlayAnimationList.erase(itFind);
	}
}
