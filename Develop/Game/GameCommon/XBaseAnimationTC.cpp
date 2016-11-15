#include "StdAfx.h"
#include "XBaseAnimationTC.h"
#include "RActor.h"
#include "RSceneNode.h"
#include "RAnimation.h"
#include "REngine.h"
#include "MLocale.h"

XBaseAnimationTC::XBaseAnimationTC( XCharacter* pCharacter )
:XTransformController(pCharacter)
{
	for(int i = 0; i < BAI_MAX; ++i)
		m_pRAnimationController[i] = NULL;

	if(m_pActor == NULL)
		return;

	for(int i = 0; i < BAI_MAX; ++i)
	{
		m_pRAnimationController[i] = new XRAnimationController(RSNCTRL_CUSTOMUPDATE, RSNCTRL_CUSTOMUPDATE);

		m_pActor->AddController(m_pRAnimationController[i]);
	}

	// 처음에는 0번째것만 쓰인다.
	m_pCurrentRAnimationController = m_pRAnimationController[0];
	m_pPrevRAnimationController = NULL;
}

XBaseAnimationTC::~XBaseAnimationTC( void )
{
	m_pCurrentRAnimationController = NULL;
	m_pPrevRAnimationController = NULL;

	for(int i = 0; i < BAI_MAX; ++i)
	{
		SAFE_DELETE(m_pRAnimationController[i]);
	}
}

void XBaseAnimationTC::PreUpdate( float fDelta )
{
	if(m_pCurrentRAnimationController)
		m_pCurrentRAnimationController->OnPreUpdate();
}

void XBaseAnimationTC::Update( float fDelta )
{
	// 현재 컨트롤러가 없다면 업데이트 하지 않는다.
	if(m_pCurrentRAnimationController == NULL)
		return;

	// 로딩이 안되었나?
	CheckReservedAnimation();

	// update previous controller if exists
	if (m_pPrevRAnimationController != NULL)
	{
		_ASSERT(RAT_NONE != m_eAnimationBlendType);
		_ASSERT(m_fRequestedBlendTime > 0);

		m_fElapsedBlendTime += fDelta;
		float fWeight = m_fElapsedBlendTime / m_fRequestedBlendTime;

		if (fWeight > 1.f)	// 블랜딩 완료
		{
			// 초기화
			m_fRequestedBlendTime = 0.f;
			m_fElapsedBlendTime = 0.f;
			m_pPrevRAnimationController->Reset();
			m_pPrevRAnimationController = NULL;

			// 현재 컨트롤러 설정
			m_pCurrentRAnimationController->SetWeight(1.f);
			m_pCurrentRAnimationController->SetBlendMode(RBLENDMODE_OVERWRITE);
		}
		else	// 블랜딩 중
		{
			//if (RAT_IMMEDIATE_BLEND == m_eAnimationBlendType)
			{
				m_pPrevRAnimationController->SetWeight(1.f);
				m_pPrevRAnimationController->OnUpdate();
			}

			// 현재 컨트롤러 설정
			//mlog("POK : blending %s, %f\n", m_pCurrentRAnimationController->GetAnimation()->GetAliasName().c_str(), fWeight);
			m_pCurrentRAnimationController->SetWeight(fWeight);
			m_pCurrentRAnimationController->SetBlendMode(RBLENDMODE_BLEND);

			// if you do not want blend (for compare test)
			//m_pCurrentRAnimationController->SetWeight(1.f);
			//m_pCurrentRAnimationController->SetBlendMode(RBLENDMODE_OVERWRITE);
		}
	}

	m_pCurrentRAnimationController->OnUpdate();
}

void XBaseAnimationTC::PostUpdate( float fDelta )
{
	// 블렌딩중이라면...
	if (m_pPrevRAnimationController != NULL)
	{
		// 무기 블렌딩
		UpdateWeaponBlend(PLAYER_PARTS_SLOT_RWEAPON);
		UpdateWeaponBlend(PLAYER_PARTS_SLOT_LWEAPON);
	}
}

