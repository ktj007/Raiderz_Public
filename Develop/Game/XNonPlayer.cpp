#include "stdafx.h"
#include "XNonPlayer.h"
#include "XModuleMovable.h"
#include "XModuleEntity.h"
#include "XModuleAction.h"
#include "XModuleCollision.h"
#include "XModuleMotion.h"
#include "XModuleNonControl.h"
#include "XModuleBuff.h"
#include "XModuleNPCData.h"
#include "XModuleTalent.h"
#include "XGame.h"
#include "XWorld.h"
#include "XEffect.h"
#include "XModuleEffect.h"
#include "XNaming.h"
#include "XModuleBParts.h"
#include "XPost_Interaction.h"
#include "XModuleUI.h"
#include "XNPCInfo.h"
#include "XCharacter.h"
#include "XTalentInfoMgr.h"
#include "XFactionInfo.h"
#include "XFactionInfoMgr.h"
#include "CSCharHelper.h"
#include "CSNPCInfoHelper.h"
#include "XModuleSound.h"
#include "XNPCIcon.h"
#include "XMotionMgr.h"
#include "XItemManager.h"
#include "CSItemHelper.h"

#define STUN_ANIMATION_1			"MF_STUN"
#define SLEEP_ANIMATION				"sleep"

MImplementRTTI(XNonPlayer, XActor);

XNonPlayer::XNonPlayer(MUID uid, UIID nUIID) : XActor(uid, nUIID), m_pNPCInfo(NULL), m_nMode(0)
, m_bLootable(false)
, m_nAttackableType(NAT_NONE)
, m_bDeSpawn(false)
, m_bCombating(false)
, m_nNPCID(0)
, m_bHasCombatIdleAnimation(false)
, m_nLootableLastTime(0)
{
	m_nTypeID = ETID_NPC;

	m_eStunAnimation		= NPC_USE_IDLE;
	m_pModuleNonControl		= new XModuleNonControl(this);
	m_pModuleBParts = new XModuleBParts(this);
	//m_pModuleUI			= new XModuleUI(this);
	m_pModuleSound			= new XModuleSound(this);
	m_pModuleNPCData		= new XModuleNPCData(this);
	m_pNPCIcon				= new XNPCIcon(m_pModuleEffect);

	m_changeTexColor.Init(GetActor());
}

XNonPlayer::~XNonPlayer()
{
	m_pNPCInfo = NULL;
	SAFE_DELETE(m_pModuleNonControl);
	SAFE_DELETE(m_pModuleBParts);
	//SAFE_DELETE(m_pModuleUI);
	SAFE_DELETE(m_pModuleSound);
	SAFE_DELETE(m_pModuleNPCData);
	SAFE_DELETE(m_pNPCIcon);

	m_bLootable = false;
}

void XNonPlayer::RegisterModules()
{
	XActor::RegisterModules();

	m_pModuleNPCData->Initialize();
	m_Modules.AddModule(m_pModuleNPCData, true);

	m_pModuleNonControl->Initialize();
	m_Modules.AddModule(m_pModuleNonControl, true);

	m_pModuleBParts->Initialize();
	m_Modules.AddModule(m_pModuleBParts, true);

	m_pModuleSound->Initialize();
	m_Modules.AddModule(m_pModuleSound, true);
}

void XNonPlayer::UnregisterModules()
{
	m_Modules.RemoveModule(m_pModuleNonControl);
	m_pModuleNonControl->Finalize();

	m_Modules.RemoveModule(m_pModuleBParts);
	m_pModuleBParts->Finalize();

	m_Modules.RemoveModule(m_pModuleSound);
	m_pModuleSound->Finalize();

	m_Modules.RemoveModule(m_pModuleNPCData);
	m_pModuleNPCData->Finalize();

	XActor::UnregisterModules();
}

void XNonPlayer::OnUpdate(float fDelta)
{
	PFC_THISFUNC;

	UpdateMiniHpBar();
	CheckFighting();

	m_pNPCIcon->OnUpdate(fDelta);

	m_changeTexColor.Update(fDelta);

	XActor::OnUpdate(fDelta);
}

bool XNonPlayer::IsHuge()
{
	if (NULL == m_pNPCInfo)	return false;

	return (m_pNPCInfo->bHuge);
}

