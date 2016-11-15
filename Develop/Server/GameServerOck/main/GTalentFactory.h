#ifndef _GTALENT_FACTORY_H
#define _GTALENT_FACTORY_H

#include "GTalentDef.h"

class GEntityActor;
class GTalentInfo;
class GTalent;

using namespace cscommon_server;


class GTalentFactory : public MTestMemPool<GTalentFactory>
{
private:
	GTalent* NewExtraActiveTalent(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress=true) const;
public:
	virtual GTalent* NewTalent(GEntityActor* pOwner, GTalentInfo* pTalentInfo, TALENT_TARGET_INFO Target, bool bGainStress=true) const;
};

#endif