bool XBaseAnimationTC::SetAnimation( tstring& strAniName, bool bForce/*=false*/, bool bSameFrame /*= false*/ )
{
	if(m_pCurrentRAnimationController == NULL)
		return false;

	// 애니메이션 사용이 가능한가?
	ANIMATION_CHECK_STATE eState = CheckAnimationUsable(strAniName);
	if(eState == AS_NONE)
		return false;
	else if( eState != AS_LOADED)
	{
		m_strReservedAni = strAniName;
		return true;
	}

	int nFrame = 0;
	if(bSameFrame)
	{
		nFrame = m_pCurrentRAnimationController->GetFrame();
	}
	// 파일기반 Animation 관련
	else if (strAniName == m_strLastAni) 
	{
		// bForce가 true이면 프레임을 초기화한다.
		if (bForce)
		{
			// 애니메이션 재설정
			m_pCurrentRAnimationController->ResetEvent();
			m_pCurrentRAnimationController->SetFrame(0);

			// 끝난 실행을 다시 시작한다.
			m_pCurrentRAnimationController->Play();

			//mlog("강제 ");
		}

		//mlog("중복 (%s)\n", strAniName.c_str());
		return true;
	}

	// 애니메이션 확인
	RAnimation* pAnimation = m_pActor->GetAnimation(MLocale::ConvTCHARToAnsi(strAniName.c_str()).c_str());
	if( pAnimation == NULL)
	{
		return false;
	}

	RPLAYINFO playInfo;
	playInfo.fPlaySpeed = pAnimation->GetSpeed();

	// blending
	m_pPrevRAnimationController = NULL;
	RAnimation* pPrevAnimation = m_pCurrentRAnimationController->GetAnimation();
	if (pPrevAnimation)
	{
		BLEND_DATA blendDatat = m_pActor->GetAnimationBlendData(MLocale::ConvAnsiToTCHAR(pPrevAnimation->GetAliasName().c_str()), strAniName);

		m_fRequestedBlendTime = blendDatat.fDurationTime;
		m_fElapsedBlendTime = 0.f;

		// 블랜딩 요청시간이 있다면
		if (m_fRequestedBlendTime > 0.f)
		{
			//mlog("새로운 애니 블랜딩 %s -> %s(%f)\n", pPrevAnimation->GetAliasName().c_str(), strAniName.c_str(), m_fRequestedBlendTime);
			m_pPrevRAnimationController = m_pCurrentRAnimationController;
			if (m_pRAnimationController[BAI_BASE_1] != m_pCurrentRAnimationController)
			{
				m_pCurrentRAnimationController = m_pRAnimationController[BAI_BASE_1];
			}
			else
			{
				m_pCurrentRAnimationController = m_pRAnimationController[BAI_BASE_2];
			}
		}
	}

	// animation setting
	if(m_pCurrentRAnimationController->SetAnimation(MLocale::ConvTCHARToAnsi(strAniName.c_str()).c_str(), &playInfo) == false)
		return false;

	m_strLastAni = strAniName;
	m_pCurrentRAnimationController->Play();
	m_pCurrentRAnimationController->SetFrame(nFrame);

	if(bSameFrame)
	{
		m_pCurrentRAnimationController->DisableAniamtionEventPreFrame(nFrame);
	}

	//mlog("기본 애니메이션 %s(%d)\n", MLocale::ConvTCHARToAnsi(strAniName.c_str()).c_str(), nFrame);

	m_strReservedAni.clear();
	m_vecActiveNodeName.clear();

	// 버텍스 애니메이션 체크 
	m_pActor->SetUseVertexAnimation(CheckUseVertexAnimation(m_pCurrentRAnimationController));

	// 애니메이션 AABB 셋팅
	m_pActor->SetLocalAABB( m_pCurrentRAnimationController->GetAnimation()->GetResource()->GetMaxAniBoundingBox() );
	return true;
}

