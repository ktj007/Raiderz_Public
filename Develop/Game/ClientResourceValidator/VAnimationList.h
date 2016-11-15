#pragma once

#include "VAnimationInfoLoader.h"

#define ANIMATION_LIST_EXT								_T(".animation.xml")

class VAnimationList
{
public:
	string						m_strFileName;

	vector<ANIMATION_INFO>		m_vecAnimationList;

public:
	VAnimationList();
	virtual ~VAnimationList();

	bool						LoadInfo(string& strMeshFileName);

	ANIMATION_INFO*				GetAnimationInfo(string& strAniName);
};

