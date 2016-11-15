#include "StdAfx.h"
#include "XCheckTargetableEffect.h"
#include "XMyPlayer.h"
#include "XNonPlayer.h"
#include "XInteractionHelper.h"
#include "XGameState.h"
#include "MockInteractionEffect.h"
#include "XCharacter.h"
#include "XInteractionInfo.h"
#include "XEffectManager.h"

XCheckTargetableEffect::XCheckTargetableEffect(XObject* pOwner)
: m_bShowLootbleTargetEffect(false)
{
	// 테스트라면 다른 클래스를 생성한다.
	if(gg.currentgamestate &&
		gg.currentgamestate->GetRealGameNotTest() == false)
	{
		m_pInteractionEffect			= new MockInteractionEffect(pOwner);
	}
	else
	{
		m_pInteractionEffect			= new XInteractionEffect(pOwner);
	}
}

XCheckTargetableEffect::~XCheckTargetableEffect()
{
	Destroy();

	SAFE_DELETE(m_pInteractionEffect);
}

void XCheckTargetableEffect::Destroy()
{
	if (m_pInteractionEffect)
		m_pInteractionEffect->Destroy();
}

void XCheckTargetableEffect::Update(XObject* pOwner)
{
	if (gvar.Game.pMyPlayer == NULL) return;

	if (pOwner->GetEntityType() == ETID_NPC)
	{
		XNonPlayer* pTargetNPC	= pOwner->AsNPC();
		XNPCInfo* pNPCInfo		= pTargetNPC->GetInfo();

		if(pNPCInfo->nNpcType == NPC_TYPE_OBJECT)
		{
			CheckTargetableMapObject(pOwner);
		}
		else
		{
			CheckTargetableNPC(pOwner);
		}
	}
	else if (pOwner->GetEntityType() == ETID_PLAYER)
	{
		CheckTargetableNetPlayer(pOwner);
	}
}

void XCheckTargetableEffect::CheckTargetableMapObject( XObject* pOwner )
{
	if( XGetInteractionInfo().GetInteractionSearchUID() == pOwner->GetUID() )
	{
		XNonPlayer* pTargetNPC	= pOwner->AsNPC();
		XNPCInfo* pNPCInfo		= pTargetNPC->GetInfo();

		if (pNPCInfo->bInteract && pTargetNPC->GetIConState() != NIS_NONE)
		{
			EnableGlowEffect(pOwner);
			return;
		}
	}

	DestroyTargetEffect();
	DisableGlowEffect(pOwner);
}