void XNonPlayer::InitNPC(XNPCInfo* pNPCInfo, TD_UPDATE_CACHE_NPC* pNPCCacheInfo)
{
	m_bDead = false;

	m_pNPCInfo = pNPCInfo;
	m_nNPCID = pNPCInfo->nID;

	m_pModuleEntity->SetScale(pNPCInfo->fScale);

	if (pNPCCacheInfo)
	{
		m_nLevel	= pNPCCacheInfo->nLevel;

		if(CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_LOOTABLE))
		{
			m_bLootable = true;
		}

		if(CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_DEAD))
		{
			m_bDead	= true;
		}
	}

	if(m_pModuleCollision)
	{
		m_pModuleCollision->InitHitCapsuleIndex();
	}
}

XModuleActorControl* XNonPlayer::GetModuleActorControl()
{ 
	return m_pModuleNonControl; 
}

void XNonPlayer::InNPC( XNPCInfo* pNPCInfo, TD_UPDATE_CACHE_NPC* pNPCCacheInfo, bool bAppearEffect /*= true*/, bool bLoadingAsync /*= true*/ )
{
	bool bMoving = false;
	m_bLoadingAsync = bLoadingAsync;

	if (GetEntityType() == ETID_NPC)
	{
		InitNPC(pNPCInfo, pNPCCacheInfo);
	}

	vec3 vStartDir;
	if (m_pModuleEntity)
	{
		m_pModuleEntity->SerializeNPC(pNPCInfo, m_bLoadingAsync);
	
		//  [6/25/2008 madduck]
		// TODO : 
		//UINotifier->>SetCharacterPaneNPC(pNPCInfo);

		// NPC 이름 색깔
		NPC_ATTACKABLE_TYPE nAttackableType = NAT_NONE;

		if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_ATTACKABLE_ALWAYS))
			nAttackableType = NAT_ALWAYS;
		else if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_ATTACKABLE_FACTION))
			nAttackableType = NAT_FACTION;

		SetAttackableType(nAttackableType);
		bool bAttackable = IsAttackable();
		bool bFirstAttackable = IsFisrtAttacker(pNPCInfo->nAA);
		SetNPCNameColor(bAttackable, bFirstAttackable);

		m_pModuleUI->SetCharacterPane_NPC(pNPCInfo->bShowName, pNPCInfo->GetName(), pNPCInfo->GetClan(), m_nLevel, IsMonster());

		vec3 pos = pNPCCacheInfo->vPos;

		if (CSNPCInfoHelper::IsUsingGravityNPC(pNPCInfo->nNpcType))
		{
			if ( gg.currentgamestate  &&  gg.currentgamestate->GetWorld())
				pos.z = gg.currentgamestate->GetWorld()->GetGroundZ(pos, NPC_PICKING_MAP_HEIGHT);	// 일단 서버에서 충돌처리가 없으므로..
		}
		else
		{
			if (m_pModuleMovable)
			{
				bool bUsingGravity = false;
				m_pModuleMovable->SetInitialFactors(bUsingGravity);
			}
		}

		// by pok, InGame
		if(m_pModuleNonControl)
		{
			// by pok, InGame
			// m_pModuleEntity->SetPosition(pos);

			//vec3 dir = pMonsterInfo->vTarPos - pMonsterInfo->vPos;
			vStartDir = pNPCCacheInfo->svDir;
			vStartDir.z = 0;
			if(vStartDir.LengthSq()<0.01f)	// TODO: 예외케이스, 없어져야한다.
			{
				vStartDir = RVector(0,1,0);
			}
			vStartDir.Normalize();

			m_pModuleNonControl->OnEnterGame(pos, vStartDir, this);
		}
	}



	// [2007/6/7 isnara] 파괴된 파츠 없애기
	if (m_pModuleBParts)
	{
		if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_BREAKPART1))		m_pModuleBParts->SetBrokenParts(1);
		if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_BREAKPART2))		m_pModuleBParts->SetBrokenParts(2);
		if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_BREAKPART3))		m_pModuleBParts->SetBrokenParts(3);
		if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_BREAKPART4))		m_pModuleBParts->SetBrokenParts(4);
	}
	
	if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_DEAD) && m_pModuleMotion)
	{
		m_bDead = true;
		DeadProcEnterField();

		if(m_pModuleMovable)
			m_pModuleMovable->TriggerDie(true);
	}
	if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_MODE1)) ChangeMode(NPC_MODE_1);
	else if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_MODE2)) ChangeMode(NPC_MODE_2);
	else if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_MODE3)) ChangeMode(NPC_MODE_3);
	else if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_MODE4)) ChangeMode(NPC_MODE_4);
	else if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_MODE5)) ChangeMode(NPC_MODE_5);
	

	if (CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_RUN) || 
		CheckBitSet(pNPCCacheInfo->nStatusFlag, UNS_WALK))
	{
		bMoving = true;
	}

	if (m_pModuleMotion)
	{
		m_pModuleMotion->ChangeMotion(MOTION_NAME_NPC_IDLE);
	}

	if (bMoving)
	{
		if (m_pModuleNonControl)
		{
			TD_NPC_MOVE npc_move;
			npc_move.nUIID = pNPCCacheInfo->nUIID;
			npc_move.vTarPos = pNPCCacheInfo->vTarPos;
			npc_move.svDir = vStartDir;
			npc_move.nSpeed = pNPCCacheInfo->nSpeed;

			if( npc_move.nSpeed != 0 || pNPCInfo->fRotateSpeed != 0.0f )
			{
				bool bRun = (CheckBitSet(pNPCCacheInfo->nStatusFlag,UNS_RUN) > 0) ? true : false;
				m_pModuleNonControl->OnNPCMove(&npc_move, bRun);
			}
		}
	}

	if (m_pModuleCollision)
	{
		m_pModuleCollision->InitCol(pNPCInfo->pMeshInfo);
	}

	// [12/3/2007 isnara] NPC STUN 모션 체크
	CheckAnimation();


	// NPC를 서서히 나타나게 한다.
	if(bAppearEffect)
	{
		m_pModuleNonControl->StartAppear();
	}

	// 버프
	if(m_pModuleBuff)
	{
		// 게임 처음 진입시 버프 입력
		for(int iBuff = 0; iBuff < MAX_OWN_BUFF_NUMBER; iBuff++)
		{
			m_pModuleBuff->SetRemainBuffList(pNPCCacheInfo->Buffs[iBuff], 0.0f);
		}
	}

	// 먹기라면...
	XTalentInfo* pTalentInfo = info.talent->Get(pNPCCacheInfo->nTalentID);
	if(pTalentInfo &&
		pTalentInfo->m_nExtraActive == TEAT_NPC_ONLY_EAT)
	{
		m_pModuleNonControl->OnSwallowedSynch(pNPCCacheInfo->nTalentID, pNPCCacheInfo->uidTarget);
	}

	// NPC 페이스 애니메이션
	if(m_pModuleEntity && m_pModuleEntity->GetActor())
	{
		wstring strMeshName = pNPCInfo->strMeshName;
		if(strMeshName.find(L"hf") != std::wstring::npos ||
			strMeshName.find(L"hm") != std::wstring::npos)
			m_pModuleEntity->CreateFaceAnimation(true);
	}

	// fake beaten 애니메이션 컨트롤러
	//if(m_pModuleEntity)
	//{
	//	float fWeight;
	//	tstring strBeatenAniName;

	//	if (m_pNPCInfo->pMeshInfo)
	//	{
	//		if (m_pNPCInfo->pMeshInfo->m_fReActionBlendWeight != 0.0f)
	//		{
	//			fWeight = m_pNPCInfo->pMeshInfo->m_fReActionBlendWeight;
	//		}
	//		else
	//		{
	//			if (pNPCInfo->bHuge) 
	//			{
	//				fWeight = XCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT;
	//			}
	//			else
	//			{
	//				fWeight = XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT;
	//			}
	//		}

	//		if (!m_pNPCInfo->pMeshInfo->m_strReActionAnimationName.empty())
	//		{
	//			strBeatenAniName = m_pNPCInfo->pMeshInfo->m_strReActionAnimationName;
	//		}
	//		else
	//		{
	//			strBeatenAniName = XCONST::HIT_FAKE_BEATEN_DEFAULT_ANI_NAME;
	//		}

	//		m_pModuleEntity->CreateFakeBeatenAnimationController(strBeatenAniName, fWeight);
	//	}
	//}
	if(m_pModuleEntity)
	{
		float fWeight;
		float fSpeed;
		tstring strBeatenAniName = XCONST::HIT_FAKE_BEATEN_DEFAULT_ANI_NAME;

		if (pNPCInfo->bHuge) 
		{
			fWeight = XCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT;
			fSpeed	= XCONST::HIT_HUGE_FAKE_BEATEN_ANIMATION_BLEND_SPEED;
		}
		else
		{
			fWeight = XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_WEIGHT;
			fSpeed	= XCONST::HIT_FAKE_BEATEN_ANIMATION_BLEND_SPEED;
		}

		m_pModuleEntity->CreateFakeBeatenAnimationController(strBeatenAniName, fWeight, fSpeed);
	}

	SetNPCIcon();
}

