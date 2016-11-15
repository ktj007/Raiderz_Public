#include "StdAfx.h"
#include "XModuleEffect.h"
#include "XEventID.h"
#include "XNonPlayer.h"
#include "XMyPlayer.h"
#include "XModuleMotion.h"
#include "XModuleActorControl.h"
#include "XModuleCollision.h"
#include "XModuleTalent.h"
#include "XModuleEntity.h"
#include "XModuleUI.h"
#include "XModuleBParts.h"
#include "XCharacterCaption.h"
#include "XCameraManager.h"
#include "XSystem.h"
#include "XCharacter.h"
#include "XTalentEffectPreLoader.h"
#include "XEffectInvoker.h"
#include "XCaptionEffect.h"
#include "XInteractionHelper.h"
#include "XHeadIcon.h"
#include "XTalentHelper.h"
#include "XStrings.h"


XModuleEffect::XEffectEvent::XEffectEvent( XModuleEffect * pOwner )
{
	m_pOwner = pOwner;

	m_vecEffectEventList.clear();
}

XModuleEffect::XEffectEvent::~XEffectEvent()
{
	m_pOwner = NULL;

	for(vector<XEffectEventData *>::iterator itEvent = m_vecEffectEventList.begin(); itEvent != m_vecEffectEventList.end(); ++itEvent)
	{
		SAFE_DELETE(*itEvent);
	}

	for(vector<XEffectEventData *>::iterator itSendEvent = m_vecSendEffectEventList.begin(); itSendEvent != m_vecSendEffectEventList.end(); ++itSendEvent)
	{
		SAFE_DELETE(*itSendEvent);
	}

	m_vecEffectEventList.clear();
	m_vecSendEffectEventList.clear();
}

void XModuleEffect::XEffectEvent::update( float fDelta )
{
	PFC_THISFUNC;
	for(vector<XEffectEventData *>::iterator itEvent = m_vecEffectEventList.begin(); itEvent != m_vecEffectEventList.end(); ++itEvent)
	{
		m_vecSendEffectEventList.push_back(*itEvent);
	}

	m_vecEffectEventList.clear();

	sendUpdate(fDelta);
}

void XModuleEffect::XEffectEvent::sendUpdate( float fDelta )
{
	PFC_THISFUNC;

	vector<XEffectEventData *>::iterator itEvent = m_vecSendEffectEventList.begin();
	while(itEvent != m_vecSendEffectEventList.end())
	{
		(*itEvent)->m_fRemainTime -= fDelta;
		if((*itEvent)->m_fRemainTime < 0.0f &&
			(*itEvent)->OnEffectEvent(m_pOwner) == true)
		{
			SAFE_DELETE(*itEvent);
			itEvent = m_vecSendEffectEventList.erase(itEvent);
			continue;
		}
		itEvent++;
	}
}

void XModuleEffect::XEffectEvent::push_back( XEffectEventData* pEvent )
{
	m_vecEffectEventList.push_back(pEvent);
}

int XModuleEffect::XEffectEvent::GetEffectEventIDCount( XEffectEventID nEventID )
{
	int nCount = 0;

	vector<XEffectEventData *>::iterator itEvent = m_vecEffectEventList.begin();
	while(itEvent != m_vecEffectEventList.end())
	{
		if((*itEvent)->m_nEffectEventID == nEventID)
		{
			nCount++;
		}

		itEvent++;
	}

	return nCount;
}

void XModuleEffect::XEffectEvent::OnEvent( XEffectEventData* pEventData )
{
	if(pEventData == NULL)
		return;

	pEventData->OnEffectEvent(m_pOwner);
	SAFE_DELETE(pEventData);
}

//////////////////////////////////////////////////////////////////////////
XModuleEffect::XModuleEffect( XObject* pOwner/*=NULL*/ )
: XModule(pOwner)
{
	m_pHeadEffect			= NULL;
	m_pBuffEffect			= NULL;
	m_pNPCIConEffect		= NULL;
	m_pTalentEffect			= NULL;
	m_pSwordTrail			= NULL;

	m_pEffectEvent			= new XEffectEvent(this);
	m_pCaptionEffect		= new XCaptionEffect();

	m_uidPlayerIcon			= MUID::ZERO;
}

XModuleEffect::~XModuleEffect(void)
{
	SAFE_DELETE(m_pCaptionEffect);
	SAFE_DELETE(m_pHeadEffect);
	SAFE_DELETE(m_pBuffEffect);
	SAFE_DELETE(m_pNPCIConEffect);
	SAFE_DELETE(m_pTalentEffect);

	SAFE_DELETE(m_pEffectEvent);

	if(m_pSwordTrail)
		m_pSwordTrail->Destroy();
	SAFE_DELETE(m_pSwordTrail);

	DestroyPlayerIcon();
}

