#include "stdafx.h"
#include "XInteractionHelper.h"
#include "XModuleCollision.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XGame.h"
#include "XCharacter.h"
#include "RMesh.h"
#include "RSceneManager.h"

rs3::RBoundingBox XInteractionHelper::GetObjectAABB( XObject* pObject )
{
	RBoundingBox	ObjAABB;
	ObjAABB.Initialize();

	float fRadius = 70.f;

	if(pObject->GetEntityType() == ETID_PLAYER)
	{
		ObjAABB.vmax =  RVector(fRadius, fRadius, 0.f) + pObject->GetPosition();
		ObjAABB.vmin = -RVector(fRadius, fRadius, 0.f) + pObject->GetPosition();
	}
	else if(pObject->GetType() == XOBJ_BPART_NPC)
	{
		ObjAABB = pObject->GetActor()->GetAABB();
	}
	else
	{
		ObjAABB = pObject->GetActor()->GetInteractionAABB();
		vec3 vCab = ObjAABB.vmax - ObjAABB.vmin;
		if (vCab.y < fRadius)
		{
			ObjAABB.vmax =  RVector(fRadius, fRadius, 0.f) + pObject->GetPosition();
			ObjAABB.vmin = -RVector(fRadius, fRadius, 0.f) + pObject->GetPosition();
		}
	}

	return ObjAABB;
}

vec3 XInteractionHelper::GetInteractionCenter( XObject* pObject, RBoundingBox& rbb )
{
	vec3 vPos = rbb.GetCenter();
	RActorNode* pInteractionCenterBoneNode = pObject->GetActor()->GetActorNodePos(INTERACTION_CENTER_BONE_NAME, vPos, E_TS_WORLD);

	// 본이 없으면 왜 위치점에서 했는지 잘 모르겠습니다.
	// BPart NPC는 z값만 수정하도록 하겠습니다.
	if (pInteractionCenterBoneNode || pObject->GetType() == XOBJ_BPART_NPC)
	{
		vPos.z = pObject->GetPosition().z;	
	}
	else
	{
		vPos = pObject->GetPosition();
	}

	return vPos;
}

float XInteractionHelper::GetInteractionSize( RBoundingBox& rbb )
{
	float fX = (vec3(rbb.maxx, 0, 0) - vec3(rbb.minx, 0, 0)).Length();
	float fY = (vec3(0, rbb.maxy, 0) - vec3(0, rbb.miny, 0)).Length();

	float fSize = max(fX, fY) / 2.0f;

	if (fSize < MINSIZE_INTERACTION_CIRCLE)
		fSize = MINSIZE_INTERACTION_CIRCLE;

	if (fSize > MAXSIZE_INTERACTION_CIRCLE)
		fSize = MAXSIZE_INTERACTION_CIRCLE;	//너무 커지는 거 방지.

	return fSize;
}

bool XInteractionHelper::IsLootableDistance( vec3 vMyPos, XObject* pTarget )
{
	if (pTarget == NULL) return false;

	const float CIRCLE_RATIO = 0.85f;		// 데칼 이펙트 크기에 따라서 달라질 수 있습니다.
	const float LOOTABLE_TOLER = 90.0f;

	RBoundingBox rbb = XInteractionHelper::GetObjectAABB(pTarget);
	vec3 vCenter = XInteractionHelper::GetInteractionCenter(pTarget, rbb);
	float fLootCircleRadius = XInteractionHelper::GetInteractionSize(rbb) * CIRCLE_RATIO;
	float fMyDist = vMyPos.DistanceTo(vCenter);

	if (fMyDist < fLootCircleRadius + LOOTABLE_TOLER) return true;

	return false;
}

float XInteractionHelper::GetInteractionDistance( XObject* pObject )
{
	float fRadius = pObject->GetRadius();

	return CHECK_INTERACTION_DISTANCE + fRadius * 1.5f;
}

bool XInteractionHelper::IsInteractableNPC( XNonPlayer* pNPC )
{
	if (pNPC && pNPC->GetInfo())
	{
		if ((pNPC->GetInfo()->bInteract == true)			&&
			(pNPC->GetIConState() != NIS_NONE)				&&
			(pNPC->GetIConState() != NIS_ITEM_UNUSABLE)		&&
			(pNPC->GetIConState() != NIS_QUEST_NOT_BEGIN_ONLYICON)	&&
			(pNPC->GetIConState() != NIS_QUEST_CONTINUE_ONLYICON)	&&
			((pNPC->IsAttackable() == false) ||
			(pNPC->IsAttackable() == true && pNPC->IsDead())))
		{
			return true;
		}
	}

	return false;
}

bool XInteractionHelper::IsInteractablePC( XPlayer* pPC )
{
	if (pPC								&&
		pPC->IsDead() == false			&&
		pPC->IsCutScene() == false		&&
		pPC->IsAlly(gvar.Game.pMyPlayer))
	{
		return true;
	}

	return false;
}

NPC_ICON_STATE XInteractionHelper::ConvertNPCIConTypeToIconState( NPC_ICON_TYPE NIT)
{
	NPC_ICON_STATE NIS = NIS_NONE;
	switch (NIT)
	{
	case NIT_NONE:				NIS = NIS_NONE;				break;
	case NIT_ITEM_UNUSABLE:		NIS = NIS_ITEM_UNUSABLE;	break;
	case NIT_ITEM_USABLE:		NIS = NIS_ITEM_USABLE;		break;
	case NIT_ONLYINTERACTION:	NIS = NIS_ONLY_INTERACTION;	break;
	case NIT_TRIGGER:			NIS = NIS_TRIGGER;			break;
	case NIT_QUEST_INTERACT:	NIS = NIS_QUEST_INTERACT;	break;
	case NIT_LOOT:				NIS = NIS_LOOT;				break;
	case NIT_TALK:				NIS = NIS_TALK;				break;
	case NIT_HELPER:			NIS = NIS_HELPER;			break;
	case NIT_SHOP:				NIS = NIS_SHOP;				break;
	case NIT_GUILD_CREATE:		NIS = NIS_GUILD_CREATE;		break;
	case NIT_CRAFT:				NIS = NIS_CRAFT;			break;
	case NIT_QUEST_NOT_BEGIN_ONLYICON:	NIS = NIS_QUEST_NOT_BEGIN_ONLYICON;	break;
	case NIT_QUEST_NOT_BEGIN:			NIS = NIS_QUEST_NOT_BEGIN;	break;
	case NIT_QUEST_CONTINUE_ONLYICON:	NIS = NIS_QUEST_CONTINUE_ONLYICON;	break;
	case NIT_QUEST_CONTINUE:			NIS = NIS_QUEST_CONTINUE;	break;
	case NIT_QUEST_BEGIN:		NIS = NIS_QUEST_BEGIN;		break;
	case NIT_QUEST_END:			NIS = NIS_QUEST_END;		break;
	case NIT_COMBAT:			NIS = NIS_COMBAT;			break;
	default:					_ASSERT(0);					break;
	}

	return NIS;
}
