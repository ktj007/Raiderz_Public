#pragma once

class GEntityActor;
class GRelationSelector;
class GParty;

class GRelationChecker
{
public:
	GRelationChecker();
	~GRelationChecker();

	bool IsAll(GEntityActor* pReqActor, GEntityActor* pTarActor);	
	bool IsAlly(GEntityActor* pReqActor, GEntityActor* pTarActor);	
	bool IsEnemy(GEntityActor* pReqActor, GEntityActor* pTarActor);	
	bool IsParty(GEntityActor* pReqActor, GEntityActor* pTarActor);
	bool IsAllyDead(GEntityActor* pReqActor, GEntityActor* pTarActor);

private:
	GRelationSelector*	m_pRelationSelector;
};