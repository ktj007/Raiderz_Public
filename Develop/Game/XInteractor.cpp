#include "StdAfx.h"
#include "XInteractor.h"
#include "XPost_Trade.h"
#include "XPost_Interaction.h"
#include "XModuleEntity.h"
#include "XModuleCollision.h"
#include "XNonPlayer.h"
#include "XCharacter.h"
#include "XInteractionHelper.h"
#include "XInteractionInfo.h"

#include "RMesh.h"
#include "RDebugPassRenderer.h"

XInteractor::XInteractor(XObject* pOwner)
{
	Init(pOwner);
}

void XInteractor::Init( XObject* pOwner )
{
	m_TargetNPCRegulator.SetTickTime(GAME_TARGET_NPC_CHECK_TICK);

	m_pOwner = pOwner;
}

void XInteractor::Update( float fDelta )
{
	if (m_pOwner == NULL) return;

	UpdateTarget(fDelta);
}

void XInteractor::UpdateTarget( float fDelta )
{
	if (!m_TargetNPCRegulator.IsReady(fDelta)) return;

	CheckInteractionEnd();

	_CheckLootingResult nLootingResult = CheckLootingInteractionObject();

	if(nLootingResult != CLR_TRUE)
	{
		if (CheckInteractionNPC() == false)
		{
			if(CheckInteractionGather() == false)
			{
				if (nLootingResult == CLR_FALSE)
				{
					CheckInteractionPC();
				}
			}
		}
	}
}

void XInteractor::CheckInteractionEnd()
{
	if (XGetInteractionInfo().InteractionTargetUID	== MUID::ZERO) return;

	XObject* pObject = gg.omgr->FindObject(XGetInteractionInfo().InteractionTargetUID);
	if (pObject == NULL) return;

	vec3 vPos = m_pOwner->GetPosition();
	vec3 vObjPos = pObject->GetPosition();

	float fOutDist = vPos.DistanceTo(vObjPos);
	if (fOutDist > CHECK_INTERACTION_DISTANCE )
	{
		mlog_duck("Interaction_End 메세지 보냄. \n");
		if (gvar.MyInfo.TradeContents.IsTrade())
		{
			XPostTrade_Cancel();
		}
		else
		{
			XPostNPCInteraction_End();
		}
	}
}

bool XInteractor::CheckInteractionPC()
{
	XObject* pTarObject = FindInteractionPC();

	if (pTarObject)
	{
		XGetInteractionInfo().SetInteractionSearchUID(pTarObject->GetUID());
		return true;
	}
	else
	{
		XGetInteractionInfo().SetInteractionSearchUID(MUID::ZERO);
		return false;
	}
}

XObject* XInteractor::FindInteractionPC()
{
	XModuleCollision* pModuleCollision = m_pOwner->GetModuleCollision();
	if( pModuleCollision == NULL) return NULL;

	vec3 vPos = vec3(0.f, 0.f, 100.f);
	vec3 vDir = m_pOwner->GetDirection();

	float fCheckX = 100.f;
	float fBottomZ = -CHECK_INTERACTION_COL_Z*0.5f;
	RBoundingBox CollisionBox;
	CollisionBox.vmax.Set( fCheckX, fCheckX, CHECK_INTERACTION_COL_Z);
	CollisionBox.vmin.Set(-fCheckX, -CHECK_INTERACTION_DISTANCE, fBottomZ);

	vector<XObject*> vecTargetList;
	pModuleCollision->GetInteractionObjectListColBox(vecTargetList, FIND_NETPLAYER, vPos, vDir, CollisionBox);

	XObject* pTarObject = NULL;
	float fCurrDist = FLT_MAX;

	for (size_t i = 0; i < vecTargetList.size(); i++)
	{
		XObject* pThisObject = vecTargetList[i];
		if (pThisObject)
		{
			if (pThisObject->GetEntityType() != ETID_PLAYER) 
			{
				_ASSERT(0);
				continue;
			}

			float fTarDistance = pThisObject->GetPosition().DistanceTo(m_pOwner->GetPosition());
			if (fCurrDist < fTarDistance) continue;

			//전투중인 상대에게 인터랙션 걸리지 않도록
			if(pThisObject->GetUID() == XGetInteractionInfo().VSPlayerUID) continue;

			XPlayer* pPlayer = static_cast<XPlayer*>(pThisObject);
			if(XInteractionHelper::IsInteractablePC(pPlayer))
			{
				pTarObject = pThisObject;
				fCurrDist = fTarDistance;
				continue;
			}
		}
	}
	return pTarObject;
}