float XBaseAnimationTC::GetAnimationSpeed()
{
	if(m_pCurrentRAnimationController == NULL)
		return 0.0f;

	return m_pCurrentRAnimationController->GetSpeed();

}

void XBaseAnimationTC::SetAnimationSpeed( float fSpeed )
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->SetSpeed(fSpeed);

}

void XBaseAnimationTC::SetAnimationFrame( unsigned int nFrame )
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->SetFrame(nFrame);
}

float XBaseAnimationTC::GetMaxFrameToSecond()
{
	if(m_pCurrentRAnimationController == NULL)
		return 0.0f;

	DWORD dwMaxFrame = m_pCurrentRAnimationController->GetMaxFrame();
	float fSpeed = m_pCurrentRAnimationController->GetSpeed();

	return dwMaxFrame / 4.8f / (fSpeed * 1000.0f);
}

float XBaseAnimationTC::GetCurFrameToSecond()
{
	if(m_pCurrentRAnimationController == NULL)
		return 0.0f;

	DWORD dwFrame = m_pCurrentRAnimationController->GetFrame();
	float fSpeed = m_pCurrentRAnimationController->GetSpeed();

	return dwFrame / 4.8f / (fSpeed * 1000.0f);
}

void XBaseAnimationTC::PauseAnimation()
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->Pause();
	//m_pActor->Pause();

}

void XBaseAnimationTC::StopAnimation()
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->Stop();
	//m_pActor->Stop();

}

void XBaseAnimationTC::PlayAnimation()
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->Play();
	//m_pActor->Play();
}

tstring XBaseAnimationTC::GetAnimationName()
{
	if(m_pCurrentRAnimationController == NULL)
		return _T("");

	if (!m_strReservedAni.empty()) return m_strReservedAni;

	RAnimation* pAni = m_pCurrentRAnimationController->GetAnimation();
	if (pAni) 
		return MLocale::ConvAnsiToTCHAR(pAni->GetAliasName().c_str());

	return _T("");
}

tstring XBaseAnimationTC::GetPreAnimaionName()
{
	if(m_pPrevRAnimationController == NULL)
		return _T("");

	RAnimation* pAni = m_pPrevRAnimationController->GetAnimation();
	if (pAni) 
		return MLocale::ConvAnsiToTCHAR(pAni->GetAliasName().c_str());

	return _T("");
}

int XBaseAnimationTC::GetAniFrame()
{
	if(m_pCurrentRAnimationController == NULL)
		return 0;

	return m_pCurrentRAnimationController->GetFrame();
}

int XBaseAnimationTC::GetAniMaxFrame()
{
	if(m_pCurrentRAnimationController == NULL)
		return 0;

	return m_pCurrentRAnimationController->GetMaxFrame();
}

bool XBaseAnimationTC::IsAnimationLoop()
{
	if(m_pCurrentRAnimationController == NULL)
		return false;

	return m_pCurrentRAnimationController->IsAnimationLoop();
}

bool XBaseAnimationTC::IsPlayDone()
{
	if(m_pCurrentRAnimationController == NULL)
		return false;

	return m_pCurrentRAnimationController->IsPlayDone();

}

bool XBaseAnimationTC::IsOncePlayDone()
{
	if(m_pCurrentRAnimationController == NULL)
		return false;

	return m_pCurrentRAnimationController->IsOncePlayDone();

}

void XBaseAnimationTC::CheckReservedAnimation()
{
	if(m_strReservedAni.empty())
		return;

	// 모델이나 애니메이션이 로딩 되었는가?
	ANIMATION_CHECK_STATE eState = CheckAnimationUsable(m_strReservedAni);
	if(eState == AS_NONE)
	{
		dlog("예약된 애니메이션 에러 %s\n", MLocale::ConvTCHARToAnsi(m_strReservedAni.c_str()).c_str());
		m_strReservedAni.clear();
		return;
	}
	else if(eState != AS_LOADED)
		return;

	if (SetAnimation( m_strReservedAni ))
	{
		m_strReservedAni.clear();
	}
	else
	{
		//_ASSERT(0);
		// by pok. 09. 02. 20
		// article_flower_01.elu 같은경우에 idle 애니메이션이 없어서 어써션이 걸립니다
		// 어써션대신 로그로 대체합니다.
		//mlog("%s does not have %s ani file\n", m_pActor->m_pMesh->GetName().c_str(), m_strReserverdAni.c_str());
	}
}

