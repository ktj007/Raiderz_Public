#include "stdafx.h"
#include "XOverlayAnimationInfo.h"

XOverlayAnimationInfo::XOverlayAnimationInfo()
{

}

XOverlayAnimationInfo::~XOverlayAnimationInfo()
{

}

OVERLAY_ANIMATION_LIST * XOverlayAnimationInfo::Get( tstring strBaseAni )
{
	map<tstring, OVERLAY_ANIMATION_LIST>::iterator itFind = m_mapOverlayAnimationList.find(strBaseAni);
	if(itFind != m_mapOverlayAnimationList.end())
		return &(itFind->second);

	return NULL;
}

void XOverlayAnimationInfo::Set( tstring& source, OVERLAY_ANIMATION_LIST& overlayList )
{
	m_mapOverlayAnimationList.insert(map<tstring, OVERLAY_ANIMATION_LIST>::value_type(source, overlayList));
}