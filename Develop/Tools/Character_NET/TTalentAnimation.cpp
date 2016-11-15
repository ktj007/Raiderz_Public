#include "stdafx.h"
#include "TTalentAnimation.h"
#include "TEffectCharacterToolInvoker.h"
#include "TEffectCharacterToolSinglyInvoker.h"
#include "TAnimationHelper.h"
#include "XMotionMgr.h"
#include "TTalentEffectMgr.h"
#include "RRenderHelper.h"

TTalentAnimation::TTalentAnimation()
: m_pActor(NULL)
, m_fPostDelayRemainTime(0.0f)
{
	m_pMotion = NULL;
	m_checkDummyLocResult = CDLR_NO_ANIMATION;
	m_bShowHitEffect = false;
	m_nTalentAnimationState = TAS_STOP;

	InitTalentAnimation();
}

TTalentAnimation::~TTalentAnimation()
{

}

bool TTalentAnimation::SetTalentAnimation( CSTalentInfo* pTalentInfo, int nMode )
{
	InitTalentEvent();
	DelTalentEffect(true);
	InitDummyLocPos();
	g_pMainApp->m_MagicArea.OnDestroy();

	m_pTalentInfo = static_cast<TTalentInfo*>(pTalentInfo);

	g_pMainApp->m_pEffectManager->Clear();

	m_nCurrentTalentID = pTalentInfo->m_nID;
	m_nCurrentTalentMode = nMode;

	m_nTalentAnimationState = TAS_PLAY;

	return Play();
}

void TTalentAnimation::TalentAnimationStop()
{
	InitTalentEvent();
	DelTalentEffect();
	InitDummyLocPos();

	// 정지
	Stop();

	m_nTalentAnimationState = TAS_STOP;
}

void TTalentAnimation::Init( TPlayMotion* pMotion )
{
	m_pMotion = pMotion;

	m_TalentEvent.InitTalentEventHandler();
}

void TTalentAnimation::Update( float fDelta )
{
	CheckTalentAnimation(fDelta);

	m_TalentEvent.EventUpdate(fDelta);
	m_TalentEffectHitCapsule.Update(fDelta, m_pActor);

	if(m_bShowHitEffect &&
		m_TalentHitEffectDirTest.Update(fDelta))
		ShowTalentEffect(ATTACK_EFFECT);
}

bool TTalentAnimation::TalentAnimationPlay(bool bPrePause)
{
	m_TalentEvent.SetPause(false);
	m_nTalentAnimationState = TAS_PLAY;

	if(bPrePause == false)
	{
		g_pMainApp->m_MagicArea.OnDestroy();

		m_TalentEvent.SetActor(m_pActor);
		m_TalentEvent.SetTalent(m_pTalentInfo);
		m_TalentEvent.EventsStart(&m_pTalentInfo->m_Events);
	}
	else 
	{
		m_bUseAct			= false;
		m_bCastingLoop		= false;
		m_bCasting			= false;
		m_nCurAniIndex		= 0;

		g_pMainApp->UpdateModelScale();
		m_pActor->PlayAnimation();
		return true;
	}

	return Play();
}