bool XBaseAnimationTC::IsNodeController( tstring& strDummyName )
{
	RAnimationControllerNode* pControllerNode = m_pCurrentRAnimationController->GetNodeController(MLocale::ConvTCHARToAnsi(strDummyName.c_str()));
	if(pControllerNode)
	{
		RAnimationNode* pAnimationNode = pControllerNode->GetAnimationNode();
		if(pAnimationNode &&
			pAnimationNode->m_positionKeyTrack.size() > 2)
		{
			return true;
		}
	}

	return false;
}

RActorNode* XBaseAnimationTC::GetAnimatedActorNodeTransform( tstring& strNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace )
{
	_rOut.MakeIdentity();
	RActorNode* pActorNode = m_pActor->GetActorNode(MLocale::ConvTCHARToAnsi(strNodeName.c_str()).c_str());
	if (NULL != pActorNode &&
		m_pCurrentRAnimationController != NULL)
	{
		//////////////////////////////////////////////////////////////////////////
		// 부모가 없거나, Actor일때에만 이 함수 사용 허용(무분별하게 사용을 금지 하기 위합니다.)
		if(pActorNode->GetParent() &&
			pActorNode->GetParent() != m_pActor)
			return NULL;
		//////////////////////////////////////////////////////////////////////////

		DWORD nFrame = m_pCurrentRAnimationController->GetFrame();
		if (m_pActor->GetUpdatedFrame() != REngine::GetDevice().GetFrameCount())
		{
			bool bPlayDone = false;
			m_pCurrentRAnimationController->GetNextFrameAndState(REngine::GetTimer().GetElapsedTime(), nFrame, bPlayDone);
		}

		switch(eTransformSpace)
		{
		case E_TS_WORLD:
			{
				m_pCurrentRAnimationController->GetWorldTransformAt(nFrame, _rOut, pActorNode);
			}
			break;
		case E_TS_LOCAL:
			{
				m_pCurrentRAnimationController->GetLocalTransformAt(nFrame, _rOut, pActorNode);
			}
			break;
		case E_TS_LOCAL_RESULT:
			{
				m_pCurrentRAnimationController->GetResultTransformAt(nFrame, _rOut, pActorNode);
			}
			break;
		}
	}

	return pActorNode;
}

RAnimation * XBaseAnimationTC::GetAnimation()
{
	if(m_pCurrentRAnimationController == NULL)
		return NULL;

	return m_pCurrentRAnimationController->GetAnimation();
}

bool XBaseAnimationTC::PushActiveActorNode( tstring& strNodeName )
{
	// 애니메이션이 셋팅이 되면 초기화가 되므로 애니메이션이 변경이 될때마다 셋팅을 해주어야 합니다.

	if(m_pCurrentRAnimationController == NULL)
		return false;

	if(m_pCurrentRAnimationController->PushActiveNode(strNodeName))
	{
		m_vecActiveNodeName.push_back(strNodeName);
		return true;
	}

	return false;
}

bool XBaseAnimationTC::CheckDuplicationActiveNodeName( tstring& strNodeName )
{
	for(vector<tstring>::iterator it = m_vecActiveNodeName.begin(); it != m_vecActiveNodeName.end(); ++it)
	{
		if((*it) == strNodeName)
			return true;
	}

	return false;
}

