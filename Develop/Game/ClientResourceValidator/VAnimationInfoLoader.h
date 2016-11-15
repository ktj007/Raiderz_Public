#pragma once

#include "RAnimation.h"

struct  ANIMATION_INFO
{
	string				strAnimationName;
	string				strAnimationFileFullName;
	float				fAnimationSpeed;
	AnimationLoopType	loopType;

	ANIMATION_INFO()
	{
		fAnimationSpeed = 1.0;

		loopType		= RAniLoopType_Loop;
	}
};

struct ANIMATION_EVENT_INFO
{
	string				strAnimationName;
	vector<RAnimationEvent*>	vecAnimationEvent;

	void DeleteInfo()
	{
		for(vector<RAnimationEvent*>::iterator it = vecAnimationEvent.begin(); it != vecAnimationEvent.end(); ++it)
		{
			SAFE_DELETE(*it);
		}
	}
};

class VAnimationInfoLoader
{
public:
	VAnimationInfoLoader();
	virtual ~VAnimationInfoLoader();

	static bool				GetAnimationInfoInXml(vector<ANIMATION_INFO>& vecOutData, string strAnimationXMLFileName, string* strFilter);
	static bool				GetAnimationEventInfoInXml(vector<ANIMATION_EVENT_INFO>& vecOutData, string strAnimationXMLFileName);

};