XNPCInfo* XNonPlayer::GetInfo(bool bCurrentMode)
{
	if (!bCurrentMode) return m_pNPCInfo;

	return GetInfo(m_nMode);
}

XNPCInfo* XNonPlayer::GetInfo( int nMode )
{
	if (m_pNPCInfo == NULL) return NULL;

	int nSize = m_pNPCInfo->vecModes.size();
	if(nMode > nSize)
		mlog("%s NPC Mode Error!!  curMode = %d  ModeCount = %d\n", MLocale::ConvUTF16ToAnsi(m_pNPCInfo->GetName()).c_str(), m_nMode, m_pNPCInfo->vecModes.size());
	else if (nMode > 0) 
		return static_cast<XNPCInfo*>(m_pNPCInfo->vecModes[nMode-1]);

	return m_pNPCInfo;
}

void XNonPlayer::ChangeMode( NPC_MODE nNextMode )
{
	if (m_nMode == nNextMode) return;

	// 텍스쳐 색깔 변경
	XNPCInfo* pCurrInfo = GetInfo(m_nMode);
	XNPCInfo* pNextInfo = GetInfo(nNextMode);
	if(pCurrInfo && pNextInfo &&
		pCurrInfo->m_bUseTexColor && pNextInfo->m_bUseTexColor)
	{
		CHANGE_NPC_COLOR npcColor;
		npcColor.colorTexSource = pCurrInfo->nTexColor;
		npcColor.colorTexTarget = pNextInfo->nTexColor;

		npcColor.colorSkinSource = pCurrInfo->nSkinColor;
		npcColor.colorSkinTarget = pNextInfo->nSkinColor;

		npcColor.colorHairSource = pCurrInfo->nHairColor;
		npcColor.colorHairTarget = pNextInfo->nHairColor;

		m_changeTexColor.Start(npcColor);
	}

	// 모드 변경
	m_nMode = nNextMode;

	CheckAnimation();

	// 히트 캡슐에 쓰는 모드 인덱스와 여기서 쓰이는 모드 인덱스가 다르다.
	if(m_pModuleCollision)
	{
		int nMode = 0;
		if(NPC_MODE_DEFAULT == nNextMode) nMode = 0;
		else if(NPC_MODE_1 == nNextMode) nMode = 1;
		else if(NPC_MODE_2 == nNextMode) nMode = 2;
		else if(NPC_MODE_3 == nNextMode) nMode = 3;
		else if(NPC_MODE_4 == nNextMode) nMode = 4;
		else if(NPC_MODE_5 == nNextMode) nMode = 5;

		m_pModuleCollision->SetHitCapsuleByNPCMode(nMode);
	}

	if (m_pModuleUI)
	{
		XNPCInfo* pNPCInfo = GetInfo(true);

		m_pModuleUI->SetCharacterPane_NPC(pNPCInfo->bShowName, pNPCInfo->GetName(), pNPCInfo->GetClan(), m_nLevel, IsMonster());
	}
}

