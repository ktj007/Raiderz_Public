#ifndef _TREACTION_ANIMATION_TABLE_H
#define _TREACTION_ANIMATION_TABLE_H

#pragma once

#include "XReActionAnimationInfo.h"

class TReActionAnimationInfo : public XReActionAnimationInfo
{
public:
	TReActionAnimationInfo();
	virtual ~TReActionAnimationInfo();

	void			DelReActionAnimationInfo(string strBaseAni);
};

#endif