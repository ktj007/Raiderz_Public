#pragma once

#include "MTstring.h"

class XCharacter;

class XAnimationHitSoundInfo
{
public:
	map<tstring, tstring>			m_mapAnimationHitSoundList;

public:
	XAnimationHitSoundInfo();
	virtual ~XAnimationHitSoundInfo();

	void						Set(tstring& source, tstring& soundname);

	tstring						Get(tstring strBaseAni);
};