bool TTalentAnimation::PlayTalentAnimation()
{
	if (m_pActor == NULL) return false;
	if (m_pTalentInfo == NULL) return false;

	RMatrix mat;
	mat.MakeIdentity();
	m_pActor->SetTransform(mat);
	g_pMainApp->m_MagicArea.OnDestroy();

	m_pActor->InitMeshNodeNoiseRef();

	// npc scale
	g_pMainApp->UpdateModelScale();

	// 초기화
	m_bCasting			= false;
	m_bCastingLoop		= false;
	m_bUseAct			= false;
	m_nUseActAniIndex	= 0;
	m_nCurAniIndex		= 0;

	string cstrCasting = m_pTalentInfo->m_szCastingAnimation;
	string cstrCastingLoop = m_pTalentInfo->m_szCastingLoopAnimation;
	string cstrUse = m_pTalentInfo->m_szUseAnimation;
	if (!m_pTalentInfo->IsPassiveSkill() && cstrUse.empty())
	{
		mlog("에러 : TalentID = %d : UseAnimation 명이 없습니다.\r", m_pTalentInfo->m_nID);
		return false;
	}

	// 체크 애니메이션
	if(CheckExistTalentAnimation(m_pTalentInfo) == false)
	{
		return false;
	}

	m_TalentHitEffectDirTest.SetData(m_pTalentInfo);

	bool bCasting = false;
	bool bNormalStance = CheckTalentNomalStance();
	//////////////////////////////////////////////////////////////////////////
	// 캐스팅
	if(cstrCasting.empty() == false)
	{
		if(XMotionMgr::IsTalentMotionName(cstrCasting.c_str()))
		{
			if(m_pMotion->ChangeMotion(cstrCasting.c_str(), MT_DEFAULT, true))
			{
				bCasting = true;
				m_nUseActAniIndex += GetAniCount(cstrCasting);
			}
		}
		else
		{
			m_pMotion->ChangeCustomMotion(MOTION_NAME_TALENT_CASTING, cstrCasting.c_str(), MT_DEFAULT, true, bNormalStance);
			m_nUseActAniIndex += 1;
			bCasting = true;
		}

		//////////////////////////////////////////////////////////////////////////
		// 캐스팅 Loop
		if(cstrCastingLoop.empty() == false)
		{
			if(XMotionMgr::IsTalentMotionName(cstrCastingLoop.c_str()))
			{
				m_pMotion->PushMotion(cstrCastingLoop.c_str(), true);
				m_nUseActAniIndex += GetAniCount(cstrCastingLoop);
			}
			else
			{
				m_pMotion->PushCustomMotion(MOTION_NAME_TALENT_CASTING_LOOP, cstrCastingLoop.c_str(), true, bNormalStance);
				m_nUseActAniIndex += 1;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 유즈, Act
	if (XMotionMgr::IsTalentMotionName(cstrUse.c_str()))
	{
		string strMotionName = cstrUse;
		string strReturnMotionName;

		int fndpos = cstrUse.find(";");
		if (fndpos != string::npos)
		{
			strMotionName = cstrUse.substr(0, fndpos);
			strReturnMotionName = cstrUse.substr(fndpos+1);
		}

		if(bCasting)
		{
			m_pMotion->PushMotion(strMotionName.c_str());
		}
		else
		{
			m_pMotion->ChangeMotion(strMotionName.c_str());
		}

		if(strReturnMotionName.empty() == false)
			m_pMotion->PushMotion(strReturnMotionName.c_str());
	}
	else
	{
		if(bCasting)
		{
			m_pMotion->PushCustomMotion(MOTION_NAME_TALENT_USE, cstrUse.c_str(), false, bNormalStance);
		}
		else
		{
			if ( m_pMotion->ChangeCustomMotion(MOTION_NAME_TALENT_USE, cstrUse.c_str(), MT_DEFAULT, false, bNormalStance) == false)
			{
				return false;
			}
		}
	}

	if(m_pMotion->CheckExistCurAnimation() == false)
	{
		m_pMotion->CurMotionClear();
		return false;
	}

	m_pMotion->CalAnimationDummyLocLastMatrixList(m_pActor, m_vecDummyLocLast);

	CheckDummyLoc();

	if(g_pMainApp->m_pEffectManager)
		g_pMainApp->m_pEffectManager->StopAnimationEffect();

	return true;
}

void TTalentAnimation::CheckTalentAnimation( float fDelta )
{
	if (m_pTalentInfo == NULL) 
		return;

	// Act 애니메이션인지 체크하자.
	string cstrCasting = m_pTalentInfo->m_szCastingAnimation;
	string cstrCastingLoop = m_pTalentInfo->m_szCastingLoopAnimation;
	string cstrUse = m_pTalentInfo->m_szUseAnimation;

	if(CheckTalentAnimationEnd() &&
		m_fPostDelayRemainTime > 0.0f && m_nTalentAnimationState != TAS_STOP)
	{
		if(m_nTalentAnimationState == TAS_PLAY)
		{
			m_fPostDelayRemainTime -= fDelta;

			if(m_fPostDelayRemainTime <= 0.0f)
			{
				m_fPostDelayRemainTime = 0.0f;
				TalentAnimationPause();
			}

			if(m_bUseAct == false)
			{
				ShowTalentEffect(USE_EFFECT);
				m_bUseAct = true;
			}
		}

		return;
	}

	if(m_pMotion->GetMotion())
	{
		if(m_pActor->GetAnimationState() != RPS_PAUSE)
		{
			if(cstrCasting.empty() == false)
			{
				// 캐스팅 애니메이션이 있다면... Use가 Act 시점이다.
				if(m_bCasting == false)
				{
					DelTalentEffect();

					//////////////////////////////////////////////////////////////////////////
					// 캐스팅 이펙트 발동
					ShowTalentEffect(CAST_EFFECT);

					m_bCasting = true;
				}

				// 캐스팅 완료 이펙트
				if(m_pTalentInfo->m_fCastingTime > 0.0f &&
					m_bCastingLoop == false &&
					1 == m_pMotion->GetCurAniSequence())
				{
					DelTalentEffect();
					ShowTalentEffect(CAST_END_EFFECT);

					m_bCastingLoop = true;
				}
			}

			if(cstrUse.empty() == false)
			{
				if(m_bUseAct == false &&
					m_nUseActAniIndex <= m_pMotion->GetCurAniSequence())
				{
					// 캐스팅 이펙트 삭제
					DelTalentEffect();

					// Act이다.
					m_TalentEvent.SetActStart(true);

					ShowTalentEffect(USE_EFFECT);

					m_bUseAct = true;
				}
			}
		}
		else if(m_pActor->GetAnimationState() == RPS_PAUSE)
		{
			// 일시정지에서는 현재 상태를 체크해서 캐스팅과 Act를 설정한다.
			// 일시정지는 프레임 슬라이드를 움직였거나 일시정지 버튼을 누른것이다.
			if(m_nUseActAniIndex > m_pMotion->GetCurAniSequence())
			{
				if(m_bUseAct)
				{
					m_bCasting = false;
					m_bCastingLoop = false;
				}

				m_bUseAct = false;

				// 이벤트 재설정
				m_TalentEvent.SetActStart(false);
			}
			else
			{
				if(m_bCasting)
					DelTalentEffect();

				m_bCasting = true;
				m_bCastingLoop = true;
				m_bUseAct = true;

				m_TalentEvent.SetActStart(true);
			}

			m_TalentEvent.SetTalentEventTime(m_pMotion->GetCurTalentFrametime(m_nUseActAniIndex), m_pMotion->GetCurTalentCurFrametime());
		}
	}
}

void TTalentAnimation::TalentAnimationPause()
{
	m_TalentEvent.SetPause(true);
	m_pActor->PauseAnimation();

	m_nTalentAnimationState = TAS_PAUSE;
}

void TTalentAnimation::ShowTalentEffect( TALENT_EFFECT_TYPE nType )
{
	XTalentEffectDataMgr* pEffectMgr = m_pTalentInfo->GetEffectDataMgr();
	if(pEffectMgr)
	{
		// 레퍼런스 탤런트 이펙트
		if(pEffectMgr->m_nRefID > 0)
		{
			TTalentInfo* pRefTalent = TTalentHelper::GetTalentInfo(pEffectMgr->m_nRefID, 0);
			if(pRefTalent)
			{
				pEffectMgr = pRefTalent->GetEffectDataMgr();
			}
		}

		vector<XTalentEffectInfo *> vecEffectInfoByType;
		pEffectMgr->Get(nType, vecEffectInfoByType);

		if(nType == ATTACK_EFFECT)
		{
			ShowTalentHitEffect(pEffectMgr, &vecEffectInfoByType);
		}
		else
		{
			for(vector<XTalentEffectInfo *>::iterator itEffect = vecEffectInfoByType.begin(); itEffect != vecEffectInfoByType.end(); itEffect++)
			{
				// 판정 캡슐이라면 걸러낸다.
				if((*itEffect)->m_nImpactPos == TALENT_HIT_CAPSULE_POS)
				{
					m_TalentEffectHitCapsule.AddTalentHitCapsuleImpact(m_pActor, m_pTalentInfo, (*itEffect), nType);
				}
				else
				{
					TEffectCharacterToolInvoker effectInvoker;
					effectInvoker.Invoke(m_pActor, (*itEffect)->m_strEffectName, (*itEffect)->m_strParameter, (*itEffect)->m_strBoneName, &m_vecTalentEffectResult);
				}
			}
		}
	}
}

void TTalentAnimation::DelTalentEffect( bool bImmediatlyDelete /*= false*/ )
{
	vector<EFFECT_RESULT_DATA>::iterator itTalentEffect = m_vecTalentEffectResult.begin();
	while(itTalentEffect != m_vecTalentEffectResult.end())
	{
		if((*itTalentEffect).type == CET_MODEL)
		{
			g_pMainApp->m_pEffectManager->DeleteTEffect((*itTalentEffect).uidEffect, bImmediatlyDelete);
		}
		if((*itTalentEffect).type == CET_SOUND)
		{
			g_pMainApp->m_pSoundMgr.StopDynamic3D((*itTalentEffect).uidEffect);
		}

		itTalentEffect++;
	}

	m_vecTalentEffectResult.clear();
}

void TTalentAnimation::Stop()
{
	Play();

	m_pActor->PauseAnimation();
}

int TTalentAnimation::GetAniCount( string strMotionName )
{
	XMotion* pMotion = m_pMotion->GetMotionMgr()->GetMotion(strMotionName.c_str());
	if(pMotion == NULL)
		return 0;

	int nAniCount = pMotion->m_vecAniSequence[m_pMotion->GetCurMotionType()].size();

	if(pMotion->IsCustom())
	{
		nAniCount = 1;
	}

	return nAniCount;
}

void TTalentAnimation::TalentAnimationEnd()
{
	InitTalentEvent();
	DelTalentEffect();
	InitDummyLocPos();

	m_pMotion->CurMotionClear();

	m_nTalentAnimationState = TAS_STOP;
}

void TTalentAnimation::InitTalentEvent()
{
	m_TalentEvent.SetActor(NULL);
	m_TalentEvent.SetTalent(NULL);
	m_TalentEvent.EventsEnd();
	m_TalentEvent.SetActStart(false);
}

bool TTalentAnimation::CalAnimaitonPos()
{
	return TAnimationHelper::CalAnimaitonPos(m_pActor, m_pMotion, m_vecDummyLocLast);
}

void TTalentAnimation::InitDummyLocPos()
{
	m_vecDummyLocLast.clear();
}

void TTalentAnimation::InitTalentAnimation()
{
	m_pTalentInfo		= NULL;
	m_bUseAct			= false;
	m_bCastingLoop		= false;
	m_bCasting			= false;
	m_nUseActAniIndex	= 0;
	m_nCurAniIndex		= 0;

	m_nCurrentTalentID	= -1;
	m_nCurrentTalentMode= -1;

	m_checkDummyLocResult = CDLR_NO_ANIMATION;

	m_vDummyLocPos		= vec3::ZERO;
	m_vDummyLocDir		= vec3::AXISY;

	InitTalentEvent();

	if(g_pMainApp)
		g_pMainApp->DelProjectile();

	if(m_pMotion)
		m_pMotion->CurMotionClear();

	if(g_pMainApp && g_pMainApp->m_pEffectManager)
		g_pMainApp->m_pEffectManager->StopAnimationEffect();

	m_TalentHitEffectDirTest.SetData(m_pTalentInfo);
}

void TTalentAnimation::CheckDummyLoc()
{
	CHECK_DUMMY_LOC_RESULT checkDummyLocResult = CDLR_NO_ANIMATION;

	for(vector<XMotionAniSequence*>::iterator itAni = m_pMotion->GetMotion()->m_vecAniSequence[m_pMotion->GetCurMotionType()].begin();
		itAni != m_pMotion->GetMotion()->m_vecAniSequence[m_pMotion->GetCurMotionType()].end();
		itAni++)
	{
		CHECK_DUMMY_LOC_RESULT result = g_pMainApp->m_TabAnimation.CheckAnimationDummyLocNode((*itAni)->GetAniName());
		if(result == CDLR_NOT_USE)
			checkDummyLocResult = result;
		else if(result == CDLR_OK && checkDummyLocResult != CDLR_NOT_USE)
			checkDummyLocResult = result;
	}

	m_checkDummyLocResult = checkDummyLocResult;
}

bool TTalentAnimation::CheckTalentNomalStance()
{
	// 전투상태가 아니라면 애니메이션 접두어는 none 이다.
	if (CheckBitSet(m_pTalentInfo->m_nEnabledStanceFlag, CS_NORMAL) &&
		!CheckBitSet(m_pTalentInfo->m_nEnabledStanceFlag, CS_BATTLE))
	{
		return true;
	}

	return false;
}

bool TTalentAnimation::CheckExistTalentAnimation( TTalentInfo* pTalentInfo )
{
	WEAPON_TYPE weapon_type = (WEAPON_TYPE)m_pMotion->GetWeaponType();
	bool bNoExist = false;
	bool bAutoPrefix = false;
	if(m_pMotion->GetMotion())
		m_pMotion->GetMotion()->IsAutoPrefix();

	bNoExist |= !CheckExistTalentAnimationByAniType(pTalentInfo, TAT_CASTING, weapon_type, bAutoPrefix);
	bNoExist |= !CheckExistTalentAnimationByAniType(pTalentInfo, TAT_CASTINGLOOP, weapon_type, bAutoPrefix);
	bNoExist |= !CheckExistTalentAnimationByAniType(pTalentInfo, TAT_USE, weapon_type, bAutoPrefix);
	bNoExist |= !CheckExistTalentAnimationByAniType(pTalentInfo, TAT_EXTRA, weapon_type, bAutoPrefix);
	bNoExist |= !CheckExistTalentAnimationByAniType(pTalentInfo, TAT_EXTRA2, weapon_type, bAutoPrefix);
	bNoExist |= !CheckExistTalentAnimationByAniType(pTalentInfo, TAT_EXTRA3, weapon_type, bAutoPrefix);

	return !bNoExist;
}

bool TTalentAnimation::CheckExistTalentAnimationByAniType( TTalentInfo* pTalentInfo, TALENT_ANIMATION_TYPE eAniType, WEAPON_TYPE eWeaponType, bool bAutoPrefix )
{
	if(pTalentInfo == NULL)
		return false;

	bool bExist = true;

	TAnimationHelper talentAnimationHelper;
	TALENT_ANIMATION_INFO talentAniInfo;

	talentAniInfo = talentAnimationHelper.GetTalentAnimationInfo(pTalentInfo, eAniType);

	for(vector<WEAPON_ANIMATION_INFO>::iterator it_aniInfo = talentAniInfo.vecTalentAniInfo.begin(); it_aniInfo != talentAniInfo.vecTalentAniInfo.end(); ++it_aniInfo)
	{
		if(it_aniInfo->eWeaponType != eWeaponType)
			continue;

		for(vector<ANIMATION_INFO>::iterator it_ani = it_aniInfo->vecAniInfo.begin(); it_ani != it_aniInfo->vecAniInfo.end(); ++it_ani)
		{
			RAnimation* pAnimation = m_pActor->GetAnimation(it_ani->strAnimationName.c_str());
			if( pAnimation == NULL)
			{
				bool bModeCheck = false;

				if(bAutoPrefix)
				{
					for(int i = 1; i < NPC_MODE_MAX; ++i)
					{
						char chModeID[2] = {0,};
						sprintf_s(chModeID, "%d", i);
						string strModeName = "mode" + string(chModeID) + "_" + it_ani->strAnimationName;
						RAnimation* pModeAnimation = m_pActor->GetAnimation(strModeName.c_str());
						if(pModeAnimation)
						{
							bModeCheck = true;
							break;
						}
					}
				}
				
				if(bModeCheck == false)
				{
					string strError;
					if(talentAniInfo.strMotionName.empty() == false)
						strError= talentAniInfo.strMotionName + " / ";

					strError += it_ani->strAnimationName;

					// 존재하지 않는다.
					if (System::Windows::Forms::MessageBox::Show(String::Format("[{0}] 애니메이션이 존재하지 않습니다.", gcnew String(strError.c_str())), "애니메이션 이상."
						,MessageBoxButtons::OK) == System::Windows::Forms::DialogResult::OK)
					{
						bExist = false;
					}
				}
			}
		}
	}

	return bExist;
}

void TTalentAnimation::Reload()
{
	CSTalentInfo* pTalentInfo = TTalentHelper::GetTalentInfo(m_nCurrentTalentID, m_nCurrentTalentMode);
	if(pTalentInfo == NULL)
	{
		m_pTalentInfo = NULL;
		m_TalentHitEffectDirTest.SetData(m_pTalentInfo);
		return;
	}
	
	SetTalentAnimation(pTalentInfo, m_nCurrentTalentMode);
}

void TTalentAnimation::ShowTalentHitEffect( XTalentEffectDataMgr* pEffectMgr, vector<XTalentEffectInfo *>* pvecEffectInfo )
{
	if(m_pActor == NULL ||
		pvecEffectInfo == NULL ||
		pEffectMgr == NULL)
		return;

	vector<XTalentEffectInfo *> vecHitEffectInfoByType;
	pEffectMgr->Get(HIT_EFFECT_POSDIR, vecHitEffectInfoByType);

	string n = g_pMainApp->GetActor()->GetMeshName();
	string nn = n.substr(0, n.rfind("."));

	for(vector<XTalentEffectInfo *>::iterator itEffect = vecHitEffectInfoByType.begin(); itEffect != vecHitEffectInfoByType.end(); itEffect++)
	{
		for(vector<XTalentHitEffectPosDir>::iterator it = (*itEffect)->m_vecTalentHitEffectPosDir.begin(); it != (*itEffect)->m_vecTalentHitEffectPosDir.end(); ++it)
		{
			if(it->fHitTime == m_TalentHitEffectDirTest.GetCheckTime() &&
				it->strMeshName == nn)
			{
				MMatrix matResult;
				matResult.MakeIdentity();
				matResult.SetTranslation(it->vHitEffectPos);

				MMatrix matEffect = matResult * m_pActor->GetWorldTransform();

				bool bModelEffect = false;

				for(vector<XTalentEffectInfo *>::iterator itHitEffect = pvecEffectInfo->begin(); itHitEffect != pvecEffectInfo->end(); ++itHitEffect)
				{
					vector<EFFECT_RESULT_DATA> vecResult;

					TEffectCharacterToolSinglyInvoker effectSinglyInvoker;
					effectSinglyInvoker.Invoke((*itHitEffect)->m_strEffectName, matEffect.GetTranslation(), vec3(-vec3::AXISY), it->vSwordTrailHitUp, 0.0f, &vecResult);

					for(vector<EFFECT_RESULT_DATA>::iterator itResult = vecResult.begin(); itResult != vecResult.end(); ++itResult)
					{
						if(itResult->type == CET_MODEL)
							bModelEffect = true;
					}
				}

				// 디폴트로?
				if(bModelEffect == false)
				{
					XTalentEffectBase * pEffectBase = g_pMainApp->m_pTalentEffectMgr->GetTalentEffectBase(TEBY_MELEE_ATTACK);
					if(pEffectBase)
					{
						TEffectCharacterToolSinglyInvoker effectSinglyInvoker;
						effectSinglyInvoker.Invoke(pEffectBase->m_strEffectBaseFileName, matEffect.GetTranslation(), vec3(-vec3::AXISY), it->vSwordTrailHitUp);
					}
				}
			}
		}
	}
}

void TTalentAnimation::ShowTalentHitEffectPosBox()
{
	XTalentEffectDataMgr* pEffectMgr = m_pTalentInfo->GetEffectDataMgr();
	if(pEffectMgr)
	{
		// 레퍼런스 탤런트 이펙트
		if(pEffectMgr->m_nRefID > 0)
		{
			TTalentInfo* pRefTalent = TTalentHelper::GetTalentInfo(pEffectMgr->m_nRefID, 0);
			if(pRefTalent)
			{
				pEffectMgr = pRefTalent->GetEffectDataMgr();
			}
		}

		vector<XTalentEffectInfo *> vecEffectInfoByType;
		pEffectMgr->Get(HIT_EFFECT_POSDIR, vecEffectInfoByType);

		string n = g_pMainApp->GetActor()->GetMeshName();
		string nn = n.substr(0, n.rfind("."));

		if(vecEffectInfoByType.empty())
			return;

		for(vector<XTalentEffectInfo *>::iterator itEffect = vecEffectInfoByType.begin(); itEffect != vecEffectInfoByType.end(); itEffect++)
		{
			for(vector<XTalentHitEffectPosDir>::iterator it = (*itEffect)->m_vecTalentHitEffectPosDir.begin(); it != (*itEffect)->m_vecTalentHitEffectPosDir.end(); ++it)
			{
				if(it->strMeshName == nn)
				{
					MMatrix matResult;
					MMatrix matWorld;

					int nFrame = m_pActor->GetSecToFrame(it->fHitTime);
					m_pActor->GetAnimatedActorNodeTransform2(nFrame, string("dummy_loc"), matWorld, E_TS_LOCAL_RESULT);
				
					matResult.MakeIdentity();
					matResult.SetLocalMatrix(it->vHitEffectPos, vec3::AXISY, it->vSwordTrailHitUp);
					//matResult.SetTranslation(it->vHitEffectPos);

					MMatrix matEffect = matResult * matWorld;

					RBoundingBox box = RBoundingBox(RVector(-5,-5,-5),RVector(5,5,5));
					RRenderHelper::RenderBox(matEffect,box,0xffff0000);
					RRenderHelper::RenderAxis(matEffect, 5);
					g_pMainApp->RenderAxisText(matEffect, 5);
				}
			}
		}
	}
}

void TTalentAnimation::Render()
{
	if(m_bShowHitEffect)
	{
		RRenderHelper::SetRenderState();

		ShowTalentHitEffectPosBox();
	
		RRenderHelper::EndRenderState();
	}
}

bool TTalentAnimation::Play()
{
	if (m_pActor == NULL) return false;
	if (m_pTalentInfo == NULL) return false;

	bool bResult = PlayTalentAnimation();

	bool bPost = false;
	m_fPostDelayRemainTime = m_pTalentInfo->m_fPostDelay;
	if(m_fPostDelayRemainTime > 0.0f)
	{
		bPost = true;
	}

	if(bResult || bPost)
	{
		m_TalentEvent.SetActor(m_pActor);
		m_TalentEvent.SetTalent(m_pTalentInfo);
		m_TalentEvent.EventsStart(&m_pTalentInfo->m_Events);
		m_TalentEvent.SetActStart(false);
	}

	return bResult;
}

int TTalentAnimation::GetCurTalentFrameMax()
{
	if(m_pMotion == NULL)
		return 0;

	int nCurTalentFrameMax = m_pMotion->GetCurTalentFrameMax();
	if(m_pActor && m_pTalentInfo && m_pTalentInfo->m_fPostDelay > 0.0f)
	{
		// Post Delay
		nCurTalentFrameMax += m_pActor->GetSecToFrame(m_pTalentInfo->m_fPostDelay);
	}

	return nCurTalentFrameMax;
}

int TTalentAnimation::GetCurTalentCurFrame()
{
	if(m_pMotion == NULL)
		return 0;

	int nCurTalentFrame = m_pMotion->GetCurTalentCurFrame();
	if(m_pActor && m_pTalentInfo)
	{
		// Post Delay
		float fElapsedTime = m_pTalentInfo->m_fPostDelay - m_fPostDelayRemainTime;
		if(fElapsedTime < 0.0f)
			fElapsedTime = 0.0f;
		nCurTalentFrame += m_pActor->GetSecToFrame(fElapsedTime);
	}

	return nCurTalentFrame;
}

float TTalentAnimation::GetCurTalentCurFrametime()
{
	if(m_pMotion == NULL)
		return 0;

	float time = m_pMotion->GetCurTalentCurFrametime();

	if(m_pActor && m_pTalentInfo)
	{
		// Post Delay
		float fElapsedTime = m_pTalentInfo->m_fPostDelay - m_fPostDelayRemainTime;
		if(fElapsedTime < 0.0f)
			fElapsedTime = 0.0f;
		time += fElapsedTime;
	}

	return time;
}

float TTalentAnimation::GetCurTalentMaxFrametime()
{
	if(m_pMotion == NULL)
		return 0;

	float time = m_pMotion->GetCurTalentMaxFrametime();

	if(m_pActor && m_pTalentInfo)
	{
		// Post Delay
		time += m_pTalentInfo->m_fPostDelay;
	}

	return time;
}

int TTalentAnimation::GetTalentAnimationMaxIndex()
{
	if(m_pMotion == NULL)
		return 0;

	int nMaxIndex = m_pMotion->GetCurAniMaxSequence();


	if(m_pActor && m_pTalentInfo && m_pTalentInfo->m_fPostDelay > 0.0f)
	{
		// Post Delay
		++nMaxIndex;
	}

	return nMaxIndex;
}

int TTalentAnimation::GetTalentFrame( int nAniIndex )
{
	if(m_pMotion == NULL)
		return 0;

	int nFrame = m_pMotion->GetTalentFrame(nAniIndex);

	if(nFrame == 0 && m_pActor && m_pTalentInfo)
	{
		nFrame = m_pActor->GetSecToFrame(m_pTalentInfo->m_fPostDelay);
	}

	return nFrame;
}

std::string TTalentAnimation::GetTalentAniName( int nAniIndex )
{
	if(m_pMotion == NULL)
		return "";

	XMotion* pMotion = m_pMotion->GetMotion();
	if(pMotion == NULL)
		return "";

	XMotionAniSequence* pAniSequence = pMotion->GetAniSequence(m_pMotion->GetCurMotionType(), nAniIndex);
	if(pAniSequence == NULL && m_pActor && m_pTalentInfo && m_pTalentInfo->m_fPostDelay > 0.0f)
	{
		return "Post Delay";
	}

	return pAniSequence->GetAniName();
}

bool TTalentAnimation::CheckTalentAnimationEnd()
{
	if(m_pMotion == NULL)
		return 0;

	int nCurTalentFrameMax = m_pMotion->GetCurTalentFrameMax();
	int nCurTalentFrame = m_pMotion->GetCurTalentCurFrame();

	if(nCurTalentFrameMax > 0 && nCurTalentFrameMax > nCurTalentFrame)
		return false;

	return true;
}

void TTalentAnimation::SetTalentAnimationFrame( int frame )
{
	if(m_pMotion == NULL)
		return;

	int nCurTalentFrameMax = m_pMotion->GetCurTalentFrameMax();

	if(nCurTalentFrameMax > 0 && nCurTalentFrameMax > frame)
	{
		m_pMotion->SetFrame(frame);
		return;
	}

	m_pMotion->SetFrame(nCurTalentFrameMax);

	int nPostFrame = frame - nCurTalentFrameMax;
	m_fPostDelayRemainTime = m_pTalentInfo->m_fPostDelay - m_pActor->GetFrameToSec(nPostFrame);
}