void XModuleEffect::SetRegisterUseEffectID( int nUseEffectID )
{
	// 속성에 따른 이펙트 분류
	if((nUseEffectID & (1 << XEFT_ID_HEAD)) &&
		m_pHeadEffect == NULL)
	{
		m_pHeadEffect = new XHeadEffect(m_pOwner);
	}

	if((nUseEffectID & (1 << XEFT_ID_BUFF)) &&
		m_pBuffEffect == NULL)
	{
		m_pBuffEffect = new XBuffEffect(m_pOwner);
	}

	if((nUseEffectID & (1 << XEFT_ID_NPCICON)) &&
		m_pNPCIConEffect == NULL)
	{
		m_pNPCIConEffect = new XNPCIConEffect;
	}

	if((nUseEffectID & (1 << XEFT_ID_TALENT)) &&
		m_pTalentEffect == NULL)
	{
		m_pTalentEffect = new XTalentEffect;
	}

	if((nUseEffectID & (1 << XEFT_ID_SWORDTRAIL)) &&
		m_pSwordTrail == NULL)
	{
		m_pSwordTrail = new XSwordTrail();
	}
}

void XModuleEffect::OnSubscribeEvent()
{
	XModule::OnSubscribeEvent();

	Subscribe(XEVTL_MOTION_PLAYONCE_NPC_DIE);
	Subscribe(XEVTL_ON_CHANGE_ANIMATION);
	Subscribe(XEVTL_ON_CHANGE_MOTION);

	Subscribe(XEVTD_MESH_LOADING_COMPLETE);

	Subscribe(XEVTD_NPC_SAY_END);	
}

XEventResult XModuleEffect::OnEvent( XEvent& msg )
{
	switch (msg.m_nID)
	{
	case XEVTL_MOTION_PLAYONCE_NPC_DIE:
		{
			m_pOwner->GetActor()->RefreshInteractionAABB();

			//다이애니메이션이 끝나면 루팅가능이펙트 보여주기.
			SetLootableEffectByDie();

			//즉을때 타켓인디게이터 위치 수정용
			XNonPlayer* pTargetNPC = m_pOwner->AsNPC();
			if (pTargetNPC->IsLootable())
			{
				gvar.Game.pMyPlayer->GetModuleMyControl()->ChangeIndicatorEffectPos();  
			}
		}
		return MR_TRUE;

	case XEVTL_ON_CHANGE_ANIMATION:
		{
			ChangeSwordTrailAnimation();
		}
		return MR_TRUE;

	case XEVTD_MESH_LOADING_COMPLETE:
		{
			OnLoadingComplete();
		}
		break;
	case XEVTD_NPC_SAY_END:
		{
			ReSetNPCIConEffect();
		}
		return MR_TRUE;

	case XEVTL_ON_CHANGE_MOTION:
		{
			// 자신한테 붙은 이펙트에게 모션 변경에 따른 이펙트 캔슬을 물어본다.
			global.emgr->ActorChangeMotion(m_pOwner->GetUID());
		}
		break;
	}

	return MR_FALSE;
}

//////////////////////////////////////////////////////////////////////////

#include "RDebugPassRenderer.h"

void XModuleEffect::OnUpdate( float fDelta )
{
	PFC_THISFUNC;

//	TIMEPROFILE_THIS_FUNCTION;

	PFI_BLOCK_THISFUNC(306);

	m_pEffectEvent->update(fDelta);

	if(m_pTalentEffect)
		m_pTalentEffect->Update(fDelta);
	if(m_pBuffEffect)
		m_pBuffEffect->Update(fDelta);
	if(m_pHeadEffect)
		m_pHeadEffect->Update(fDelta);
}

void XModuleEffect::DeleteLootableEffect()
{
	if (m_pHeadEffect)
	{
		m_pHeadEffect->DeleteHeadEffect(HET_LOOTABLE);
	}
}

void XModuleEffect::CreateLootableEffect()
{
	if (m_pHeadEffect)
	{
		m_pHeadEffect->CreateHeadEffect(HET_LOOTABLE);
	}
}

void XModuleEffect::SetLootableEffectByDie()
{
	XNonPlayer* pTargetNPC = m_pOwner->AsNPC();
	if (pTargetNPC->IsLootable() == false) return;
	if (pTargetNPC->GetInfo()->nGatherType != GATHER_NONE) return;

	CreateLootableEffect();
}