bool XNonPlayer::IsInteract()
{
	if (GetInfo())
	{
		return GetInfo()->bInteract;
	}
	return false;
}

bool XNonPlayer::IsMonster()
{
	if (GetInfo())
	{
		return (GetInfo()->nNpcType == NPC_TYPE_MONSTER);
	}
	return true;
}

void XNonPlayer::CheckStunAnimation()
{
	if(m_pModuleEntity)
	{
		RActor * pActor = m_pModuleEntity->GetActor();
		if(pActor)
		{
			// 1. 모션에서 찾아서 애니메이션이 있으면 스턴 애니메이션으로
			XMotion* pMotion = info.motion->GetMotion(MOTION_NAME_NPC_MF_STUN);
			if(pMotion)
			{
				XMotionAniSequence * pAni = pMotion->GetAniSequence(MT_DEFAULT, 0);
				wstring strAniName = GetModeAnimationNameHeader() + pAni->GetAniName();

				RAnimation* pStunAnimation = pActor->GetAnimation(MLocale::ConvUTF16ToAnsi(strAniName.c_str()).c_str());
				// 첫번째 애니메이션이 존재한다면...
				if(pStunAnimation)
				{
					m_eStunAnimation = NPC_USE_STUN;
					return;
				}
			}

			// 2. 1번 애니메이션이 안돼면... sleep 애니메이션으로 
			RAnimation* pSleepAnimation = pActor->GetAnimation(SLEEP_ANIMATION);
			if(pSleepAnimation != NULL)
			{
				m_eStunAnimation = NPC_USE_SLEEP;
				return;
			}

			// 3. 모든 애니메이션이 없다면 IDLE로 교체
			m_eStunAnimation = NPC_USE_IDLE;
		}
	}
}

