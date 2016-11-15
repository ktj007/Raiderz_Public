#include "StdAfx.h"
#include "GRelationChecker.h"
#include "GRelationSelector.h"
#include "GDef.h"
#include "GEntityActor.h"
#include "GEntityNPC.h"
#include "GDuel.h"
#include "GEntityPlayer.h"


GRelationChecker::GRelationChecker()
{
	m_pRelationSelector = new GRelationSelector;
}

GRelationChecker::~GRelationChecker()
{
	SAFE_DELETE(m_pRelationSelector);
}

bool GRelationChecker::IsAll(GEntityActor* pReqActor, GEntityActor* pTarActor)
{
	VALID_RET(pReqActor, false);
	VALID_RET(pTarActor, false);	

	RELATION_TYPE nRelation = m_pRelationSelector->Select(pReqActor, pTarActor);

	return (RT_ALL == nRelation ||
			RT_ENEMY == nRelation ||
			RT_ALLY == nRelation ||
			RT_ALLYDEAD == nRelation ||
			RT_PARTY == nRelation);
}

bool GRelationChecker::IsAlly(GEntityActor* pReqActor, GEntityActor* pTarActor)
{
	VALID_RET(pReqActor, false);
	VALID_RET(pTarActor, false);
	if (pReqActor == pTarActor) return true;

	RELATION_TYPE nRelation = m_pRelationSelector->Select(pReqActor, pTarActor);

	return (RT_ALLY == nRelation ||
			RT_ALLYDEAD == nRelation ||
			RT_PARTY == nRelation);
}

bool GRelationChecker::IsEnemy(GEntityActor* pReqActor, GEntityActor* pTarActor)
{
	VALID_RET(pReqActor, false);
	VALID_RET(pTarActor, false);
	if (pReqActor == pTarActor) return false;

	return (RT_ENEMY == m_pRelationSelector->Select(pReqActor, pTarActor));	
}

bool GRelationChecker::IsParty(GEntityActor* pReqActor, GEntityActor* pTarActor)
{
	VALID_RET(pReqActor, false);
	VALID_RET(pTarActor, false);

	if (pReqActor == pTarActor) 
		return true;

	return (RT_PARTY == m_pRelationSelector->Select(pReqActor, pTarActor));		
}

bool GRelationChecker::IsAllyDead( GEntityActor* pReqActor, GEntityActor* pTarActor )
{
	VALID_RET(pReqActor, false);
	VALID_RET(pTarActor, false);

	return (RT_ALLYDEAD == m_pRelationSelector->Select(pReqActor, pTarActor));		
}