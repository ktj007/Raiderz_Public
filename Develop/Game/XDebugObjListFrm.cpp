#include "StdAfx.h"
#include "XDebugObjListFrm.h"
#include "XModuleEntity.h"
#include "XModuleMovable.h"
#include "XModuleMotion.h"
#include "XModuleNetControl.h"
#include "XModuleNonControl.h"
#include "XGameTransformControllerManager.h"
#include "XPost_GM.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XNPCAligner.h"

XDebugObjListFrm::XDebugObjListFrm( const char* szName/*=NULL*/, MWidget* pParent/*=NULL*/, MListener* pListener/*=NULL*/ )
: mint3::MTextFrame(szName, pParent, pListener)
{
	SetRect(10, 120, 700, 240);
//	SetOpacity(0.9f);
	SetText("Object List");
}


void XDebugObjListFrm::OnRun( DWORD dwCurrTime )
{
	if (gg.omgr == NULL) return;

	ResetContent();

	int maxcnt = m_Rect.h/16;

	wchar_t text[1024] = L"";
	swprintf_s(text, L"PC ------------- ");
	AddString(MLocale::ConvUTF16ToAnsi(text).c_str());
	int cnt = 1;

	vector<XObject*> vecObj;
	gg.omgr->GetSortDistanceEntityTypeList(vecObj, ETID_PLAYER);

	for (vector<XObject*>::iterator itor = vecObj.begin(); itor != vecObj.end(); ++itor)
	{
		if (maxcnt/2 < cnt) break;

		XObject* pPC = (*itor);
		if (pPC->GetEntityType() != ETID_PLAYER) continue;

		if (pPC != NULL)
		{
			XModuleEntity* pModuleEntity = (XModuleEntity*)pPC->GetModule(XMID_ENTITY);
			if (pModuleEntity)
			{
				vec3 pos, dir;
				pos = pModuleEntity->GetPosition();
				dir = pModuleEntity->GetDirection();

				XModuleNetControl* pModuleNetControl = (XModuleNetControl*)pPC->GetModuleNetControl();
				XModuleMotion* pModuleMotion = (XModuleMotion*)pPC->GetModule(XMID_MOTION);
				XModuleMovable* pModuleMovable = (XModuleMovable*)pPC->GetModule(XMID_MOVABLE);
				const vec3& vel = pModuleMovable->GetMovableFactor().VELOCITY.m_vVelocity;

				MUID uid = pPC->GetUID();
				XNetPlayer* pNetPlayer = gg.omgr->FindNetPlayer(uid);

				wstring strActorUsable = L"false";
				wstring strAniUsable = L"false";
				if(pModuleEntity->GetActor() && pModuleEntity->GetActor()->GetUsable())
					strActorUsable = L"true";
				if(pModuleEntity->GetAnimationController()->IsAnimationUsable())
					strAniUsable = L"true";

				if (pNetPlayer && pModuleMotion)
				{
					swprintf_s(text, L"%s (%u:%u) uiid=%d , pos=(%.2f %.2f %.2f) , dir=(%.2f %.2f %.2f), vel=(%.2f %.2f %.2f), motion=%s , ani=%s state = %s actor_usable = %s ani_usable = %s", 
						pNetPlayer->GetName(),
						pPC->GetUID().High, pPC->GetUID().Low, pNetPlayer->GetUIID(),
						pos.x, pos.y, pos.z, dir.x, dir.y, dir.z,
						vel.x, vel.y, vel.z,
						pModuleMotion->GetCurrMotion().c_str(),
						pModuleEntity->GetAnimationController()->GetAnimationName().c_str(),
						pModuleNetControl->GetCurrentActionState()->GetDebugName(),
						strActorUsable.c_str(),
						strAniUsable.c_str());
					AddString(MLocale::ConvUTF16ToAnsi(text).c_str());
					cnt++;
				}
			}
		}
	}

	swprintf_s(text, L"NPC ------------- ");
	AddString(MLocale::ConvUTF16ToAnsi(text).c_str());
	cnt++;

	vecObj.clear();
	gg.omgr->GetSortDistanceEntityTypeList(vecObj, ETID_NPC);

	for (vector<XObject*>::iterator itor = vecObj.begin(); itor != vecObj.end(); ++itor)
	{
		if (maxcnt < cnt) break;

		XObject* pNPC = (*itor);
		if (pNPC->GetEntityType() != ETID_NPC) continue;

		if (pNPC != NULL)
		{
			XModuleEntity* pModuleEntity = (XModuleEntity*)pNPC->GetModule(XMID_ENTITY);
			if (pModuleEntity)
			{
				vec3 pos, dir;
				pos = pModuleEntity->GetPosition();
				dir = pModuleEntity->GetDirection();

				vec3 vPos = gvar.Game.pMyPlayer->GetPosition();
				float fObj1Distance = vPos.DistanceTo(pos);

				XModuleMotion* pModuleMotion = (XModuleMotion*)pNPC->GetModule(XMID_MOTION);

				if (pModuleMotion)
				{
					XNonPlayer* pNonPlayer = dynamic_cast<XNonPlayer*>(pNPC);
					XModuleNonControl* pModuleNonControl = pNonPlayer->GetModuleNonControl();

					bool bAlign = pModuleNonControl->GetAligner()->IsEnabled();

					swprintf_s(text, L"%s (%I64u) uiid=%d, pos = (%.2f %.2f %.2f) , dir = (%.2f %.2f %.2f), distance = %f, motion=%s , ani=%s , state=%s , align=%s", 
						pNPC->GetName(),
						pNPC->GetUID(), pNonPlayer->GetUIID(),
						pos.x, pos.y, pos.z, dir.x, dir.y, dir.z,
						fObj1Distance,
						pModuleMotion->GetCurrMotion().c_str(), 
						pModuleEntity->GetAnimationController()->GetAnimationName().c_str(),
						pModuleNonControl->GetActionStateName(),
						(bAlign) ? L"TRUE" : L"FALSE"
						);
					AddString(MLocale::ConvUTF16ToAnsi(text).c_str());
					cnt++;
				}
			}
		}
	}
}