void XNonPlayer::CheckCombatIdleAnimation()
{
	if(m_pModuleEntity)
	{
		RActor * pActor = m_pModuleEntity->GetActor();
		if(pActor)
		{
			wstring strAniName = GetModeAnimationNameHeader() + ANIM_NAME_IDLE_COMBAT;

			if (pActor->GetAnimation(MLocale::ConvUTF16ToAnsi(strAniName.c_str()).c_str()) != NULL)
			{
				m_bHasCombatIdleAnimation = true;
				return;
			}
		}
	}

	m_bHasCombatIdleAnimation = false;
}

wstring XNonPlayer::GetStunMotionAnimation()
{
	return XNaming::StunMotionAnimation(m_eStunAnimation);
}

wstring XNonPlayer::GetModeAnimationNameHeader()
{
	return XNaming::ModeAnimationNameHeader(GetMode(), GetInfo(true));
}

void XNonPlayer::SetLootable( bool bLootable )
{
	if (m_bLootable == bLootable) return;

	m_bLootable = bLootable;

	if (bLootable == false)
	{
		m_pModuleEffect->OnEffectEvent(XEFTEVT_EFFECT_LOOTABLE_DEL);
	}

	m_pModuleNonControl->OnLootable(bLootable);

	m_nLootableLastTime = XGetNowTime();
}

bool XNonPlayer::IsLootable()
{
	return m_bLootable;
}

bool XNonPlayer::IsLootableRecently()
{
	if (!m_bLootable)
	{
		uint32 nNowTime = XGetNowTime();
		if (nNowTime - m_nLootableLastTime <= 30000)		// 루팅가능한지 30초이상 지났으면 최근에 루팅 가능했음
		{
			return true;
		}
	}

	return m_bLootable;
}

void XNonPlayer::SetNPCNameColor( bool bAttackable, bool bFirstAttackable )
{
	if(bAttackable)
	{
		if(bFirstAttackable)
			m_dwNameColor = MONSTER_ID_COLOR_FIRST_ATTACKER;
		else
			m_dwNameColor = MONSTER_ID_COLOR_NOT_FIRST_ATTACKER;
	}
	else
	{
		m_dwNameColor = QUEST_NPC_ID_COLOR;
	}

	m_pModuleUI->SetNameColor(m_dwNameColor);
}
//void XNonPlayer::CheckInteractionable()
//{
//	if( m_pNPCInfo->bInteract && IsModuleEffect())
//		GetModuleEffect()->Targeted(TARGET_INTERACTIVE_DISABLE);
//}

bool XNonPlayer::IsAttackable()
{
	if ( GetInfo()->nID == DUEL_FLAG_NPCID)
		return false;

	switch ( m_nAttackableType)
	{
	case NAT_NONE :
		return false;

	case NAT_ALWAYS :
		return true;

	case NAT_FACTION :
		{
			FACTION_RELATION_TYPE nFRT = GetFactionType();
			return ( nFRT <= FRT_BAD)  ?  true : false;
		}

	default :
		return false;
	}

	return false;
}


void XNonPlayer::SetDirForwardToActor( XObject* pObject )
{
	if (pObject == NULL) return;

	SetFacingDir(GetDirection());

	XNPCInfo* pNPCInfo = GetInfo();
	if (pNPCInfo->nNpcType == NPC_TYPE_OBJECT) return;

	vec3 vDir = pObject->GetPosition() - GetPosition();
	vDir.Normalize();
	vDir.z = 0.f;

	SetDirection(vDir);
}

NPC_ICON_STATE XNonPlayer::GetIConState() const
{
	return m_pNPCIcon->GetNPCIConState();
}