bool XInteractor::CheckInteractionNPC()
{
	XModuleCollision* pModuleCollision = m_pOwner->GetModuleCollision();
	if( pModuleCollision == NULL) return false;

	vec3 vPos = vec3(0.f, 0.f, 100.f);
	vec3 vDir = m_pOwner->GetDirection();

	float fCheckX = 100.f;
	float fBottomZ = -CHECK_INTERACTION_COL_Z*0.5f;
	RBoundingBox CollisionBox;
	CollisionBox.vmax.Set( fCheckX, fCheckX, CHECK_INTERACTION_COL_Z);
	CollisionBox.vmin.Set(-fCheckX, -CHECK_INTERACTION_DISTANCE, fBottomZ);

	XObject* pTarObject = NULL;
	float fCurrDist = FLT_MAX;

	vector<XObject*> vecTargetList;
	pModuleCollision->GetInteractionObjectListColBox(vecTargetList, FIND_NPC, vPos, vDir, CollisionBox);

	for (size_t i = 0; i < vecTargetList.size(); i++)
	{
		XObject* pThisObject = vecTargetList[i];
		if (pThisObject)
		{
			if (pThisObject->GetEntityType() != ETID_NPC) 
			{
				_ASSERT(0);
				continue;
			}

			XNonPlayer* pNPC = pThisObject->AsNPC();
			if(XInteractionHelper::IsInteractableNPC(pNPC) == false) continue;

			float fTarDistance = pThisObject->GetPosition().DistanceTo(m_pOwner->GetPosition());
			if (fCurrDist < fTarDistance) continue;

			vec3 trpos;
			RActorNode* pTargetActorNode = pNPC->GetActor()->GetActorNodePos(INTERACTION_DUMMY, trpos, E_TS_WORLD,true);
			if(pTargetActorNode)
			{
				if(pNPC->GetInfo()->nNpcType == NPC_TYPE_OBJECT)
				{
					float distance = trpos.DistanceTo(m_pOwner->GetPosition());

					if (distance < INTERACTOR_DISTANCE)
					{
						pTarObject = pThisObject;
						fCurrDist = fTarDistance;
						continue;
					}
				}
			}
			else
			{
				pTarObject = pThisObject;
				fCurrDist = fTarDistance;
				continue;
			}
		}
	}

	if (pTarObject)
	{
		XGetInteractionInfo().SetInteractionSearchUID(pTarObject->GetUID());
		return true;
	}
	else
	{
		XGetInteractionInfo().SetInteractionSearchUID(MUID::ZERO);
		return false;
	}
}