void XModuleEffect::SetLootableEffectByIcon()
{
	CreateLootableEffect();
}

void XModuleEffect::OnEffectEvent( XEffectEventID nEffectEventID, XEffectEventData* pEffectData /*= NULL*/ )
{
	// 데이터 정리
	if(pEffectData == NULL)
	{
		pEffectData = new XEffectEventData;
	}

	pEffectData->m_nEffectEventID = nEffectEventID;

	// 애니메이션 업데이트 완료 체크
	XModuleEntity* pModuleEntity = m_pOwner->GetModuleEntity();
	if(pModuleEntity->IsAnimationUpdateCompleted() == false)
	{
		// 아직이면 업데이트일때 처리하도록 대기...
		m_pEffectEvent->push_back(pEffectData);

		return;
	}

	// 완료면 바로 처리
	m_pEffectEvent->OnEvent(pEffectData);
}

void XModuleEffect::OnEffectEvent( XEffectEventID nEffectEventID, int nParam1 )
{
	XEffectEventData* pEffectData = new XEffectEventData;
	pEffectData->m_nEffectEventID	= nEffectEventID;
	pEffectData->m_nParam1			= nParam1;

	OnEffectEvent(nEffectEventID, pEffectData);
}

// 노말 데미지는 나와 싸우고 있는 상대만 보이도록 하고
// 다른 스탠스면 다 보이도록
void XModuleEffect::ShowDamageCaptionEffect( _DamageInfo * pInfo, vec3 vDamagePos )
{
	m_pCaptionEffect->ShowDamage(m_pOwner, pInfo, vDamagePos);
}

void XModuleEffect::ShowExpCaptionEffect( int nExp )
{
	m_pCaptionEffect->ShowExp(m_pOwner, nExp);
}

void XModuleEffect::ShowBuffCaptionEffect( const wchar_t* szName, bool bIsDebuff)
{
	m_pCaptionEffect->ShowBuff(m_pOwner, szName, bIsDebuff);
}

vec3 XModuleEffect::GetDamageCaptionEffectPos( vec3 vHitEffectPos )
{
	vec3 ret = vHitEffectPos;
	ret.z += 70.0f;
	return ret;
}

void XModuleEffect::ShowHitAndDamage( _DamageInfo * pInfo, XObject * pAttacker, const vec3& vOffset )
{
	XModuleCollision* pModuleCollision = m_pOwner->GetModuleCollision();
	if (pModuleCollision == NULL) return;

	// 피격 위치 가져오기
	vec3 vHitCenterPos = pModuleCollision->GetMeleeHitEffectPosition(pAttacker, pInfo->nAttackedTalentID) + vOffset;

	// 맞은 이펙트
	HITEFFECT_RESULT resultHitEffect;

	if(pInfo->nDamageFlag != DF_BUFF &&							// 버프는 맞은 이펙트가 안나온다.
		pInfo->nDamageFlag != DF_GUARD_PARTIAL &&				// 가드는 맞은 이펙트가 안나온다.
		pInfo->nMFState != MF_DEFLECT &&
		(pInfo->IsFlagImmune() == false) &&		// 무효
		(pInfo->IsFlagAvoid() == false))	// 회피
	{
		XModuleTalent * pModuleTalent = m_pOwner->GetModuleTalent();
		if(pModuleTalent)
		{
			pModuleTalent->HitEffect(pInfo->nAttackedTalentID, pAttacker, vHitCenterPos, resultHitEffect, pInfo->bSecondaryEffect, pInfo->nFlags);
		}
	}

	// 브레이커블 파츠 히트
	ShowPartsHit(pInfo, vHitCenterPos, resultHitEffect);

	// damage caption
	vec3 vDamagePos = GetDamageCaptionEffectPos(vHitCenterPos);
	ShowDamageCaptionEffect(pInfo, vDamagePos);

	// 플래그에 따른 사운드
	PlayDamageEffect(pInfo);
}