void XCheckTargetableEffect::CheckTargetableNPC( XObject* pOwner )
{
	XNonPlayer* pNPC	= pOwner->AsNPC();
	XNPCInfo* pNPCInfo	= pNPC->GetInfo();

	vec3 vPos				= gvar.Game.pMyPlayer->GetPosition();
	vec3 vObjPos			= pOwner->GetPosition();
	float fRadius			= pOwner->GetRadius();
	float fOutDist			= vPos.DistanceTo(vObjPos);

	enum TargetType
	{
		TargetType_NONE,
		TargetType_NPC,
		TargetType_LOOT
	} nTargetType = TargetType_NONE;

	// 타겟 타입 구분 ------
	if (pNPCInfo->bInteract &&
		pNPC->GetIConState() != NIS_NONE &&
		pNPC->GetIConState() != NIS_COMBAT &&
		pNPC->IsAttackable() == false)
	{
		nTargetType = TargetType_NPC;
	}
	else 
	if(pNPC->IsLootable() && m_bShowLootbleTargetEffect)
	{
		nTargetType = TargetType_LOOT;
	}
	else
	{
		nTargetType = TargetType_NONE;
	}

	// 거리 체크 ------
	bool bFarDistance = false;

	switch (nTargetType)
	{
	case TargetType_NPC:
	case TargetType_NONE:
		{
			if (fOutDist >= XInteractionHelper::GetInteractionDistance(pOwner))
			{
				bFarDistance = true;
			}
		}
		break;
	case TargetType_LOOT:
		{
			if (XInteractionHelper::IsLootableDistance(fOutDist, pOwner))
			{
				bFarDistance = true;
			}
		}
		break;
	}
	if (bFarDistance)
	{
		DestroyTargetEffect();
		return;
	}

	// 이펙트 ------
	switch (nTargetType)
	{
		case TargetType_NPC:
			{
				if( XGetInteractionInfo().GetInteractionSearchUID() == pOwner->GetUID() &&
					XGetInteractionInfo().m_bNPCInteractionState == false)
				{
					EnableGlowEffect(pOwner);
					global.ui->OnGuideMsg(GUIDE_MSG_CAN_INTERACT_TO_NPC, pOwner->GetUID());
				}
				else
				{
					DisableGlowEffect(pOwner);
					global.ui->ReleaseGuideMsg(GUIDE_MSG_CAN_INTERACT_TO_NPC, pOwner->GetUID());
				}
			}
			break;
	case TargetType_LOOT:
		{
			if( XGetInteractionInfo().GetInteractionSearchUID() == pOwner->GetUID())
			{
				if(pOwner->GetType() == XOBJ_BPART_NPC)
					EnableGlowEffect(pOwner, XCONST::INTERACTION_BPART_TARGET_EDGE_COLOR);
				else
					EnableGlowEffect(pOwner);
				global.ui->OnGuideMsg(GUIDE_MSG_CAN_LOOT, pOwner->GetUID());
			}
			else
			{
				DisableGlowEffect(pOwner);
				DestroyTargetEffect();
				global.ui->ReleaseGuideMsg(GUIDE_MSG_CAN_LOOT, pOwner->GetUID());
			}
		}
		break;
	case TargetType_NONE:
		{
			//루팅후 타겟표시 없애주기
			DestroyTargetEffect();
			DisableGlowEffect(pOwner);
		}
		break;
	}
}

void XCheckTargetableEffect::CheckTargetableNetPlayer( XObject* pOwner )
{
	if(global.ui->IsMouseVisible() == false && gvar.Game.pMyPlayer->IsCurrStanceBattle())
	{
		DestroyTargetEffect();
		DisableGlowEffect(pOwner);
		return;
	}

	if((XGetInteractionInfo().GetInteractionSearchUID() == pOwner->GetUID()) &&
		(XGetInteractionInfo().VSPlayerUID == MUID::ZERO) &&
		(gvar.MyInfo.BattleArenaInfo.IsInBattleArena() == false))
	{
		if(pOwner->GetActor() && pOwner->GetActor()->IsPEEdge() == false)
		{
			if ( global.sound)
				global.sound->PlayDynamic3D( "interact_round", NULL, 0, pOwner->GetUID());

			//CreateTargetEffect(INTERACTION_INTERACTIVE_NPC);
			EnableGlowEffect(pOwner);
		}
		return;
	}

	DestroyTargetEffect();
	DisableGlowEffect(pOwner);
}

void XCheckTargetableEffect::CreateTargetEffect(INTERACTION_EFFECT_TYPE eEffectType)
{
	if(m_pInteractionEffect)
	{
		if( m_pInteractionEffect->GetParent() == NULL || m_pInteractionEffect->IsEffect() == false )
			m_pInteractionEffect->CreateInteractionEffect(eEffectType);
		else
		{
			m_pInteractionEffect->SetInteractionEffectType(eEffectType);
		}
	}
}

void XCheckTargetableEffect::DestroyTargetEffect()
{
	if(m_pInteractionEffect && m_pInteractionEffect->IsEffect())
	{
		m_pInteractionEffect->Destroy();
	}
}

void XCheckTargetableEffect::EnableGlowEffect( XObject* pOwner, vec4 vColor /*= XCONST::INTERACTION_TARGET_EDGE_COLOR */ )
{
	global.emgr->EnablePEEdge((MUID)pOwner->GetUID(), vColor, XCONST::INTERACTION_TARGET_EDGE_TICKNESS, XCONST::INTERACTION_TARGET_EDGE_BLUR_FACTOR, false);
}

void XCheckTargetableEffect::DisableGlowEffect( XObject* pOwner )
{
	global.emgr->DisablePEEdge((MUID)pOwner->GetUID());
}

void XCheckTargetableEffect::DisableTargetEffect( XObject* pOwner )
{
	DisableGlowEffect(pOwner);
}