bool XInteractor::CheckInteractionGather()
{
	XModuleCollision* pModuleCollision = m_pOwner->GetModuleCollision();
	if( pModuleCollision == NULL) return false;

	vec3 vPos = vec3(0.f, 0.f, 100.f);
	vec3 vDir = m_pOwner->GetDirection();

	float fCheckX = 100.f;
	float fBottomZ = -CHECK_INTERACTION_COL_Z*0.5f;
	RBoundingBox CollisionBox;
	CollisionBox.vmax.Set( fCheckX, fCheckX, CHECK_INTERACTION_COL_Z);
	CollisionBox.vmin.Set(-fCheckX, -CHECK_INTERACTION_DISTANCE, fBottomZ);

	vector<XObject*> vecTargetList;
	pModuleCollision->GetInteractionObjectListColBox(vecTargetList, FIND_DEADNPC, vPos, vDir, CollisionBox);

	XObject* pTarObject = NULL;
	float fCurrDist = FLT_MAX;

	for (size_t i = 0; i < vecTargetList.size(); i++)
	{
		XObject* pThisObject = vecTargetList[i];
		if (pThisObject)
		{
			if (pThisObject->GetEntityType() != ETID_NPC)
			{
				_ASSERT(0);
				continue;
			}

			float fTarDistance = pThisObject->GetPosition().DistanceTo(m_pOwner->GetPosition());
			if (fCurrDist < fTarDistance) continue;

			XNonPlayer* pNPC = pThisObject->AsNPC();
			if(pNPC->IsDead() && pNPC->GetInfo()->nGatherType == GATHER_NONE)  continue;

			vec3 trpos;
			RActorNode* pTargetActorNode = pNPC->GetActor()->GetActorNodePos(INTERACTION_DUMMY, trpos, E_TS_WORLD,true);
			if(pTargetActorNode)
			{
				if(pNPC->GetInfo()->nNpcType == NPC_TYPE_OBJECT)
				{
					float distance = trpos.DistanceTo(m_pOwner->GetPosition());

					if (distance < INTERACTOR_DISTANCE)
					{
						pTarObject = pThisObject;
						fCurrDist = fTarDistance;
						continue;
					}
				}
			}
			else
			{
				pTarObject = pThisObject;
				fCurrDist = fTarDistance;
				continue;
			}
		}
	}

	if (pTarObject)
	{
		XGetInteractionInfo().SetInteractionSearchUID(pTarObject->GetUID());
		return true;
	}
	else
	{
		XGetInteractionInfo().SetInteractionSearchUID(MUID::ZERO);
		return false;
	}
}

bool XInteractor::CheckBaseLootable(XNonPlayer* pTargetNPC)
{
	if (pTargetNPC->IsDead() == false) return false;

	XModuleEntity* pObjModuleEntity = pTargetNPC->GetModuleEntity();
	if (pObjModuleEntity == NULL) return false;
	if (pObjModuleEntity->IsLoadingCompleted() == false) return false;

	XModuleCollision* pObjCollision = pTargetNPC->GetModuleCollision();
	if (pObjCollision == NULL) return false;

	return true;
}

XInteractor::_CheckLootingResult XInteractor::CheckLootingInteractionObject()
{
	if (gg.omgr == NULL) return CLR_FALSE;
	vec3 vOwnerPos = m_pOwner->GetPosition();

	vector<XObject*> vecObj;
	gg.omgr->GetSortDistanceEntityTypeList(vecObj, ETID_NPC);
	if (vecObj.empty()) return CLR_FALSE;

	bool bExistRecentLootableNPC = false;

	for (vector<XObject*>::iterator itor = vecObj.begin(); itor != vecObj.end(); ++itor)
	{
		XObject* pObj = (*itor);
		XNonPlayer* pTargetNPC = AsNPC(pObj);

		if (pTargetNPC)
		{
			if (CheckBaseLootable(pTargetNPC))
			{
				if (pTargetNPC->IsLootable())
				{
					if (XInteractionHelper::IsLootableDistance(vOwnerPos, pTargetNPC))
					{
						XGetInteractionInfo().SetInteractionSearchUID(pTargetNPC->GetUID());
						return CLR_TRUE;
					}
				}
				else if (pTargetNPC->IsLootableRecently())
				{
					if (XInteractionHelper::IsLootableDistance(vOwnerPos, pTargetNPC))
					{
						bExistRecentLootableNPC = true;
					}
				}
			}
		}
	}

	XGetInteractionInfo().SetInteractionSearchUID(MUID::ZERO);

	if (bExistRecentLootableNPC)
	{
		return CLR_EXIST_RECENTLY;
	}

	return CLR_FALSE;
}