void XModuleEffect::ShowDamageInfo( _DamageInfo * pInfo, const MUID& uidTarget )
{
	wstring strFlags = m_pCaptionEffect->GetDamageString(pInfo);
	if(!strFlags.empty())
	{
		strFlags = L"[" + strFlags + L"]";
	}

	XObject* pObj = gg.omgr->Find(uidTarget);

	if (uidTarget == XGetMyUID())
	{
		gvar.MyInfo.SetHP(gvar.MyInfo.ChrStatus.nHP - pInfo->nDamage);

		if (pInfo->nDamage > 0)
		{
			// 전투메세지 분리 _by tripleJ 110504
			if(pInfo->nDamageFlag == DF_BUFF)
			{
//				global.ui->OnSystemMsg(L"당신은 {0}의 피해를 입었습니다. ({1}) {2}", FSParam(pInfo->nDamage, pInfo->strName, strFlags));		// <-- 뭔 소린지 모름... 이런거 자제염
				//global.ui->OnSystemMsg( XGetStr( L"MSG_RECIEVEDDAMAGE"), FSParam(pInfo->nDamage));
				global.ui->OnBattleMsg( XGetStr( L"MSG_RECIEVEDDAMAGE"), FSParam(pInfo->nDamage));
			}
			else
			{
//				global.ui->OnSystemMsg(L"당신은 {0}의 피해를 입었습니다. {1}", FSParam(pInfo->nDamage, strFlags));								// <-- 뭔 소린지 모름... 이런거 자제염
				//global.ui->OnSystemMsg( XGetStr( L"MSG_RECIEVEDDAMAGE"), FSParam(pInfo->nDamage));
				global.ui->OnBattleMsg( XGetStr( L"MSG_RECIEVEDDAMAGE"), FSParam(pInfo->nDamage));
			}
		}
	}
	else if (pObj && pObj->GetEntityType() == ETID_NPC && pInfo->nAttackerUIID == XGetMyUIID())
	{
		XNonPlayer* pNPC = AsNPC(pObj);

		if (pInfo->nDamage > 0)
		{
			// 전투 메세지 분리 _by tripleJ 110504
			if(pInfo->nDamageFlag == DF_BUFF)
			{
//				global.ui->OnSystemMsg(L"{0,이} {1}의 피해를 입었습니다. ({2}) {3}", FSParam(pNPC->GetInfo()->GetName(), pInfo->nDamage, pInfo->strName, strFlags));	// <-- 뭔 소린지 모름... 이런거 자제염
				//global.ui->OnSystemMsg( XGetStr( L"MSG_RECIEVEDDAMAGEOTHER"), FSParam(pNPC->GetInfo()->GetName(), pInfo->nDamage));
				global.ui->OnBattleMsg( XGetStr( L"MSG_RECIEVEDDAMAGEOTHER"), FSParam(pNPC->GetInfo()->GetName(), pInfo->nDamage));
			}
			else
			{
//				global.ui->OnSystemMsg(L"{0,이} {1}의 피해를 입었습니다. {2}", FSParam(pNPC->GetInfo()->GetName(), pInfo->nDamage, strFlags));							// <-- 뭔 소린지 모름... 이런거 자제염
				//global.ui->OnSystemMsg( XGetStr( L"MSG_RECIEVEDDAMAGEOTHER"), FSParam(pNPC->GetInfo()->GetName(), pInfo->nDamage));
				global.ui->OnBattleMsg( XGetStr( L"MSG_RECIEVEDDAMAGEOTHER"), FSParam(pNPC->GetInfo()->GetName(), pInfo->nDamage));
			}
		}

	}
}

void XModuleEffect::ShowDebugCaptionEffect( const wchar_t* szMessage, uint32 nColor )
{
	m_pCaptionEffect->ShowDebug(m_pOwner, szMessage, nColor);
}

void XModuleEffect::ShowHeal( int nHeal, UIID nUserUIID /*= UIID_INVALID*/ )
{
	m_pCaptionEffect->ShowHeal(m_pOwner, nHeal, nUserUIID);
}

void XModuleEffect::ShowHPRegen( int nHeal )
{
	m_pCaptionEffect->ShowHPRegen(m_pOwner, nHeal);
}

void XModuleEffect::ShowGuardDefense(GUARD_TYPE eGuardType, int nDamage)
{
	m_pCaptionEffect->ShowGuardDefense(m_pOwner, nDamage, eGuardType);

	_DamageInfo damageInfo;
	damageInfo.nDamage = nDamage;
	ShowDamageInfo(&damageInfo, m_pOwner->GetUID());
}

