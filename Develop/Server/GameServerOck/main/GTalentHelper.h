#pragma once

#include "GTalentInfo.h"

class GEntity;
class GTalent;

class GTalentHelper
{
public:
	static GTalentInfo* FindTalentInfo(int nTalentID, GEntity* pEntity=NULL);
	static void			AddHitterForTargetAndNonProjTalent(GTalent* pTalent);
};