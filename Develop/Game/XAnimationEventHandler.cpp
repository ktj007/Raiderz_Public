#include "stdafx.h"
#include "XAnimationEventHandler.h"
#include "XModuleEntity.h"
#include "XModuleMotion.h"
#include "XModuleNonControl.h"
#include "XModuleMovable.h"
#include "XModuleCollision.h"
#include "XCameraEffect.h"
#include "XEventID.h"
#include "XEffectManager.h"
#include "XGameTransformControllerManager.h"
#include "XEffectInvoker.h"
#include "XCameraManager.h"

#include "XGame.h"
#include "XWorld.h"
#include "RActorNodeConstraintController.h"

void XAnimationEventHandler::OnEvent( RActor* pActor, const RAnimationEvent* pEvent, XModuleEntity& entity )
{
	ANIMATIONEVENTMAP::iterator itr = m_eventHandlers.find(MLocale::ConvAnsiToUTF16(pEvent->m_strEvent.c_str()));
	if(itr!=m_eventHandlers.end())
	{
		itr->second(entity, MLocale::ConvAnsiToUTF16(pEvent->m_strParam1.c_str()), MLocale::ConvAnsiToUTF16(pEvent->m_strParam2.c_str()), MLocale::ConvAnsiToUTF16(pEvent->m_strParam3.c_str()));
	}
}

// 이벤트(사운드나 이펙트를 발생시킬)로 변경.
void XAnimationEventHandler::OnAnimationEvent_walk(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3)
{
	GetInstance().OnWalkSound(entity);
}

void XAnimationEventHandler::OnAnimationEvent_effect(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3)
{
	if( strEventParam1.empty() ||
		entity.GetActor() == NULL) 
	{
		return;
	}

	// 루프 이펙트가 루프인 애니메이션 프레임 마다 생성되지않도록 체크
	if(GetInstance().CheckDuplicationEffectLoop(entity, (wstring)strEventParam1, (wstring)strEventParam2, (wstring)strEventParam3))
		return;

	if (global.emgr == NULL) return;

	MUID uidEffect = MUID::ZERO;
	if( !strEventParam2.empty() )
	{
		RActorNode *pParentActorNode = entity.GetActor()->GetActorNode(MLocale::ConvUTF16ToAnsi(strEventParam2.c_str()).c_str());

		uidEffect = global.emgr->AddXEffect(strEventParam1, entity.GetActor(), pParentActorNode);

		//현재 애니메이션과 현재 모션이 다를수 있어서 현재 애니메이션으로 체크 하도록 수정.
		if( (uidEffect != MUID::ZERO) && entity.GetAnimationController()->IsAnimationLoop() )
		{
			// 루프 이펙트만 체크
			XEffect* pXEffect = global.emgr->GetXEffect(uidEffect);
			if( pXEffect && pXEffect->IsEffectLoop())
			{
				stLoopAniEffect stLoopEffect;
				stLoopEffect.m_uid = uidEffect;
				stLoopEffect.m_string1 = strEventParam1;
				stLoopEffect.m_string2 = strEventParam2;
				stLoopEffect.m_string3 = strEventParam3;

				entity.GetAniEffect().push_back(stLoopEffect);
			}
		}
	}
	else
	{
		//vec3 vObjectDir = entity.GetActor()->GetWorldDirection();

		//RMatrix matLocal;
		//matLocal.SetLocalMatrix(entity.GetPosition(), vObjectDir, vec3::AXISZ);
		global.emgr->AddXEffect(strEventParam1, entity.GetActor(), NULL);
	}
}

void XAnimationEventHandler::OnAnimationEvent_sound(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3)
{
	if (global.sound == NULL) return;
	if(strEventParam1.empty()) return;

	// 사운드 렌덤 발생(http://dev/redmine/issues/13696)
	int nRandomCount = _wtoi(strEventParam3.c_str());
	int nRandom = MMath::RandomNumber(1, 10);

	if(nRandomCount > 0 && nRandomCount < 10 && nRandomCount < nRandom)
		return;

	if ( global.sound)
		global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(strEventParam1.c_str()).c_str(), NULL, 0, entity.GetOwner()->GetUID(), MLocale::ConvUTF16ToAnsi(strEventParam2.c_str()).c_str());
}

void XAnimationEventHandler::OnAnimationEvent_spell_fire(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3)
{
}

void XAnimationEventHandler::OnAnimationEvent_arc_shoot(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3)
{
	OnAnimationEvent_spell_fire(entity, strEventParam1, strEventParam2, strEventParam3);
}

