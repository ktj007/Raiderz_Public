#pragma once

class GEntityActor;
class GEntityPlayer;
class GEntityNPC;

enum RELATION_TYPE
{
	RT_NONE = 0,
	RT_ALL,
	RT_ENEMY,
	RT_ALLY,	
	RT_PARTY,
	RT_ALLYDEAD,

	RT_MAX
};


class GRelationSelector
{
private:
	RELATION_TYPE ReqPlayerTarPlayer(GEntityPlayer* pReqPlayer, GEntityPlayer* pTarPlayer);
	RELATION_TYPE ReqPlayerTarNPC(GEntityPlayer* pReqPlayer, GEntityNPC* pTarNPC);	
	RELATION_TYPE ReqNPCTarPlayer(GEntityNPC* pReqNPC, GEntityPlayer* pTarPlayer);
	RELATION_TYPE ReqNPCTarNPC(GEntityNPC* pReqNPC, GEntityNPC* pTarNPC);

public:
	RELATION_TYPE Select(GEntityActor* pReqActor, GEntityActor* pTarActor);	
};
