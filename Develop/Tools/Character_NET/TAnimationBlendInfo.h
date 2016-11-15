#pragma once

#include "XAnimationBlendInfo.h"

struct ANIMATION_BLEND_DATA
{
	string			strTarget;
	float			fDurationTime;
};

struct ANIMATION_BLEND_LIST
{
	string			strSource;

	vector<ANIMATION_BLEND_DATA>	vecTargetList;
};

class TAnimationBlendInfo : public XAnimationBlendInfo
{
public:
	TAnimationBlendInfo() : XAnimationBlendInfo()	{}
	virtual ~TAnimationBlendInfo()					{}

	void GetAnimationBlendList(string& strSource, ANIMATION_BLEND_LIST& pOutBlendList);
};
