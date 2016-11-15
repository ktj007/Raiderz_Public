#include "StdAfx.h"
#include "RAnimationController.h"
#include "RAnimation.h"
#include "RAnimationNode.h"
#include "RAnimationControllerNode.h"
#include "RActor.h"
#include "RActorNodePhysique.h"
#include "RAnimationResource.h"
#include "RMesh.h"
#include "RMeshNode.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
// RActorNodeSelection
RActorNodeSelection::RActorNodeSelection()
{
}

bool RActorNodeSelection::IsExistNode(const char* szNodeName)
{
	RACTORNODESET::iterator itr = m_nodes.find( szNodeName );
	if( itr!= m_nodes.end() )
		return true;

	return false;
}

bool RActorNodeSelection::SelectNode(RActor* pActor, const char* szNodeName)
{
	if( IsExistNode(szNodeName) )
		return false;

	m_nodes.insert( szNodeName );
	return true;
}

bool RActorNodeSelection::SelectNodeAndChildren( RActor* pActor, const char* szNodeName, bool bExceptSelectNode /*= false*/ )
{
	// Root는 제외할것인가?
	if(bExceptSelectNode == false)
	{
		if(!SelectNode(pActor, szNodeName)) 
			return false;
	}

	RActorNode* pActorNode = pActor->GetActorNode(szNodeName);
	for(int i=0;i<pActor->GetActorNodeCount();++i)
	{
		RActorNode* pNode = pActor->GetActorNodes()[i];
		if(pActorNode==pNode->GetParent())
			SelectNodeAndChildren( pActor, pNode->GetNodeName().c_str());
	}
	return true;
}

RAnimationFrameController::RAnimationFrameController( RSNCTRL_TYPE eType /*= RSNCTRL_UPDATE*/ )
: RActorController( eType )
, m_fSpeed(1.f), m_nState(RPS_STOP)
, m_nMaxFrame(0), m_nFrame(0)
, m_bPlayDone(false), m_bOncePlayDone(false), m_bEventActive(true)
{
}

void RAnimationFrameController::Update(RCameraSceneNode* pCamera)
{
	DWORD dwElapsedTime = REngine::GetSceneManager().GetTimer().GetElapsedTime();

	if(m_nMaxFrame)
		UpdateFrame(dwElapsedTime);

	if( m_bPlayDone && m_bLoop)
	{
		m_bPlayDone = false;
		ResetEvent();
	}
}

void RAnimationFrameController::GetNextFrameAndState(DWORD dwElapsedTime, DWORD& _nOutNextFrame, bool& _bPlayDone) const
{
	if(m_nState==RPS_PLAY)
		_nOutNextFrame += DWORD(4.8f*m_fSpeed*dwElapsedTime);	// Max의 단위 4800 프레임이 1초

	if(_nOutNextFrame >= m_nMaxFrame)
	{
		if(m_bLoop && m_nState==RPS_PLAY && m_nMaxFrame > 0 )
			_nOutNextFrame = _nOutNextFrame % m_nMaxFrame;
		else
			_nOutNextFrame = m_nMaxFrame;
		_bPlayDone = true;
	}
}

void RAnimationFrameController::UpdateFrame(DWORD dwElapsedTime)
{
	GetNextFrameAndState(dwElapsedTime, m_nFrame, m_bPlayDone);
	if (m_bPlayDone)
		m_bOncePlayDone = true;

	if( !m_pActor->GetListener() ) return;

	// 애니메이션 이벤트 발생
	if (m_bEventActive)
	{
		for(size_t i=0; i<m_AniamtionEvents.size(); ++i)
		{
			ANIMATION_EVENT &aniEvent = m_AniamtionEvents[i];
			if(!aniEvent.bEventDone && aniEvent.pAnimationEvent->m_nFrame <= m_nFrame)
			{
				aniEvent.bEventDone = true;
				m_pActor->GetListener()->OnAnimationEvent(m_pActor, aniEvent.pAnimationEvent);
			}
		}
	}
}

void RAnimationFrameController::ResetEvent()
{
	for(vector<ANIMATION_EVENT>::iterator itEvent = m_AniamtionEvents.begin(); itEvent != m_AniamtionEvents.end(); ++itEvent)
	{
		(*itEvent).bEventDone = false;
	}
}