MMatrix XModuleEffect::GetMeleeHitDirection(XTalentInfo* pTalentInfo)
{
	if(m_pOwner->GetActor() == NULL)
		return MMatrix::IDENTITY;

	float fCurAniTime = m_pOwner->GetActor()->GetCurFrameToSec();

	// 제일 가까운 시간을 가져온다.
	float fTempNearTime = FLT_MAX;
	float fHitTime = 0.f;
	for(vector<CSHitSegment>::iterator itSeg = pTalentInfo->m_HitInfo.m_vSegments.begin(); itSeg != pTalentInfo->m_HitInfo.m_vSegments.end(); itSeg++)
	{
		float fCalNear = abs(itSeg->m_fCheckTime - fCurAniTime);
		if(fCalNear < fTempNearTime)
		{
			fTempNearTime = fCalNear;
			fHitTime = itSeg->m_fCheckTime;
		}
	}

	wstring n = MLocale::ConvAnsiToUTF16(m_pOwner->GetActor()->GetMeshName().c_str());
	wstring nn = n.substr(0, n.rfind(L"."));

	vec3 vSwordHitDir = pTalentInfo->GetSwordHitDir(fHitTime, nn);

	MMatrix matCal;
	matCal.MakeIdentity();

	if(vSwordHitDir != vec3::AXISZ)
		matCal.SetLocalMatrix(vec3::ZERO, vec3::AXISY, vec3(-vSwordHitDir.x, vSwordHitDir.y, vSwordHitDir.z));

	return matCal;
}

void XModuleEffect::OnLoadingComplete()
{
	global.emgr->ActorLoadingComplete(m_pOwner->GetUID());

	if(m_pHeadEffect)
		m_pHeadEffect->OnLoadingComplete();
}

void XModuleEffect::CreateCutSceneEffect()
{
	if(m_pOwner->GetActor()->CheckLoadingCompletedFastly() == false)
	{
		EFFECT_CREATE_DATA effectCreateDatat;
		effectCreateDatat.strEffectName		= ICON_EFFECT_CUTSCENE;
		effectCreateDatat.strBoneName		= HEAD_EFFECT_BASE_BONE;
		effectCreateDatat.uidActor			= m_pOwner->GetUID();
		m_uidPlayerIcon = global.emgr->AddEffectWaiting(effectCreateDatat);
	}
	else
	{
		RActorNode* pActorNode = m_pOwner->GetActor()->GetActorNode(MLocale::ConvUTF16ToAnsi(HEAD_EFFECT_BASE_BONE).c_str());

		m_uidPlayerIcon = global.emgr->AddXEffect(ICON_EFFECT_CUTSCENE, m_pOwner->GetActor(), pActorNode);
	}
}

void XModuleEffect::DestroyPlayerIcon()
{
	if(global.emgr)
		global.emgr->DeleteXEffect(m_uidPlayerIcon);

	m_uidPlayerIcon = MUID::ZERO;
}

void XModuleEffect::DestroyNPCIconEffect()
{
	XNonPlayer* pTargetNPC	= m_pOwner->AsNPC();
	if (pTargetNPC->GetIConState() != NIS_NONE)
		m_pNPCIConEffect->Destroy(pTargetNPC);
}

void XModuleEffect::CreateRebirthEffect()
{
	XEffectInvoker effectInvoker;
	effectInvoker.Invoke(m_pOwner, wstring(REBIRTH_EFFECT_NAME), wstring(), wstring());
}

void XModuleEffect::ShowEmptyProjrctileEffect( XObject* pTarget, XObject* pAttacker, CSProjectileInfo* pProjectileInfo, ProjectileColJudgmentType colType )
{
	// 오브젝트 충돌을 검사해서 캡슐에 맞는 위치를 가져온다.
	if(pTarget == NULL || pAttacker == NULL)
		return;

	XModuleCollision *pModuleCollision = pTarget->GetModuleCollision();
	if(pModuleCollision == NULL)
		return;

	vec3 vStartPos = vec3::ZERO;
	vec3 vTargetPos = pTarget->GetPosition();
	pAttacker->GetWorldTransform().TransformVect(pProjectileInfo->m_vLocalStartPos, vStartPos);
	vTargetPos.z = vStartPos.z;

	vec3 vImpactPos, vImpactDir;
	float out_Distance;
	MCapsule capsuleCollision(vStartPos, vTargetPos, pProjectileInfo->m_fHitRadius);
	pModuleCollision->ColCapsuleToObject(capsuleCollision, capsuleCollision, vTargetPos - vStartPos, vImpactPos, vImpactDir, out_Distance);

	XProjectileEffect projectileEffect;
	projectileEffect.CreateHitDefenceEffect(pTarget, projectileEffect.GetProjectileColEffect(pProjectileInfo, colType), vImpactPos, vImpactDir);
}