bool XNonPlayer::OnCreate()
{
	// 사용 할 이펙트 종류 등록
	if(m_pModuleEffect)
	{
		m_pModuleEffect->SetRegisterUseEffectID((1 << XEFT_ID_HEAD) | (1 << XEFT_ID_BUFF) | ( 1<< XEFT_ID_NPCICON) | (1 << XEFT_ID_TALENT) | (1 << XEFT_ID_SWORDTRAIL));
	}

	return true;
}

const wchar_t* XNonPlayer::GetName()
{
	if (m_pNPCInfo) return m_pNPCInfo->GetName();
	
	return L"";
}

bool XNonPlayer::IsColliable()
{
	if(XActor::IsColliable() == false)
		return false;

	if(GetEntityType() == ETID_NPC)
	{
		// 충돌 안합니다.
		if(GetColTestObject() == false)
		{
			return false;
		}
	}

	if (m_pNPCInfo)
	{
		if (CSAltitudeHelper::IsColliable(m_pNPCInfo->nAltitude) == false)
		{
			return false;
		}
	}

	return true;
}

// bool XNonPlayer::IsEnemy(NPC_ATTACKABLE_TYPE nAttackableType)
// {
// 	if (DUEL_FLAG_NPCID == GetInfo()->nID)
// 	{
// 		return false;
// 	}
// 
// 	if (NAT_ALWAYS == nAttackableType)
// 	{
// 		return true;
// 	}
// 
// 	if (NAT_FACTION == nAttackableType)
// 	{
// 		FACTION_RELATION_TYPE nFRT = GetFactionType();
// 		if (FRT_BAD >= nFRT)
// 		{
// 			return true;
// 		}
// 	}
// 
// 	return false;
// }

float XNonPlayer::GetValidInteractionDistance()
{
	return CSCharHelper::CalcValidInteractionDistance(m_pModuleCollision->GetRadius(), GetInfo()->nInteractionDistance);
}

bool XNonPlayer::IsFisrtAttacker( NPC_AA_TYPE nAAType )
{
	if(nAAType == NAAT_ALWAYS)
		return true;

	if(nAAType == NAAT_FACTION)
	{
		FACTION_RELATION_TYPE nFRT = GetFactionType();
		if (FRT_BAD >= nFRT)
		{
			return true;
		}
	}

	return false;
}

void XNonPlayer::CheckFighting()
{
	if(m_pModuleUI &&
		m_bDead == false)
	{
		if(IsCombating())
		{
			// 싸운중이면...
			m_pModuleUI->SetNameColor(MONSTER_ID_COLOR_FIGHTING);
		}
		else if ( m_nAttackableType == NAT_FACTION)
		{
			m_pModuleUI->SetNameColor( IsAttackable() ? MONSTER_ID_COLOR_FIRST_ATTACKER : QUEST_NPC_ID_COLOR);
		}
		else
		{
			m_pModuleUI->SetNameColor(m_dwNameColor);
		}
	}
}

void XNonPlayer::UpdateMiniHpBar()
{
	
}

bool XNonPlayer::GetColTestObject()
{
	XNPCInfo* pNPCInfo = GetInfo(true);

	// 정보가 없으면 그냥 통과 시켜용
	if(pNPCInfo == NULL)
		return true;

	return pNPCInfo->m_bColTestObject;
}

bool XNonPlayer::Reload()
{
	m_pNPCInfo = info.npc->Get(m_nNPCID);

	m_pModuleTalent->Cancel();

	if (GetModuleBParts())
	{
		GetModuleBParts()->ClearForReload();
	}

	m_pModuleEntity->SerializeNPC(m_pNPCInfo, false);

	XObject::Reload();
	
	return true;
}

void XNonPlayer::SetNPCIConTypeToIconState( NPC_ICON_TYPE NIT )
{
	m_pNPCIcon->SetNPCIConTypeToIconState(NIT);
}

void XNonPlayer::SetNPCIConState( NPC_ICON_STATE NIS )
{
	m_pNPCIcon->SetNPCIConState(NIS);
}

void XNonPlayer::SetPreviousNPCIcon()
{
	m_pNPCIcon->SetPreviousNPCIcon();
}

void XNonPlayer::SetNPCIcon()
{
	map<UIID, NPC_ICON_INFO>::iterator itor = gvar.Game.vecNPCIcon.find(m_nUIID);
	if (itor != gvar.Game.vecNPCIcon.end())
	{
		NPC_ICON_INFO tdNPCIcon = itor->second;
		SetNPCIConTypeToIconState(tdNPCIcon.m_nIcon);
	}
}

