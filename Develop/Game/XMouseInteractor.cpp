#include "stdafx.h"
#include "XMouseInteractor.h"
#include "XModuleCollision.h"
#include "XModuleUI.h"
#include "XCameraManager.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XPost_Interaction.h"
#include "XController.h"
#include "XInteractionHelper.h"
#include "XInteractionInfo.h"
#include "MMainFrame.h"

bool XMouseInteractor::CheckLineTarget( XObject* pObject, vec3& pos, vec3& dir, float& fOutDist )
{
	if(pObject->GetModuleCollision())
	{
		float fCurDist			= fOutDist;

		// 1. Pick으로 검사
		if(pObject->GetModuleCollision()->PickRayToObject(pos, dir, fCurDist, fOutDist))
		{
			return true;
		}
	}

	return false;
}

XObject* XMouseInteractor::OnOver( int x, int y )
{
	if (global.device == NULL) return NULL;

	XBaseCameraSceneNode* pCameraNode = global.camera->GetCamera();

	vec3 vDir, vPos;

	if(GetTrans3DCoordinates(x, y, vPos, vDir) == false)
		return NULL;

	XObject* pTarget = CheckObject(vPos, vDir);
	return pTarget;
}

XObject* XMouseInteractor::CheckObject( vec3 vPos, vec3 vDir )
{
	float fMinDist = FLT_MAX;
	XObject* pTarget = NULL;

	for(XObjectMap::iterator itor = gg.omgr->BeginItor(); itor != gg.omgr->EndItor(); ++itor)
	{
		XObject* pObj = (*itor).second;
		if (pObj->GetEntityType() != ETID_NPC && 
			pObj->GetEntityType() != ETID_PLAYER) continue;

		if(pObj->GetUID() == XGetMyUID()) continue;

		float fOutDist = FLT_MAX;

		if (CheckLineTarget(pObj, vPos, vDir, fOutDist))
		{
			if (fOutDist < fMinDist)
			{
				pTarget = pObj;
				fMinDist = fOutDist;
			}
		}
	}

	return pTarget;
}

bool XMouseInteractor::OnClick( int x, int y )
{
	if (global.device == NULL) return false;

	XBaseCameraSceneNode* pCameraNode = global.camera->GetCamera();

	vec3 vDir, vPos;

	if(GetTrans3DCoordinates(x, y, vPos, vDir) == false)
		return false;

	if(CheckInteractionObject(vPos, vDir))
		return true;

	return false;
}

void XMouseInteractor::InteractWith(XObject* pTarget)
{
	if(CheckInteractionableObject(pTarget))
		XGetInteractionInfo().SetInteractionSearchUID(pTarget->GetUID());
}

bool XMouseInteractor::CheckInteractionObject( vec3 vPos, vec3 vDir )
{
	float fMinDist = FLT_MAX;
	XObject* pTarget = NULL;

	for(XObjectMap::iterator itor = gg.omgr->BeginItor(); itor != gg.omgr->EndItor(); ++itor)
	{
		XObject* pObj = (*itor).second;
		if (pObj->GetEntityType() != ETID_NPC && 
			pObj->GetEntityType() != ETID_PLAYER) continue;

		if(pObj->GetUID() == XGetMyUID()) continue;

		float fOutDist = FLT_MAX;

		if (CheckLineTarget(pObj, vPos, vDir, fOutDist))
		{
			if (fOutDist < fMinDist)
			{
				pTarget = pObj;
				fMinDist = fOutDist;
			}
		}
	}

	if (pTarget &&
		CheckInteractionDistance(pTarget->GetPosition()))
	{
		InteractWith(pTarget);
		return true;
	}

	return false;
}

bool XMouseInteractor::CheckMyCharacterAction(XModuleMyControl* pModuleMyControl)
{
	// 액션 상태 검사
	const int nActionFSMStateID = pModuleMyControl->GetCurrentActionStateID();
	if (nActionFSMStateID == ACTION_STATE_SIT) 
	{
		// 앉은 상태는 푼다.
		XMyActionState* pState = pModuleMyControl->GetCurrentActionState();
		if(pState)
		{
			XEvent msg(XEVTL_SIT_RISE);
			pState->Event(msg);

			return true;
		}
	}

	return false;
}

bool XMouseInteractor::CheckInteractionDistance( vec3 vPos )
{
	float fLen = (gvar.Game.pMyPlayer->GetPosition() - vPos).Length();

	if(fLen > XInteractionHelper::GetInteractionDistance(gvar.Game.pMyPlayer))
		return false;

	return true;
}

void XMouseInteractor::Update()
{
	if(global.ui->IsMouseVisible() == false)
		return;

	Mint* pMint = global.ui->GetMint();
	if(pMint && pMint->GetMainFrame() && pMint->GetMainFrame()->GetMouseOver())
	{
		SetDefaultCursor();

		XGetInteractionInfo().SetInteractionSearchUID(MUID::ZERO);
		return;
	}

	XObject* pTarget = OnOver(MEvent::GetMouseClientPos().x, MEvent::GetMouseClientPos().y);

	if (pTarget && CheckInteractionDistance(pTarget->GetPosition()))
	{
		if(CheckInteractionableObject(pTarget))
		{
//			MCursorSystem::Set("defInteractionCursor");
			XGetInteractionInfo().SetInteractionSearchUID(pTarget->GetUID());
		}
	}
	else
	{
		SetDefaultCursor();

		XGetInteractionInfo().SetInteractionSearchUID(MUID::ZERO);
	}
}

bool XMouseInteractor::CheckInteractionableObject( XObject* pObject )
{
	XModuleMyControl* pModuleMyControl = gvar.Game.pMyPlayer->GetModuleMyControl();
	if (pModuleMyControl == NULL) 
		return false;

	if (pObject->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pNPC = pObject->AsNPC();
		if(XInteractionHelper::IsInteractableNPC(pNPC) == false) 
			return false;
	}
	else if (pObject->GetEntityType() == ETID_PLAYER)
	{
		if (pObject->GetUID() == XGetMyUID()) 
			return false;

		XPlayer* pTargetPlayer = pObject->AsPlayer();
		if (XInteractionHelper::IsInteractablePC(pTargetPlayer) == false)
			return false;
	}

	return true;
}

void XMouseInteractor::SetDefaultCursor()
{
	MCursor* pCursor = MCursorSystem::Get();
	if(pCursor && pCursor->GetName() == "defInteractionCursor")
	{
//		MCursorSystem::Set("defCursor");
	}
}
