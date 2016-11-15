#pragma once

#include "VAnimationInfoLoader.h"

#define ANIMATION_EVENT_INFO_EXT						".animationevent.xml"

class VAnimationEventInfo
{
public:
	string							m_strFileName;

	vector<ANIMATION_EVENT_INFO>	m_vecAnimationEvents;

public:
	VAnimationEventInfo();
	virtual ~VAnimationEventInfo();

	bool							LoadInfo(string& strMeshFileName);
	void							DeleteInfo();
};