bool XBaseAnimationTC::CheckUseVertexAnimation(XRAnimationController * pAnimationController)
{
	if(pAnimationController == NULL)
		return false;

	RAnimation * pAnimation = pAnimationController->GetAnimation();
	if(pAnimation == NULL)
		return false;

	if(	pAnimation->GetResource()->GetAnimationType() == RAniType_Vertex )
		return true;

	return false;
}

void XBaseAnimationTC::CloseEvent()
{
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->CloseEvent();
}

bool XBaseAnimationTC::IsActiveNodeController( tstring& strNodeName )
{
	if(m_pCurrentRAnimationController == NULL)
		return false;

	return m_pCurrentRAnimationController->IsActiveNode(strNodeName);
}

bool XBaseAnimationTC::GetActorNodePosByFrame( tstring& strNodeName, unsigned int nFrame, vec3& vOut )
{
	if(m_pCurrentRAnimationController == NULL)
		return false;

	MMatrix matResult;
	matResult.MakeIdentity();
	RActorNode* pActorNode = m_pActor->GetActorNode(MLocale::ConvTCHARToAnsi(strNodeName.c_str()).c_str());
	if (NULL != pActorNode &&
		m_pCurrentRAnimationController != NULL)
	{
		m_pCurrentRAnimationController->GetWorldTransformAt(nFrame, matResult, pActorNode);
		vOut = matResult.GetTranslation();
		return true;	
	}

	return false;
}

int XBaseAnimationTC::GetAniFrameWithUpdateTime()
{
	if(m_pCurrentRAnimationController == NULL)
		return 0;


	DWORD nFrame = m_pCurrentRAnimationController->GetFrame();
	if (m_pActor->GetUpdatedFrame() != REngine::GetDevice().GetFrameCount())
	{
		bool bPlayDone = false;
		m_pCurrentRAnimationController->GetNextFrameAndState(REngine::GetTimer().GetElapsedTime(), nFrame, bPlayDone);
	}

	return nFrame;
}

int XBaseAnimationTC::GetPreAniFrame()
{
	if(m_pPrevRAnimationController == NULL)
		return 0;

	bool bPlayDone = false;
	DWORD nFrame = m_pPrevRAnimationController->GetFrame();
	m_pPrevRAnimationController->GetNextFrameAndState(REngine::GetTimer().GetElapsedTime(), nFrame, bPlayDone);

	return nFrame;
}

float XBaseAnimationTC::GetPreAniFrameToSec( unsigned int nFrame )
{
	float fSpeed = 0.0f;

	if(m_pPrevRAnimationController)
		fSpeed = m_pPrevRAnimationController->GetSpeed();

	return nFrame / 4.8f / (fSpeed * 1000.f);
}

void XBaseAnimationTC::GetActiveRActorNodeList( vector<tstring>& vecActorNodeList )
{
	for(vector<tstring>::iterator it = m_vecActiveNodeName.begin(); it != m_vecActiveNodeName.end(); ++it)
	{
		vecActorNodeList.push_back(*it);
	}
}

void XBaseAnimationTC::CheckActiveActorNode( RActorNode* pActorNode, bool bForceUpdate /*= false*/  )
{
	if(CheckDonotPushActiveNodeName(MLocale::ConvAnsiToTCHAR(pActorNode->GetNodeName().c_str())) ||
		CheckDuplicationActiveNodeName(MLocale::ConvAnsiToTCHAR(pActorNode->GetNodeName().c_str())))
		return;

	// 없네.....
	PushActiveActorNode(MLocale::ConvAnsiToTCHAR(pActorNode->GetNodeName().c_str()));

	// 애니메이션 업데이트 하셨는가?
	if(m_pActor->GetAnimationUpdated() || bForceUpdate)
	{
		// 안했으면...
		// 업데이트 하자.
		pActorNode->UpdateAllParentDependentValues();
		pActorNode->UpdateFinalResultTransforms();
	}

	// 부모도 확인하자.
	// 재귀함수입니다. 조심해야 합니다.
	RActorNode* pParentActorNode = pActorNode->GetParentActorNode();
	if(pParentActorNode != NULL)
		CheckActiveActorNode(pParentActorNode);
}

