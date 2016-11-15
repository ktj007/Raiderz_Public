#ifndef _TOVERLAY_ANIMATION_TABLE_H
#define _TOVERLAY_ANIMATION_TABLE_H

#pragma once

#include "XOverlayAnimationInfo.h"

class TOverlayAnimationInfo : public XOverlayAnimationInfo
{
public:
	TOverlayAnimationInfo();
	virtual ~TOverlayAnimationInfo();

	void						InsertData(string strAni);
	void						EraseData(string strAni);
};

#endif