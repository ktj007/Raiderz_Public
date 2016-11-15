#include "StdAfx.h"
#include "GInteractionSystem.h"
#include "GNPCInteractor.h"
#include "GPlayerInteractor.h"
#include "GEntityActor.h"
#include "GEntityPlayer.h"
#include "GEntityNPC.h"
#include "CSDef.h"

GInteractionSystem::GInteractionSystem()
{	
	m_pNPCInteractor = new GNPCInteractor();
	m_pPlayerInteractor = new GPlayerInteractor;	
}

GInteractionSystem::~GInteractionSystem()
{	
	SAFE_DELETE(m_pNPCInteractor);
	SAFE_DELETE(m_pPlayerInteractor);
}

bool GInteractionSystem::Interaction(GEntityPlayer* pReqPlayer, const MUID& nTarUID)
{
	if (NULL == pReqPlayer) return false;

	GEntityActor* pTarActor = pReqPlayer->FindActor(nTarUID);
	if (NULL == pTarActor) return false;

	switch (pTarActor->GetTypeID())
	{
	case ETID_PLAYER:
		{
			GEntityPlayer* pTarPlayer = ToEntityPlayer(pTarActor);
			if (false == m_pPlayerInteractor->Interaction(pReqPlayer, pTarPlayer)) return false;
		}
		break;
	case ETID_NPC:
		{
			GEntityNPC* pTarNPC = ToEntityNPC(pTarActor);
			if (false == m_pNPCInteractor->InteractionByClick(pReqPlayer, pTarNPC))
			{
				GetNPCInteractor().End(pReqPlayer);
				return false;
			}
		}
		break;
	default:
		{
			return false;
		}
	}

	pReqPlayer->OnInteraction();

	return true;
}

bool GInteractionSystem::IsInteractableDistance(GEntityActor* pActor1, GEntityActor* pActor2)
{
	if (NULL == pActor1) return false;
	if (NULL == pActor2) return false;

	if (pActor1->GetField() != pActor2->GetField()) return false;
	
	float fXYDistance = pActor1->GetPos().XYDistanceTo(pActor2->GetPos());
	float fXYValidDistance = GetInteractableDistance(pActor1, pActor2);

	if (-1.0f == fXYValidDistance) return false;
	if (fXYDistance > fXYValidDistance) return false;

	return true;
}

float GInteractionSystem::GetInteractableDistance( GEntityActor* pActor1, GEntityActor* pActor2 )
{
	float fZDistance = pActor1->GetPos().ZDistanceTo(pActor2->GetPos());	

	if (fZDistance > GAME_INTERACTION_DISTANCE_Z) return -1.0f;

	float fXYValidDistance = pActor2->GetValidInteractionDistance();

	const float fTolerDistance = GAME_INTERACTION_SERVER_TOLER_DISTANCE_XY;

	return fXYValidDistance + fTolerDistance;
}