//void XAnimationEventHandler::OnAnimationEvent_camera(XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2)
//{
//	int nRadius = atoi(strEventParam2.c_str());
//
//	// 내 이벤트 인가?
//	// 나에 대한 카메라 이벤트는 없다.
//	if(entity.GetOwner()->GetUID() != gvar.MyInfo.MyUID)
//	{
//		XObject * pObject = gg.omgr->Find(gvar.MyInfo.MyUID);
//		if(pObject)
//		{
//			// 반경 조사
//			float fLen = (entity.GetOwner()->GetPosition() - pObject->GetPosition()).Length();
//			if(fLen <= nRadius)
//			{
//				// 카메라 이펙트 발생
//				// 
//				XCameraEffect::GetInstance().PlayCameraEffect(strEventParam1.c_str(), 0.f);
//			}
//		}
//	}
//}

XAnimationEventHandler& XAnimationEventHandler::GetInstance()
{
	static XAnimationEventHandler m_stHandler;
	return m_stHandler;
}

void XAnimationEventHandler::OnAnimationEvent_throw( XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3 )
{
	XObject* pTarget = gg.omgr->Find( entity.GetExtraTarget());
	if ( pTarget)
	{
		XEvent msg;
		msg.m_nID = XEVTL_THROW;
		pTarget->Event(msg);
	}
}

void XAnimationEventHandler::OnAnimationEvent_Custom( XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3 )
{
	vector<EFFECT_RESULT_DATA> vecUIDEffect;

	XEffectInvoker effectInvoker;
	
	// 중복 체크
	if(GetInstance().CheckDuplicationEffectLoop(entity, (wstring)strEventParam1, (wstring)strEventParam2, (wstring)strEventParam3))
	{
		return;
	}

	// 이펙트 발생
	effectInvoker.Invoke(entity.GetOwner(), (wstring)strEventParam1, (wstring)strEventParam2, (wstring)strEventParam3, &vecUIDEffect);

	for(vector<EFFECT_RESULT_DATA>::iterator itUID = vecUIDEffect.begin(); itUID != vecUIDEffect.end(); itUID++)
	{
		stLoopAniEffect stLoopEffect;
		stLoopEffect.m_uid = (*itUID).uidEffect;
		stLoopEffect.m_string1 = strEventParam1;
		stLoopEffect.m_string2 = strEventParam2;
		stLoopEffect.m_string3 = strEventParam3;

		if((*itUID).type == CET_MODEL)
		{
			// 중복 비교
			// Loop 이펙트인가?
			XEffect* pEffect = global.emgr->GetXEffect((*itUID).uidEffect);
			if(pEffect && pEffect->IsEffectLoop())
				entity.GetAniEffect().push_back(stLoopEffect);
		}
		else if((*itUID).type == CET_ILLUMINATION)
		{
			// 무조건 loop이다.
			entity.GetAniEffect().push_back(stLoopEffect);
		}
	}

	vecUIDEffect.clear();
}

bool XAnimationEventHandler::CheckDuplicationEffectLoop( XModuleEntity& entity, wstring& strParam1, wstring& strParam2, wstring& strParam3 )
{
	vector<stLoopAniEffect>::iterator itLoopAniEffect = entity.GetAniEffect().begin();
	while(itLoopAniEffect != entity.GetAniEffect().end())
	{
		XEffect* pEffect = global.emgr->GetXEffect(itLoopAniEffect->m_uid);
		if(pEffect == NULL)
		{
			itLoopAniEffect = entity.GetAniEffect().erase(itLoopAniEffect);
			continue;
		}

		if(itLoopAniEffect->m_string1 == strParam1 &&
			itLoopAniEffect->m_string2 == strParam2 &&
			itLoopAniEffect->m_string3 == strParam3)
		{
			return true;
		}

		++itLoopAniEffect;
	}

	return false;
}

void XAnimationEventHandler::OnWalkSound( XModuleEntity& entity )
{
	if (global.sound == NULL) return;

	/// 나의 발자국소리만 나도록.
	/// 바닥 텍스쳐의 재질에 맞게 소리가 나도록 수정
	if(entity.GetOwner()->GetUID() == XGetMyUID())
	{
		wstring mt = entity.GetOwner()->GetModuleCollision()->GetMaterialName();
		if(mt.empty())
			mt = gg.currentgamestate->GetWorld()->GetMaterialName(entity.GetPosition());
		wstring filename = L"footstep_" + mt + L"_R";

		if ( global.sound)
			global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(filename.c_str()).c_str(), NULL, 0, entity.GetOwner()->GetUID());
	}
}