void RAnimationFrameController::CloseEvent()
{
	for(vector<ANIMATION_EVENT>::iterator itEvent = m_AniamtionEvents.begin(); itEvent != m_AniamtionEvents.end(); ++itEvent)
	{
		(*itEvent).bEventDone = true;
	}
}

void RAnimationFrameController::SetAnimation( RAnimation *pAnimation )
{
	// 셋팅 이전에 처리해야 할 이벤트
	CheckAnimationEventSend();

	RAnimationResource* pResource = pAnimation->GetResource();
	m_nMaxFrame = pResource->GetMaxFrame();
	m_bLoop = (pAnimation->GetAnimationLoopType()==RAniLoopType_Loop);
	m_fSpeed = pAnimation->GetSpeed();


	// 애니메이션 이벤트 셋팅
	m_AniamtionEvents.clear();
	for(vector<RAnimationEvent*>::iterator itAniEvent = pAnimation->m_animationEvents.begin(); itAniEvent != pAnimation->m_animationEvents.end(); ++itAniEvent)
	{
		ANIMATION_EVENT aniEvent;
		aniEvent.pAnimationEvent = *itAniEvent;
		m_AniamtionEvents.push_back(aniEvent);
	}
}

void RAnimationFrameController::Reset()
{
	m_nFrame = 0;
	m_nMaxFrame = 0;
	m_bPlayDone = false;
	m_bOncePlayDone = false;
	m_nState = RPS_STOP;
	m_bLoop = false;

	m_AniamtionEvents.clear();
}

void RAnimationFrameController::Play()
{
	if( m_nMaxFrame <= 0 ) return;

	m_nState = RPS_PLAY;
	m_bPlayDone	= false;
	m_bOncePlayDone = false;
}

void RAnimationFrameController::Stop() 
{
	Reset();
}

void RAnimationFrameController::Pause() 
{
	m_nState = RPS_PAUSE;
}

void RAnimationFrameController::SetAnimationEvent(vector<ANIMATION_EVENT>& aniamtionEvents)
{
	m_AniamtionEvents.clear();
	m_AniamtionEvents = aniamtionEvents;
}

void RAnimationFrameController::SetForceAnimationControlPlayDoneForTest( bool bPlayDone )
{
	m_bPlayDone = bPlayDone;
	m_bOncePlayDone = bPlayDone;
}

