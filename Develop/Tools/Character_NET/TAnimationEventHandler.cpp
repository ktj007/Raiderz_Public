#include "stdafx.h"
#include <mmsystem.h>
#include "TAnimationeventhandler.h"
#include "RActor.h"
#include "REffectInstance.h"

#include "RConfiguration.h"
#include "TEffectCharacterToolInvoker.h"
#include "RActorNodeConstraintController.h"

// listener , 테스트 리스너이며 저장되지 않는다
TAnimationEventListener::~TAnimationEventListener()
{
	Clear();
}

void TAnimationEventListener::Clear()
{
	while(!empty())
	{
		delete begin()->second;
		erase(begin());
	}
}

void TAnimationEventListener::DeleteHandler(TAnimationEventHandler* pHandler)
{
	for(iterator itr = begin(); itr!=end(); )
	{
		if(itr->second==pHandler)
		{
			delete itr->second;
			itr = erase(itr);
		}
		else
			itr++;
	}
}

void TAnimationEventListener::OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent)
{
	iterator itr = find(pEvent->m_strEvent);
	while(itr!=end() && (itr->first == pEvent->m_strEvent))
	{
		itr->second->Do();
		itr++;
	}
}

/////////////// SoulHuntAnimationEventListener
map<string,ANIMATIONEVENTHANDLER> TSoulHuntAnimationEventListener::m_eventHandlers;

TSoulHuntAnimationEventListener::TSoulHuntAnimationEventListener()
: m_pEffectSceneNode(NULL)
{
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_custom",	OnAnimationEvent_custom) );
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_walk",	OnAnimationEvent_walk) );
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_sound",	OnAnimationEvent_sound) );
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_effect", OnAnimationEvent_effect) );
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_camera",	OnAnimationEvent_camera) );
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_face_ani_start",	OnAnimationEvent_face_ani_start) );
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_face_ani_end",	OnAnimationEvent_face_ani_end) );
	m_eventHandlers.insert( ANIMATIONEVENTMAP::value_type( "sh_defence_start_sound",	OnAnimationEvent_defence_start_sound) );
}

TSoulHuntAnimationEventListener::~TSoulHuntAnimationEventListener()
{
	DeleteLoopAniEffect();

	delete m_pEffectSceneNode;
}

void TSoulHuntAnimationEventListener::OnAnimationEvent(RActor* pActor, const RAnimationEvent* pEvent)
{
	ANIMATIONEVENTMAP::iterator itr = m_eventHandlers.find(pEvent->m_strEvent);
	bool bUsableEvent = g_pMainApp->m_TabAnimation.CheckUsableAnimationEvent(pEvent);
	if(itr!=m_eventHandlers.end() && bUsableEvent)
	{
		itr->second(*this, pEvent);
	}
	else
	{
		if(bUsableEvent == false)
		{
			CheckDeleteLoopAniEffect(pEvent);
		}
		::TAnimationEventListener::OnAnimationEvent(pActor,pEvent);
	}
}

void TSoulHuntAnimationEventListener::OnAnimationEvent_walk(TSoulHuntAnimationEventListener& handler, const RAnimationEvent *pEvent)
{
	g_pMainApp->m_pSoundMgr.Play( "footstep_pnt_R");			//<<<rsound3
}

void TSoulHuntAnimationEventListener::OnAnimationEvent_sound(TSoulHuntAnimationEventListener& handler, const RAnimationEvent *pEvent)
{
	// 사운드 렌덤 발생(http://dev/redmine/issues/13696)
	int nRandomCount = atoi(((string)pEvent->m_strParam3).c_str());
	int nRandom = MMath::RandomNumber(1, 10);

	if(nRandomCount > 0 && nRandomCount < 10 && nRandomCount < nRandom)
		return;

	g_pMainApp->m_pSoundMgr.PlayDynamic3D(pEvent->m_strParam1.c_str(), pEvent->m_strParam2.c_str(), NULL);
}

//내 캐릭의 이벤트들이니 스테레오로
void TSoulHuntAnimationEventListener::OnAnimationEvent_custom(TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent)
{
	vector<EFFECT_RESULT_DATA> vecUIDEffect;

	TEffectCharacterToolInvoker effectInvoker;

	// 중복 체크
	for(vector<stLoopAniEffect>::iterator itCheck = handler.m_vLoopAniEffect.begin(); itCheck != handler.m_vLoopAniEffect.end(); ++itCheck)
	{
		if(itCheck->m_string1 == (string)pEvent->m_strParam1 &&
			itCheck->m_string2 == (string)pEvent->m_strParam2 &&
			itCheck->m_string3 == (string)pEvent->m_strParam3)
		{
			return;
		}
	}
	
	// 이펙트 발생
	effectInvoker.Invoke(g_pMainApp->GetActor(), (string)pEvent->m_strParam1,(string) pEvent->m_strParam2, (string)pEvent->m_strParam3, &vecUIDEffect);

	for(vector<EFFECT_RESULT_DATA>::iterator itUID = vecUIDEffect.begin(); itUID != vecUIDEffect.end(); itUID++)
	{
		if((*itUID).type == CET_MODEL)
		{
			// 중복 비교
			bool bPush = false;

			// Loop 이펙트인가?
			TEffect* pEffect = g_pMainApp->m_pEffectManager->GetTEffect((*itUID).uidEffect);
			if(pEffect)
				bPush = pEffect->IsEffectLoop();

			if(bPush)
			{
				stLoopAniEffect loopAniEffect;
				loopAniEffect.m_uid = (*itUID).uidEffect;
				loopAniEffect.m_string1 = (string)pEvent->m_strParam1;
				loopAniEffect.m_string2 = (string)pEvent->m_strParam2;
				loopAniEffect.m_string3 = (string)pEvent->m_strParam3;

				handler.m_vLoopAniEffect.push_back(loopAniEffect);
			}
		}
	}

	vecUIDEffect.clear();
}