void XModuleEffect::ChangeSwordTrailAnimation()
{
	XModuleTalent* pModuleTalent = m_pOwner->GetModuleTalent();
	if(pModuleTalent == NULL)
		return;

	if(m_pOwner->GetType() == XOBJ_NPC)
	{
		int a = 0;
	}
	// 탤런트 액티브 상태여야 한다.
	if(pModuleTalent->IsActive())
		OnEffectEvent(XEFTEVT_EFFECT_SWORDTRAIL);

}

void XModuleEffect::ReSetNPCIConEffect()
{
	XNonPlayer* pNPC = AsNPC(m_pOwner);
	if (pNPC == NULL) return;

	NPC_ICON_STATE NIS = pNPC->GetIConState();
	if (NIS != NIS_NONE)
		m_pNPCIConEffect->SetNPCICon(m_pOwner, NIS);
}

void XModuleEffect::ShowEn( int nEn )
{
	m_pCaptionEffect->ShowEn(m_pOwner, nEn);
}

void XModuleEffect::ShowPartsHit( _DamageInfo* pInfo, vec3 vHitPos, HITEFFECT_RESULT& resultHitEffect )
{
	// NPC에만 해당사항이다.
	if(m_pOwner->GetType() != XOBJ_NPC)
		return;

	int nBPartIndex = 0;
	if (pInfo->IsFlagBPart1())	nBPartIndex = 1;
	else if (pInfo->IsFlagBPart2())	nBPartIndex = 2;
	else if (pInfo->IsFlagBPart3())	nBPartIndex = 3;
	else if (pInfo->IsFlagBPart4())	nBPartIndex = 4;
	else 
		return;

	// 번쩍 효과
	XNonPlayer* pNPC = static_cast<XNonPlayer*>(m_pOwner);
	XModuleBParts *pModuleBParts = pNPC->GetModuleBParts();

	wstring strBPartName;
	if(pModuleBParts)
		strBPartName = pModuleBParts->GetBreakablePartsActorNodeName(nBPartIndex);

	//XModuleEntity* pModuleEntity = m_pOwner->GetModuleEntity();
	//if(pModuleEntity)
	//{
	//	pModuleEntity->StartMultiplyColorForBParts(strBPartName, nBPartIndex - 1);
	//}

	// 이펙트
	XNPCInfo* pNpcInfo = pNPC->GetInfo(true);
	if(pNpcInfo == NULL)
		return;

	EFFECT_POST_TRANSFORM postData;
	postData.matWorldMatrix = resultHitEffect.matHitEffect;

	wstring strBPartsEffectName = pNpcInfo->strHitBPartsEffectName[nBPartIndex - 1];
	if(resultHitEffect.matBloodEffect == MMatrix::IDENTITY)
		strBPartsEffectName += L"01";
	else
		strBPartsEffectName += L"02";

	XEffectInvoker effectInvoker;
	effectInvoker.Invoke(m_pOwner, strBPartsEffectName, wstring(), strBPartName, NULL, NULL, &postData);
}

void XModuleEffect::PlayDamageEffect( _DamageInfo * pInfo )
{
	wstring strSoundEffectName = GetDamageSoundName(pInfo);

	if ( global.sound && strSoundEffectName.empty() == false)
		global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(strSoundEffectName).c_str(), NULL, 0, m_pOwner->GetUID());

}

wstring XModuleEffect::GetDamageSoundName( _DamageInfo * pInfo )
{
	TALENT_RESIST_TYPE nResistType = TRT_NONE;
	XObject* pAttacker = gg.omgr->FindActor_UIID(pInfo->nAttackerUIID);
	XTalentInfo* pTalentInfo = XTalentHelper::FindTalentInfo(pInfo->nAttackedTalentID, pAttacker);
	if(pTalentInfo)
	{
		nResistType = pTalentInfo->m_Resist.m_nType;
	}

	if (pInfo->IsHitDodge())								return XCONST::SOUND_DODGE_EFFECT_NAME;
	if (CheckBitSet(pInfo->nFlags,CTR_RIPOSTE))				return XCONST::SOUND_RIPOSTE_EFFECT_NAME;
	if (CheckBitSet(pInfo->nFlags,CTR_RESIST_PERFECT) && nResistType != TRT_NONE)		return XCONST::SOUND_RESIST_PERFECT_EFFECT_NAME;
	if (CheckBitSet(pInfo->nFlags,CTR_RESIST_PARTIAL) && nResistType != TRT_NONE)		return XCONST::SOUND_RESIST_PARTIAL_EFFECT_NAME;
	if (CheckBitSet(pInfo->nFlags,CTR_DRAIN))				return XCONST::SOUND_DRAIN_EFFECT_NAME;
	if (CheckBitSet(pInfo->nFlags,CTR_IMMUNE))				return XCONST::SOUND_IMMUNE_EFFECT_NAME;
	if (CheckBitSet(pInfo->nFlags,CTR_MISS))				return XCONST::SOUND_MISS_EFFECT_NAME;
	if (CheckBitSet(pInfo->nFlags,CTR_AVOID))				return XCONST::SOUND_AVOID_EFFECT_NAME;

	return L"";
}