void RAnimationFrameController::CheckAnimationEventSend()
{
	if( !m_pActor->GetListener() ) return;

	for(int i=0;i<(int)m_AniamtionEvents.size();++i)
	{
		ANIMATION_EVENT &aniEvent = m_AniamtionEvents[i];
		if(aniEvent.bEventDone == false && aniEvent.pAnimationEvent->m_bUnconditional)
		{
			aniEvent.bEventDone = true;
			m_pActor->GetListener()->OnAnimationEvent(m_pActor, aniEvent.pAnimationEvent);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// RAnimationController_New
RAnimationController::RAnimationController( RSNCTRL_TYPE eType /*= RSNCTRL_POSTANIMATION*/, RSNCTRL_TYPE eFrameControllerType /*= RSNCTRL_UPDATE*/ )
: RActorController(eType)
, m_frameController(eFrameControllerType)
, m_pAnimation(NULL)
{
	// 관리되지않는 콘트롤러로 지정
	m_frameController.AddAttribute( RCA_STICKY );
}

RAnimationController::~RAnimationController(void)
{
	Destroy();
}

void RAnimationController::OnAdd()
{
	RActorController::OnAdd();
	m_pActor->AddController( &m_frameController );
	m_frameController.SetAutoUpdate(true);
}

void RAnimationController::OnRemove()
{
	m_frameController.SetAutoUpdate(false);
	m_pActor->RemoveController( &m_frameController );
	RActorController::OnRemove();
}

void RAnimationController::ReNew()
{
	RAnimation* pAnimation = GetAnimation();
	DWORD dwFrame = GetFrame();
	RPLAYSTATE ePlayState = GetState();

	// 이벤트 보관
	vector<ANIMATION_EVENT> aniamtionEvents = m_frameController.GetAnimationEvent();

	// once play done 보관
	bool bOncePlayDone = m_frameController.IsOncePlayDone();

	/*RPLAYINFO playInfo;
	playInfo.fPlaySpeed = GetSpeed();
	playInfo.fWeight = GetWeight();
	playInfo.fBlendIn = m_fBlendIn;
	playInfo.fBlendOut = m_fBlendOut;
	playInfo.blendMode =  GetBlendMode();
	playInfo.pNodeSet = m_pNodeSelection;*/
	// 리셋
	//SetAnimation(pAnimation, NULL);
	SetAnimation(pAnimation, &m_playInfo);
	SetFrame(dwFrame);

	// 이벤트 재설정
	m_frameController.SetAnimationEvent( aniamtionEvents );

	// 위의 설정에서 애니메이션을 정지 시키므로 Play를 시킵니다.
	if (RPS_PLAY == ePlayState)
	{
		Play();
		Update(NULL);
	}

	// once play done 재설정
	m_frameController.SetOncePlayDone( bOncePlayDone );
}

void RAnimationController::Destroy()
{
	Reset();
}

void RAnimationController::Reset()
{
	Disconnect();

	m_frameController.Reset();

	m_nodeControllers.erase(m_nodeControllers.begin(),m_nodeControllers.end());
	m_activeControllers.erase(m_activeControllers.begin(),m_activeControllers.end());
}

void RAnimationController::ActivateNodes( RActorNodeSelection* pSelection )
{
	vector<RActorNode*>& lodBoneSet = m_pActor->GetAnimationReferrencedNodes();

	// 0 번 만으로 테스트중
	m_activeControllers.erase(m_activeControllers.begin(),m_activeControllers.end());
	for(size_t i=0;i<lodBoneSet.size();++i)
	{
		RActorNode* pNode = lodBoneSet[i];
		RAnimationControllerNode* pControllerNode = GetNodeController( pNode->GetNodeName() );
		if(!pControllerNode)
			continue;

		// 특정 노드 셋을 설정했으면 따른다
		if( pSelection && !pSelection->IsExistNode( pNode->GetNodeName().c_str() ) )
			continue;

		if(pControllerNode->GetAnimationNode()==NULL)
			continue;

		// 보이지 않고, 보일 애니메이션도 없으면 사용하지 않는다
		if( (pNode->m_pMeshNode->m_fBaseVisibility==0 || pNode->m_pMeshNode->m_fBaseNoiseRef==0) && pControllerNode->GetAnimationNode()->m_visKeyTrack.empty()) 
			continue;

		m_activeControllers.push_back(pControllerNode);
	}
}

void RAnimationController::ConnectAnimation(RAnimation *pAnimation, RActorNodeSelection* pSelection )
{
	Disconnect();

	RAnimationResource* pResource = pAnimation->GetResource();
	m_pAnimation = pAnimation;
	m_nodeControllers.reserve( pResource->GetAniNodeCount() );

	//mlog("ConnectAnimation \n");
	//mlog("%s , speed : %f \n", pAnimation->GetName(), pAnimation->GetSpeed());

	// actor 연결 초기화
	// 하면서.. 피직 노드가 있는지도 체크하자
	bool bUsePhysique = false;
	for(int i=0;i<m_pActor->GetActorNodeCount();++i) {
		RActorNode* pNode = m_pActor->GetActorNodes()[i];

		_ASSERT(pNode);

		// 피직의 형태를 가지고 있는지..
		if( dynamic_cast<RActorNodePhysique*>(pNode))
			bUsePhysique = true;

		// 특정 노드 셋을 설정했으면 따른다
		if( pSelection && !pSelection->IsExistNode( pNode->GetNodeName().c_str() ) ) continue;

		// 계층을 안타는 경우는 초기화 해주지 않는다.
		if (pNode->QueryAttribute(RSNA_NO_HIERARCHY_UPDATE) == false)
		{
			pNode->SetVisibility( pNode->m_pMeshNode->m_fBaseVisibility );
			pNode->SetNoiseRef( pNode->m_pMeshNode->m_fBaseNoiseRef);
		}
	}

	m_nodeControllers.resize( pResource->GetAniNodeCount() );

	for(int i=0;i<pResource->GetAniNodeCount();++i)
	{
		RAnimationNode* pAnimationNode = pResource->GetAniNode(i);
		RAnimationControllerNode* pControllerNode = &m_nodeControllers[i];

		const RMatrix &matAnimationLocal = pAnimationNode->m_matLocal;

		RActorNode* pActorNode = m_pActor->GetActorNode(pAnimationNode->GetName());

		pControllerNode->SetAnimationNode(pAnimationNode);
		
		if(pResource->GetFileVersion() < EXPORTER_ANI_VER12)
		{
			pControllerNode->m_matBase = matAnimationLocal;
			pControllerNode->m_matCurrent = matAnimationLocal;
		}

		// 기본행렬을 scale, rotation, translation 으로 분해해서 저장해둠
		if(pResource->GetFileVersion() < EXPORTER_ANI_VER12)
			pControllerNode->m_matBase.Decompose(pControllerNode->m_currentScale,pControllerNode->m_currentTranslation,pControllerNode->m_currentRotation);
		else
		{
			pControllerNode->m_currentScale = pAnimationNode->m_baseScale;
			pControllerNode->m_currentRotation = pAnimationNode->m_baseRotation;
			pControllerNode->m_currentTranslation = pAnimationNode->m_baseTranslation;
		}
		pControllerNode->m_pActorNode = pActorNode;
		// 피직의 형태를 가지는 액터의 컨트롤러에 사용 되는 액터 노드는 피직 에니메이션에 쓰이는 본의 형태로 간주.
		// do u understand?? 유남쌩? 나중에 내가 읽어도 이해가 갈라나.. 쩝..
		// 피직의 본은 계산 과정을 매트릭스로 하지 않고 쿼터니언과 트랜스레이션만..
#ifdef USING_RSC_PHYSIQUE_QT
		if( bUsePhysique && pControllerNode->m_pActorNode)
			pControllerNode->m_pActorNode->SetUseForPhysiqueBone( true);
#endif
	}

	m_frameController.SetAnimation( pAnimation );
}

void RAnimationController::Disconnect()
{
	if(!m_pAnimation)
		return ;

	m_pAnimation = NULL;
	m_nodeControllers.clear();

	m_frameController.Reset();
}

void RAnimationController::SetActorUseVertexAnimation( RAnimation* pAnimation )
{
	m_pActor->SetUseVertexAnimation( false );
	if(	pAnimation->GetResource()->GetAnimationType() == RAniType_Vertex )
		m_pActor->SetUseVertexAnimation( true );
}

bool RAnimationController::SetAnimation( const char* szAnimation, RPLAYINFO* pPlayInfo )
{
	if( !m_pActor->GetUsable() )
	{
		_ASSERT( FALSE );	// 로딩중 ?
		return false;
	}

	if(!m_pActor->m_pMesh)
		return false;

	RAnimation* pAniSet = m_pActor->m_pMesh->GetAnimationMgr()->GetAnimation( szAnimation );
	if(!pAniSet) return false;

	return SetAnimation( pAniSet, pPlayInfo );
}

bool RAnimationController::SetAnimation(RAnimation* pAnimation, RPLAYINFO* pPlayInfo )
{
	Reset();

	if(!pAnimation)
		return false;

	SetActorUseVertexAnimation( pAnimation );

	// reset play information
	if (pPlayInfo)
	{
		// 새로운 play info 이면 값을 설정해준다.
		if (pPlayInfo != &m_playInfo)
		{
			m_playInfo = *pPlayInfo;
		}

		// 새로운 node set 이면 값을 설정한다.
		if (m_playInfo.pNodeSet && m_playInfo.pNodeSet != &m_nodeSelection)
		{
			m_nodeSelection = *m_playInfo.pNodeSet;
			m_playInfo.pNodeSet = &m_nodeSelection;
		}
	}
	else
	{
		//m_playInfo.fPlaySpeed = GetSpeed();
		//m_playInfo.fWeight = GetWeight();
		//m_playInfo.blendMode = GetBlendMode();

		m_nodeSelection.clear();
		m_playInfo.pNodeSet = NULL;
		m_playInfo.fBlendIn = 0;
		m_playInfo.fBlendOut = 0;
	}

	ConnectAnimation(pAnimation, m_playInfo.pNodeSet );
	ActivateNodes(m_playInfo.pNodeSet);

	//SetSpeed( m_playInfo.fPlaySpeed );
	//SetWeight( m_playInfo.fWeight );
	//SetBlendMode( m_playInfo.blendMode );
	//m_fBlendIn = m_playInfo.fBlendIn;
	//m_fBlendOut = m_playInfo.fBlendOut;

	return true;
}

RAnimationControllerNode* RAnimationController::GetNodeController(int i)
{
	return &m_nodeControllers[i];
}

RAnimationControllerNode* RAnimationController::GetNodeController(const string& strName)
{
	if( m_pAnimation==NULL ) return NULL;
	RAnimationResource* pResource = m_pAnimation->GetResource();
	int nIndex = pResource->GetNodeIndex(strName);
	if(nIndex<0 || nIndex>=(int)m_nodeControllers.size()) return NULL;

	return &m_nodeControllers[nIndex];
}

// 현재의 프레임으로 애니메이션 데이터를 갱신한다
void RAnimationController::UpdateAnimation()
{
	RPFC_THISFUNC;
	for(size_t i=0;i<GetActiveNodeCount();++i)
	{
		RAnimationControllerNode* pControllerNode = m_activeControllers[i];
		pControllerNode->Update( m_frameController.GetFrame());
	}
}

void RAnimationController::Update(RCameraSceneNode* pCamera)
{
	RPFC_THISFUNC;
	UpdateAnimation();

	for(unsigned int i=0;i<m_activeControllers.size();++i)	// TODO: lod set 으로 부터
	{
		RAnimationControllerNode* pController = m_activeControllers[i];
		if(!pController)
			continue;

		RActorNode* pActorNode = m_pActor->GetActorNode(pController->GetNodeName());

		if (!pActorNode)
			continue;

		// 부모가 있는, 계층을 타지 않는 액터노드는 업데이트 하지 않는다.
		if (pActorNode->QueryAttribute(RSNA_NO_HIERARCHY_UPDATE) && pActorNode->GetParent() != NULL)
			continue;

		RQuaternion targetRotation		= pController->GetCurrentRotation();
		RVector		targetTranslation	= pController->GetCurrentTranslation();
		RVector		targetScale			= pController->GetCurrentScale();
		float		targetVisibility	= pController->GetVisibility();

		// overwrite 모드이면 덮어써버린다
		if(m_playInfo.blendMode==RBLENDMODE_OVERWRITE)
		{
			pActorNode->SetRotation( targetRotation );
			pActorNode->SetPosition( targetTranslation );
			pActorNode->SetScale( targetScale );
			pActorNode->SetVisibility( targetVisibility );

		}else if ( m_playInfo.blendMode==RBLENDMODE_BLEND )
		// 블렌딩해야한다
		{
			RQuaternion curRotation = pActorNode->GetRotation();	/// TODO: 속도가 많이 걸리는 작업, 최적화 하자
			RVector	curTranslation = pActorNode->GetPosition();
			RVector	curScale = pActorNode->GetScale();
			float	curVisibility = pActorNode->GetVisibility();

/* // 캐릭터 기본 포즈로 부터 블렌딩
			RQuaternion curRotation;
			RVector	curTranslation;
			RVector	curScale;
			pActorNode->m_pMeshNode->m_matLocal.Decompose(curScale,curTranslation,curRotation);
*/

			float fCurrentWeight = m_playInfo.fWeight;

			if( m_playInfo.fBlendIn>0 )
			{
				float fBlendInWeight = (float(m_frameController.GetFrame())/4800.f / m_playInfo.fBlendIn);
				fCurrentWeight = min( fCurrentWeight, fBlendInWeight );
			}

			if( m_playInfo.fBlendOut>0 )
			{
				float fBlendOutWeight = (float(m_frameController.GetMaxFrame() - m_frameController.GetFrame())/4800.f / m_playInfo.fBlendOut);
				fCurrentWeight = min( fCurrentWeight, fBlendOutWeight );
			}

// 			if( fCurrentWeight> 0 )
			{
				RQuaternion blendedRotation;
				blendedRotation.Slerp(curRotation,targetRotation,fCurrentWeight);

				RVector blendedPosition = fCurrentWeight* targetTranslation + (1.f-fCurrentWeight)*curTranslation;

				RVector blendedScale = fCurrentWeight* targetScale + (1.f-fCurrentWeight)*curScale;
				
				float blendedVisibility = fCurrentWeight* targetVisibility + (1.f-fCurrentWeight)*curVisibility;

				pActorNode->SetRotation(blendedRotation);
				pActorNode->SetPosition(blendedPosition);
				pActorNode->SetScale( blendedScale );
				pActorNode->SetVisibility( blendedVisibility );
			}

		}else
		{
			_ASSERT(FALSE);
		}
	}
}

}