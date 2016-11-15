#pragma once

#include <tchar.h>
#include "MTstring.h"

struct ReAction_DATA 
{
	tstring			strReActionName;
	tstring			strWeaponBoneName;
	float			fWeight;
	float			fSpeed;
	bool			bDisable;

	ReAction_DATA()
	{
		init();
	}

	void init()
	{
		//strReActionName = T"";
		fWeight		= 0.3f;
		fSpeed		= 1.0f;
		bDisable	= false;
	}

	void Set(ReAction_DATA& copyData)
	{
		strReActionName		= copyData.strReActionName;
		strWeaponBoneName	= copyData.strWeaponBoneName;
		fWeight				= copyData.fWeight;
		fSpeed				= copyData.fSpeed;
		bDisable			= copyData.bDisable;
	}
};


class XReActionAnimationInfo
{
protected:
	map<tstring, ReAction_DATA>		m_mapReActionAnimationList;

public:
	XReActionAnimationInfo();
	virtual ~XReActionAnimationInfo();

	void						Set(tstring& source, ReAction_DATA& reAction_Data);

	ReAction_DATA *				Get(tstring strBaseAni);
};