//////////////////////////////////////////////////////////////////////////
bool XEffectEventData::OnEffectEvent( XModuleEffect* pModuleEffect )
{
	PFC_THISFUNC;
	switch (m_nEffectEventID)
	{
	case XEFTEVT_EFFECT_MF_STUN:
		{
			if(pModuleEffect->m_pHeadEffect)
			{
				pModuleEffect->m_pHeadEffect->CreateHeadEffect(HET_STUN);
			}
		}
		break;
	case XEFTEVT_EFFECT_END_MF_STUN:
		{
			if(pModuleEffect->m_pHeadEffect)
			{
				pModuleEffect->m_pHeadEffect->DeleteHeadEffect(HET_STUN);
			}
		}
		break;
	case XEFTEVT_EFFECT_REBIRTH:
		{
			pModuleEffect->CreateRebirthEffect();
		}
		break;
	case XEFTEVT_EFFECT_NPC_ICON:
		{
			if(pModuleEffect->m_pNPCIConEffect)
			{
				if(pModuleEffect->GetOwner()->GetActor()->GetUsable() == false || 
					pModuleEffect->m_pEffectEvent->GetEffectEventIDCount(XEFTEVT_EFFECT_NPC_ICON) > 1)			// 이전 메세지들이 큐에 있다면 우선 처리를 위해서 이번 메세지도 큐에 넣는다.
				{
					// 1보다 커야 하는 이유는 기본적으로 이펙트 이벤트 리스트에 내꺼 하나가 들어 있기 때문이다.
					m_fRemainTime = 0.1f;
					return false;
				}

				NPC_ICON_STATE NIS = static_cast<NPC_ICON_STATE>(m_nParam1);
				pModuleEffect->m_pNPCIConEffect->SetNPCICon(pModuleEffect->GetOwner(), NIS);
			}
		}
		break;
	case XEFTEVT_EFFECT_SWORDTRAIL:
		{
			if(pModuleEffect->m_pSwordTrail)
			{
				pModuleEffect->m_pSwordTrail->Play(pModuleEffect->GetOwner());
			}
		}
		break;
	case XEFTEVT_EFFECT_END_SWORDTRAIL:
		{
			if(pModuleEffect->m_pSwordTrail)
			{
				pModuleEffect->m_pSwordTrail->Done();
			}
		}
		break;
	case XEFTEVT_EFFECT_PLAYER_ICON:
		{
			// 나중에 내용이 늘어나면 클래스로 따로 뺍니다.
			// 현재는 컷신 이펙트만 사용하고 있습니다.
			if(m_nParam1 == 1)
				pModuleEffect->CreateCutSceneEffect();
			else 
				pModuleEffect->DestroyPlayerIcon();

			break;
		}
	case XEFTEVT_EFFECT_LOOTABLE:
		{
			pModuleEffect->SetLootableEffectByIcon();
		}
		break;
	case XEFTEVT_EFFECT_LOOTABLE_DEL:
		{
			pModuleEffect->DeleteLootableEffect();
		}
		break;
	}		

	return true;
}

bool XBuffEffectEventData::OnEffectEvent( XModuleEffect * pOwner )
{
	switch (m_nEffectEventID)
	{
	case XEFTEVT_EFFECT_BUFF:
		{
			if(pOwner->m_pBuffEffect)
			{
				BUFF_EFFECT_SET_DATA data;
				data.nBuffID			= m_nParam1;
				data.nType				= m_eEffectType;
				data.bRemoveOncePlay	= m_bRemainBuffGain;
				data.nPartsSlotType		= m_nPartsSlotType;
				pOwner->m_pBuffEffect->AddBuffEffect(data);
			}
		}
		break;
	case XEFTEVT_EFFECT_BUFF_END:
		{
			if(pOwner->m_pBuffEffect)
			{
				pOwner->m_pBuffEffect->DelBuffEffect(m_nParam1, m_nPartsSlotType);
			}
		}
		break;
	case XEFTEVT_EFFECT_BUFF_DIE:
		{
			if(pOwner->m_pBuffEffect)
			{
				pOwner->m_pBuffEffect->DieBuffEffect(m_nParam1);
			}
		}
		break;
	case XEFTEVT_EFFECT_BUFF_CAPTION:
		{
			pOwner->ShowBuffCaptionEffect(m_strBuffName.c_str(), m_bIsDeBuff);
		}
		break;
	case XEFTEVT_EFFECT_BUFF_RESET:
		{
			pOwner->m_pBuffEffect->Init_ForChangeField();
		}
		break;
	case XEFTEVT_EFFECT_BUFF_ENCHANT:
		{
			pOwner->m_pBuffEffect->AddEnchantBuffEffect(m_nParam1, m_nPartsSlotType);
		}
		break;
	}

	return true;
}