bool TSoulHuntAnimationEventListener::CheckEffectLoop( RActor* pActor, string& strEffectFileName, string& strBoneName )
{
	// 루프 이펙트가 애니메이션 프레임 마다 생성되지않도록 체크
	bool bLoop = false;

	TCharacter* pCharacter = static_cast<TCharacter*>(pActor);
	RAnimation* pAni = pCharacter->GetCurAnimation();
	bLoop = (pAni && pAni->GetAnimationLoopType() == RAniLoopType_Loop) ? true : false;

	if(bLoop)
	{
		RSceneNodeList NodeList = pActor->GetChildren();
		for (RSceneNodeList::iterator it = NodeList.begin(); it != NodeList.end(); it++)
		{
			RSceneNode* pSceneNode = (*it);
			if (strcmp(pSceneNode->GetNodeIDString(), RSID_EFFECT) == 0)
			{
				REffectSceneNode* pEffectInstance = (REffectSceneNode*)pSceneNode;

				RActorNodeConstraintController* pConstraintController = GetFirstActorNodeConstraintController(pEffectInstance);

				//int token_path = pEffectInstance->m_strFileName.rfind("/") + 1;
				//string strTempFileName = pEffectInstance->m_strFileName.substr(token_path, pEffectInstance->m_strFileName.size() - token_path);
				//string strTempFileName2 = strTempFileName.substr(0, strTempFileName.find("."));

				string strCompareEffectName = TBaseHelper::GetFileNameNoExtension(pEffectInstance->m_strFileName);

				if( pConstraintController != NULL &&
					pConstraintController->GetTargetActorNode()->GetNodeName() == strBoneName &&
					strCompareEffectName == strEffectFileName &&
					pEffectInstance->IsLooping() )
					return false;
			}
		}
	}

	return true;
}

void TSoulHuntAnimationEventListener::OnAnimationEvent_effect( TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent )
{
	if( pEvent->m_strParam1.empty() ||
		g_pMainApp->GetActor() == NULL) 
	{
		return;
	}

	// 중복 체크
	for(vector<stLoopAniEffect>::iterator itCheck = handler.m_vLoopAniEffect.begin(); itCheck != handler.m_vLoopAniEffect.end(); ++itCheck)
	{
		if(itCheck->m_string1 == (string)pEvent->m_strParam1 &&
			itCheck->m_string2 == (string)pEvent->m_strParam2 &&
			itCheck->m_string3 == (string)pEvent->m_strParam3)
		{
			return;
		}
	}

	MUID uidEffect = MUID::ZERO;
	if( !pEvent->m_strParam2.empty() )
	{
		RActorNode *pParentActorNode = g_pMainApp->GetActor()->GetActorNode(pEvent->m_strParam2.c_str());

		uidEffect = g_pMainApp->m_pEffectManager->AddTEffect(pEvent->m_strParam1, g_pMainApp->GetActor(), pParentActorNode);

		RAnimation* pAni = g_pMainApp->GetActor()->GetCurAnimation();
		bool bLoop = (pAni && pAni->GetAnimationLoopType() == RAniLoopType_Loop) ? true : false;

		//현재 애니메이션과 현재 모션이 다를수 있어서 현재 애니메이션으로 체크 하도록 수정.
		if( (uidEffect != MUID::ZERO) && bLoop )
		{
			// 루프 이펙트만 체크
			TEffect* pXEffect = g_pMainApp->m_pEffectManager->GetTEffect(uidEffect);
			if( pXEffect && pXEffect->IsEffectLoop() )
			{
				stLoopAniEffect loopAniEffect;
				loopAniEffect.m_uid = uidEffect;
				loopAniEffect.m_string1 = (string)pEvent->m_strParam1;
				loopAniEffect.m_string2 = (string)pEvent->m_strParam2;
				loopAniEffect.m_string3 = (string)pEvent->m_strParam3;

				handler.m_vLoopAniEffect.push_back(loopAniEffect);
			}
		}
	}
	else
	{
		RMatrix matLocal;
		matLocal.SetLocalMatrix(g_pMainApp->GetActor()->GetPosition(), vec3::AXISY, vec3::AXISZ);
		g_pMainApp->m_pEffectManager->AddTEffect(pEvent->m_strParam1, NULL, NULL, matLocal);
	}
}