FACTION_RELATION_TYPE XNonPlayer::GetFactionType()
{
	FACTION_RELATION_TYPE nFRT = gvar.MyInfo.FactionInfo.GetRelation(GetInfo()->m_nFactionLossID);
	if (FRT_NONE == nFRT)
	{
		XFactionInfo* pFactionInfo = info.faction->Get(GetInfo()->m_nFactionLossID);
		if (NULL != pFactionInfo)
		{
			nFRT = pFactionInfo->m_nDefaultFRT;
		}
	}

	return nFRT;
}

float XNonPlayer::GetBallonLifeTime( float fMaintainTime, wstring &szChat )
{
	return GetModuleUI()->GetBallonLifeTime(fMaintainTime, szChat);
}

void XNonPlayer::DestroyNPCIconEffect()
{
	m_pModuleEffect->DestroyNPCIconEffect();
}

void XNonPlayer::CheckAnimation()
{
	CheckStunAnimation();
	CheckCombatIdleAnimation();
}

bool XNonPlayer::GetUseDieAni()
{
	if(m_pNPCInfo == NULL)
		return false;

	return m_pNPCInfo->bUseDieAni;
}

WEAPON_TYPE XNonPlayer::GetCurrWeaponType()
{
	XNPCInfo* pNPCInfo = GetInfo(true);
	if(pNPCInfo == NULL ||
		(pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON] == 0 && pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_RWEAPON] == 0))
		return WEAPON_NONE;

	CSItemData* pItemDataR = NULL;
	CSItemData* pItemDataL = NULL;

	if (pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON] != 0) 
		pItemDataL = info.item->GetItemData(pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON]);
	if (pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_RWEAPON] != 0) 
		pItemDataR = info.item->GetItemData(pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_RWEAPON]);

	return CSItemHelper::GetCurrentWeaponType(pItemDataR, pItemDataL);
}

WEAPON_TYPE XNonPlayer::GetEquipWeaponType( bool bLeft )
{
	XNPCInfo* pNPCInfo = GetInfo(true);
	if(pNPCInfo == NULL ||
		(pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON] == 0 && pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_RWEAPON] == 0))
		return WEAPON_NONE;

	CSItemData* pItemDataR = NULL;
	CSItemData* pItemDataL = NULL;

	if (pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON] != 0) 
		pItemDataL = info.item->GetItemData(pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON]);
	if (pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_RWEAPON] != 0) 
		pItemDataR = info.item->GetItemData(pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_RWEAPON]);

	if(bLeft && pItemDataL)
		return pItemDataL->m_nWeaponType;
	else if(pItemDataR)
		return pItemDataR->m_nWeaponType;

	return WEAPON_NONE;
}

bool XNonPlayer::IsEquipShield()
{
	XNPCInfo* pNPCInfo = GetInfo(true);
	if(pNPCInfo == NULL ||
		(pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON] == 0))
		return false;

	CSItemData* pItemDataL = NULL;

	if (pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON] != 0) 
		pItemDataL = info.item->GetItemData(pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON]);

	if(pItemDataL)
		return (CSItemHelper::IsShieldItemType(pItemDataL));

	return false;
}

bool XNonPlayer::IsShowAggro()
{
	XNPCInfo* pNpcInfo = GetInfo(true);
	if(pNpcInfo && pNpcInfo->bShowAggroInfo)
	{
		return true;
	}

	return false;
}

void XNonPlayer::GetNPCWeaponItemData( CSItemData** pItemDataR, CSItemData** pItemDataL )
{
	XNPCInfo* pNPCInfo = GetInfo(true);
	if(pNPCInfo == NULL)
		return;

	CSItemData* pItemR = NULL;
	CSItemData* pItemL = NULL;

	if (pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON] != 0) 
		pItemL = info.item->GetItemData(pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_LWEAPON]);
	if (pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_RWEAPON] != 0) 
		pItemR = info.item->GetItemData(pNPCInfo->nPartsItemID[NPC_PARTS_SLOT_RWEAPON]);

	*pItemDataR = pItemR;
	*pItemDataL = pItemL;
}

void XNonPlayer::Despawn()
{
	if(m_pModuleNonControl)
	{
		m_pModuleNonControl->OnDespawn();
	}
}