bool XTalentEffectEventData::OnEffectEvent( XModuleEffect * pOwner )
{
	if(pOwner->m_pTalentEffect == NULL)
	{
		return true;
	}

	switch(m_nEffectEventID)
	{
	case XEFTEVT_EFFECT_TALENT:
		{
			CREATE_TALENT_EFFECT_DATA effectData;
			SetEffectData(effectData);
			pOwner->m_pTalentEffect->CreateByTalentEffectType(effectData);
		}
		break;
	case XEFTEVT_EFFECT_TALENT_DEL:
		{
			if(pOwner->m_pTalentEffect)
			{
				pOwner->m_pTalentEffect->ClearEffect();
			}
		}
		break;
	case XEFTEVT_EFFECT_TALENT_EFFECT_PRE_LOADER:
		{
			XTalentEffectPreLoader talentEffectPreLoader;
			talentEffectPreLoader.PreLoadTalentEffect(m_nParam1);
		}
		break;
	case XEFTEVT_EFFECT_TALENT_GUARD_DISABLE:
		{
			if(pOwner->m_pNPCIConEffect)
			{
				wstring strEffectName = ICON_EFFECT_GUARD_DISABLE;
				pOwner->m_pNPCIConEffect->CreateEffect(pOwner->GetOwner(), strEffectName, false);
			}
		}
		break;
	}

	return true;
}

bool XHitDamageEffectEventData::OnEffectEvent( XModuleEffect * pOwner )
{
	if(pOwner->m_pTalentEffect == NULL)
	{
		return true;
	}

	XObject* pAttacker = gg.omgr->Find(m_uidAttacker);
	XObject* pTarget = gg.omgr->Find(m_uidTarget);

	switch(m_nEffectEventID)
	{
	case XEFTEVT_EFFECT_SHOW_HIT_DAMAGE:
		{
			pOwner->ShowHitAndDamage(&m_Info, pAttacker, m_vOffset);
		}
		break;
	case XEFTEVT_EFFECT_SHOW_DAMAGE:
		{
			pOwner->ShowDamageCaptionEffect(&m_Info, m_vOffset);
		}
		break;
	case XEFTEVT_EFFECT_SHOW_PROJECTIL_DAMAGE:
		{
			pOwner->ShowEmptyProjrctileEffect(pTarget, pAttacker, m_pProjectileInfo, m_eColType);
		}
		break;
	}

	return true;
}

bool XCaptionEffectEventData::OnEffectEvent( XModuleEffect * pOwner )
{
	if(pOwner->m_pTalentEffect == NULL)
	{
		return true;
	}

	switch(m_nEffectEventID)
	{
	case XEFTEVT_EFFECT_CAPTION_HEAL:
		{
			pOwner->ShowHeal(m_nParam1, m_uiidUser);
		}
		break;
	case XEFTEVT_EFFECT_CAPTION_HP_REGEN:
		{
			pOwner->ShowHPRegen(m_nParam1);
		}
		break;	
	case XEFTEVT_EFFECT_CAPTION_GUARDDEFENSE:
		{
			pOwner->ShowGuardDefense(m_eGuardType, m_nParam1);
		}
		break;
	case XEFTEVT_EFFECT_CAPTION_DEBUG:
		{
			pOwner->ShowDebugCaptionEffect(m_strMessage.c_str(), m_color);
		}
		break;
	case XEFTEVT_EFFECT_CAPTION_EXP:
		{
			pOwner->ShowExpCaptionEffect(m_nParam1);
		}
		break;
	case XEFTEVT_EFFECT_CAPTION_EN:
		{
			pOwner->ShowEn(m_nParam1);
		}
		break;
	}


	return true;
}