void XAnimationEventHandler::OnAnimationEvent_camera( XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3 )
{
	float fLen = 0.0f;
	int nParamLen = _wtoi(strEventParam2.c_str());

	// 내 이벤트 인가?
	if(entity.GetOwner()->GetUID() != XGetMyUID())
	{
		XObject * pObject = gg.omgr->Find(XGetMyUID());
		if(pObject)
		{
			// 반경 조사
			fLen = (entity.GetOwner()->GetPosition() - pObject->GetPosition()).Length();
		}
	}

	if(fLen <= nParamLen)
	{
		// 카메라 이펙트 발생
		global.camera->PlayCameraEffect(strEventParam1.c_str(), 0.0f);
	}
}

void XAnimationEventHandler::OnAnimationEvent_swallowed_disappear( XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3 )
{
	XObject* pTarget = gg.omgr->Find( entity.GetExtraTarget());
	if ( pTarget)
	{
		XEvent msg;
		msg.m_nID = XEVTL_SWALLOWED_DISAPPEAR;
		pTarget->Event(msg);
	}
}

void XAnimationEventHandler::OnAnimationEvent_battle_start( XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3 )
{
	XModuleMotion* pModuleMotion = entity.GetOwner()->GetModuleMotion();
	if(pModuleMotion)
	{
		if(pModuleMotion->GetCurrMotion() == MOTION_NAME_RUN)
			pModuleMotion->ChangeStanceType();
	}

	//if(entity.GetOwner()->GetType() == XOBJ_MY_PLAYER ||
	//	entity.GetOwner()->GetType() == XOBJ_NET_PLAYER)
	//{
	//	wstring strR, strL;
	//	XPlayer* pPlayer = static_cast<XPlayer*>(entity.GetOwner());
	//	pPlayer->GetWeaponParentName(strR, strL);

	//	wstring strName = strR + "_b";
	//	RActorNode* pActorNode = entity.GetActor()->GetActorNode(strName.c_str());
	//	if(pActorNode)
	//	{
	//		pActorNode->SetVisibility(0.0f);
	//	}
	//}

}

void XAnimationEventHandler::OnAnimationEvent_battle_end( XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3 )
{
	XModuleMotion* pModuleMotion = entity.GetOwner()->GetModuleMotion();
	if(pModuleMotion)
	{
		if(pModuleMotion->GetCurrMotion() == MOTION_NAME_RUN)
			pModuleMotion->ChangeStanceType();
	}
}

void XAnimationEventHandler::OnAnimationEvent_breakable_parts( XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3 )
{
	XModuleActorControl* pModuleActorControl = entity.GetOwner()->GetModuleActorControl();
	if (pModuleActorControl == NULL) return;

	int nParamPartsIndex = _wtoi(strEventParam1.c_str());
	int nParamXYForce = _wtoi(strEventParam2.c_str());
	int nParamZForce = _wtoi(strEventParam3.c_str());

	nParamXYForce = XMath::RandomNumber(nParamXYForce - (int)(nParamXYForce * 0.1f), nParamXYForce + (int)(nParamXYForce * 0.1f));
	nParamZForce = XMath::RandomNumber(nParamZForce - (int)(nParamZForce * 0.1f), nParamZForce + (int)(nParamZForce * 0.1f));

	if (nParamPartsIndex == 0)
	{
		// 0이면 모든 파츠를 깬다.
		for (size_t i = 1; i < 10; i++)
		{
			pModuleActorControl->OnBreakParts(i, (float)nParamXYForce, (float)nParamZForce);
		}
	}
	else
	{
		pModuleActorControl->OnBreakParts(nParamPartsIndex, (float)nParamXYForce, (float)nParamZForce);
	}

}

void XAnimationEventHandler::OnAnimationEvent_face_ani_start( XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3 )
{
	bool bEye = false;
	if(strEventParam2 == L"true")
		bEye = true;

	entity.GetAnimationController()->StartFaceAnimation((wstring)strEventParam1, bEye);
}

void XAnimationEventHandler::OnAnimationEvent_face_ani_end( XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3 )
{
	float fEndBlendTime = _wtof(strEventParam2.c_str());
	entity.GetAnimationController()->EndFaceAnimation((wstring)strEventParam1, fEndBlendTime);
}

void XAnimationEventHandler::OnAnimationEvent_defence_start_sound( XModuleEntity& entity, const wstring& strEventParam1, const wstring& strEventParam2, const wstring& strEventParam3 )
{
	if (global.sound == NULL) return;
	if(strEventParam1.empty()) return;
	if(entity.GetAnimationController()->IsOncePlayDone()) return;

	mlog("체크 %d\n", entity.GetAnimationController()->IsOncePlayDone());
	if ( global.sound)
		global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(strEventParam1.c_str()).c_str(), NULL, 0, entity.GetOwner()->GetUID(), MLocale::ConvUTF16ToAnsi(strEventParam2.c_str()).c_str());

}
