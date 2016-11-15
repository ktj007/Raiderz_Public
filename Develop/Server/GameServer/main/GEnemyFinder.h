#pragma once

#include "CSChrInfo.h"

class GEntityNPC;
class GSector;
class GField;
class GEntity;


TEST_FORWARD_FT(EnemyFinder, Fixture, CheckHeight);

class GEnemyFinder
{
	TEST_FRIEND_FT(EnemyFinder, Fixture, CheckHeight);
private:
	MRegulator	m_rgrCheckInvisibility;
	bool		m_bCheckInvisibility;
private:
	
	MUID _Find(GEntityNPC* pOwnerNPC, GField* pField, const ENTITY_TYPEID nEnemyEntityTypeID);
	
	MUID PickupEnemy(GEntityNPC* pOwnerNPC, GField* pField, vector<MUID> vecEntities, const ENTITY_TYPEID nEnemyEntityTypeID );
	bool CheckSight(GEntityNPC* pOwnerNPC, GEntity* pEnemyEntity);	
	bool CheckFaction(GEntityNPC* pOwnerNPC, GEntity* pEnemyEntity);
	bool CheckPicking(GEntityNPC* pOwnerNPC, GEntity* pEnemyEntity);
	bool CheckHeight(GEntityNPC* pOwnerNPC, GEntity* pEnemyEntity, GField* pField);

public:
	GEnemyFinder();

	MUID Find(GEntityNPC* pOwnerNPC);	

	void Update(float fDelta);
};