void XBaseAnimationTC::CheckActiveActorNode( tstring& strNodeName )
{
	RActorNode* pActorNode = m_pActor->GetActorNode(MLocale::ConvTCHARToAnsi(strNodeName.c_str()).c_str());
	if(pActorNode)
		CheckActiveActorNode(pActorNode);
}

bool XBaseAnimationTC::IsAnimationUsable()
{
	return (CheckAnimationLoadingComplete(m_strLastAni) == AS_LOADED);
}

vector<ANIMATION_EVENT>* XBaseAnimationTC::GetAnimationEvent()
{
	if(m_pCurrentRAnimationController)
		return &m_pCurrentRAnimationController->GetAnimationEvent();

	return NULL;
}

void XBaseAnimationTC::ResetAniamtionSpeed()
{
	// 원래 스피드로 돌린다.
	if(m_pCurrentRAnimationController == NULL)
		return;

	m_pCurrentRAnimationController->ResetAniamtionSpeed();
}

float XBaseAnimationTC::GetOrgAniamtionSpeed()
{
	// 원래 스피드값을 준다.
	if(m_pCurrentRAnimationController == NULL)
		return false;

	return m_pCurrentRAnimationController->GetOrgAniamtionSpeed();
}

void XBaseAnimationTC::UpdateWeaponBlend(XPlayerPartsSlotType slotType)
{
	if(m_pPrevRAnimationController == NULL ||
		m_pCurrentRAnimationController == NULL)
		return;

	vector<RActorNode*> weaponActorNode;
	vector<tstring> weaponActorNodeName;
	m_pActor->GetPartsActorNode(slotType, weaponActorNode, weaponActorNodeName);

	tstring strParentName;
	for(vector<RActorNode*>::iterator it = weaponActorNode.begin(); it != weaponActorNode.end(); ++it)
	{
		if((*it) && (*it)->GetParentActorNode())
		{
			strParentName = MLocale::ConvAnsiToTCHAR((*it)->GetParentActorNode()->GetNodeName().c_str());
		}
	}

	CheckWeaponBlendVisibility(strParentName);
	CheckWeaponBlendVisibility(GetWeaponParentName(strParentName, "_b"));
	CheckWeaponBlendVisibility(GetWeaponParentName(strParentName, "_w"));
	CheckWeaponBlendVisibility(GetWeaponParentName(strParentName, "_c"));

}

void XBaseAnimationTC::CheckWeaponBlendVisibility( tstring strParentName )
{
	if(strParentName.empty())
		return;

	RActorNode* pActorNode = m_pActor->GetActorNode(MLocale::ConvTCHARToAnsi(strParentName.c_str()).c_str());
	if(pActorNode == NULL)
		return;

	RAnimationControllerNode* pPrevControllerNode = m_pPrevRAnimationController->GetControllerNode(strParentName);
	RAnimationControllerNode* pCurrentControllerNode = m_pCurrentRAnimationController->GetControllerNode(strParentName);

	if(pPrevControllerNode && 
		pCurrentControllerNode == NULL)
	{
		pActorNode->SetVisibility(0.0f);
		return;
	}

	if(pPrevControllerNode == NULL ||
		pCurrentControllerNode == NULL)
		return;

	if(pPrevControllerNode->GetVisibility() != pCurrentControllerNode->GetVisibility())
	{
		pActorNode->SetVisibility(pCurrentControllerNode->GetVisibility());
	}
}

tstring XBaseAnimationTC::GetWeaponParentName( tstring& strParentName, string strPostfix )
{
	return strParentName + MLocale::ConvAnsiToTCHAR(strPostfix.c_str());
}

bool XBaseAnimationTC::CheckDonotPushActiveNodeName( tstring& strNodeName )
{
	// 활성화가 되면 안되는 노드들 체크
	if(strNodeName == _T("dummy_loc"))
		return true;

	return false;
}
