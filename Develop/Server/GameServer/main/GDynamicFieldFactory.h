#pragma once

class GDynamicFieldMaster;
class GEntityPlayer;
enum DYNAMIC_FIELD_TYPE;

class GDynamicFieldFactory
{
private:
	GDynamicFieldMaster*	CreateInnRoom(GEntityPlayer* pPlayer);
	GDynamicFieldMaster*	CreateBattleArena(GEntityPlayer* pPlayer, int nFieldGroupID);
	GDynamicFieldMaster*	CreateTrialField(GEntityPlayer* pPlayer, int nFieldGroupID);

public:
	GDynamicFieldMaster*	CreateDynamicField(DYNAMIC_FIELD_TYPE eType, GEntityPlayer* pPlayer, int nFieldGroupID = 0);

};