void TSoulHuntAnimationEventListener::OnAnimationEvent_camera(TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent)
{
	// 카메라 이펙트 발생
	g_pMainApp->GetCameraEffect().PlayCameraEffect(pEvent->m_strParam1.c_str(), 0.0f);
}

void TSoulHuntAnimationEventListener::StopAnimationEffect()
{
	DeleteLoopAniEffect();
}

void TSoulHuntAnimationEventListener::DeleteLoopAniEffect()
{
	for( size_t i = 0; i < m_vLoopAniEffect.size(); i++)
	{
		if(g_pMainApp->m_pEffectManager)
			g_pMainApp->m_pEffectManager->DeleteTEffect(m_vLoopAniEffect[i].m_uid);
	}

	m_vLoopAniEffect.clear();
}
void TSoulHuntAnimationEventListener::CheckDeleteLoopAniEffect(const RAnimationEvent *pEvent)
{
	// 중복 체크
	for(vector<stLoopAniEffect>::iterator itCheck = m_vLoopAniEffect.begin(); itCheck != m_vLoopAniEffect.end(); ++itCheck)
	{
		if(itCheck->m_string1 == (string)pEvent->m_strParam1 &&
			itCheck->m_string2 == (string)pEvent->m_strParam2 &&
			itCheck->m_string3 == (string)pEvent->m_strParam3)
		{
			if(g_pMainApp->m_pEffectManager)
				g_pMainApp->m_pEffectManager->DeleteTEffect(itCheck->m_uid);

			m_vLoopAniEffect.erase(itCheck);
			return;
		}
	}
}

void TSoulHuntAnimationEventListener::OnAnimationEvent_face_ani_start( TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent )
{
	if(g_pMainApp->GetActor())
	{
		bool bEye = false;
		if(pEvent->m_strParam2 == "true")
			bEye = true;

		g_pMainApp->GetActor()->StartFaceAnimation((string)pEvent->m_strParam1, bEye);
	}
}

void TSoulHuntAnimationEventListener::OnAnimationEvent_face_ani_end( TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent )
{
	if(g_pMainApp->GetActor())
	{
		float fEndTime = atof(((string)pEvent->m_strParam2).c_str());

		g_pMainApp->GetActor()->EndFaceAnimation((string)pEvent->m_strParam1, fEndTime);
	}
}

void TSoulHuntAnimationEventListener::OnAnimationEvent_defence_start_sound( TSoulHuntAnimationEventListener& handler, const RAnimationEvent* pEvent )
{
	if(g_pMainApp->GetActor() && g_pMainApp->GetActor()->IsOncePlayDone() == false)
	{
		g_pMainApp->m_pSoundMgr.PlayDynamic3D(pEvent->m_strParam1.c_str(), NULL, NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
// handler

TAnimationEventHandler::TAnimationEventHandler(void)
: m_pEffectSceneNode(NULL)
{
}

TAnimationEventHandler::~TAnimationEventHandler(void)
{
	delete m_pEffectSceneNode;
}

bool TAnimationEventHandler::Do()
{
	if(!m_strEffectFileName.empty())
	{
		RActor* pActor = g_pMainApp->GetActor();
		if (m_pEffectSceneNode)
		{
// 			RResource *pResource = m_pEffectSceneNode->GetResource();
// 			if (pResource && pResource->GetName() == m_strEffectFileName )
			
			// TODO: 이거 맞나 모르겠네요, 테스트 해봐야합니다, dubble
			if ( m_pEffectSceneNode->GetNodeName() == m_strEffectFileName )
			{
				if (m_pEffectSceneNode->IsLooping() == false)
				{
					m_pEffectSceneNode->Start();
				}

				return true;
			}
			else
			{
				m_pEffectSceneNode->RemoveFromParent();
				delete m_pEffectSceneNode;
			}
		}

		// 이팩트
		REffectInstanceSceneNode* pEffectInstance = static_cast<REffectInstanceSceneNode*>(REngine::GetSceneManagerPtr()->CreateSceneNode(RSID_EFFECTINSTANCE));
		if( !pEffectInstance->Create( m_strEffectFileName.c_str() ) )
		{
#ifdef _DEBUG
			mlog("CreateEffect 실패(%s)\n", m_strEffectFileName.c_str());
#endif
			SAFE_DELETE(pEffectInstance);
			return false;
		}

		m_pEffectSceneNode = pEffectInstance;

		RActorNode *pParentActorNode = NULL;
		if(!m_strEffectParent.empty())
			pParentActorNode = pActor->GetActorNode(m_strEffectParent.c_str());

		g_pMainApp->GetSceneMgr()->AttachEffect(m_pEffectSceneNode, pActor, pParentActorNode);
	}

	if(!m_strSoundFileName.empty())
	{
		PlaySound(m_strSoundFileName.c_str(),NULL,SND_ASYNC);
	}

	return true;
}
