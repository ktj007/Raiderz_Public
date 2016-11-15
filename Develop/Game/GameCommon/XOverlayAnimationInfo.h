#pragma once

#include "MTstring.h"

struct OVERLAY_ANIMATION_INFO
{
	tstring			strOverlayAnimation;
	tstring			strBoneName;
};

struct OVERLAY_ANIMATION_LIST
{
	tstring			strBaseAnimation;

	vector<OVERLAY_ANIMATION_INFO>	vecOverlayAnimationList;
};

class XOverlayAnimationInfo
{
protected:
	map<tstring, OVERLAY_ANIMATION_LIST>		m_mapOverlayAnimationList;

public:
	XOverlayAnimationInfo();
	virtual ~XOverlayAnimationInfo();

	void						Set(tstring& source, OVERLAY_ANIMATION_LIST& overlayList);

	OVERLAY_ANIMATION_LIST *	Get(tstring strBaseAni);
};