#include "stdafx.h"
#include "MPxAttacherTC.h"

#include "MPxRagDollTC.h"

#include "RActor.h"

// 디버그용
#include "RDebugPassRenderer.h"
#include "RCameraSceneNode.h"

namespace physx
{

using namespace rs3;

bool MPxAttacherTC::StartAttacher( rs3::RActor* pActorDest, const char* chDestBoneName, const char* chSrcBoneName, float fInterpolation, const MMatrix& matOffsetTransform )
{
	// 본이 존재하는지 체크
	bool bCheckAttach = false;
	if(pActorDest)
	{
		RActorNode* pDestActorNode = pActorDest->GetActorNode(chDestBoneName);
		RActorNode* pSrcActorNode = GetActor()->GetActorNode(chSrcBoneName);

		if(pDestActorNode && pSrcActorNode)
		{
			// 이상 없다.
			bCheckAttach = true;
		}
	}

	// 이상 없으면 데이터 입력 및 시작
	if(bCheckAttach)
	{
		m_bEnabled			= true;

		m_pActorDest		= pActorDest;
		m_strDestBoneName	= chDestBoneName;
		m_strSrcBoneName	= chSrcBoneName;
		m_fInterpolation	= fInterpolation;
		m_matOffsetTransform = matOffsetTransform;
	}

	return bCheckAttach;
}

void MPxAttacherTC::Update( float fDelta )
{
	//캐릭터
	RActor * pOwnerActor = GetActor();
	_ASSERT(pOwnerActor);

	if( !IsAvailableActor(m_pActorDest) )
	{
		EndAttacher();
		return;
	}

	RActorNode* pDestActorNode = m_pActorDest->GetActorNode(m_strDestBoneName.c_str());
	if (!pDestActorNode)
		return;

	RActorNode* pSrcActorNode = pOwnerActor->GetActorNode(m_strSrcBoneName.c_str());
	if (!pSrcActorNode)
		return;

	RMatrix matDestActorNodeWorld;
	DWORD nFrame = m_pActorDest->GetAnimationController().GetFrame();
	pDestActorNode->GetWorldTransformAt(nFrame, matDestActorNodeWorld);

	RMatrix matSrcActorNodeWorld;
	MPxRagDollTC* pRagDollTC = GetTransformController<MPxRagDollTC>();
	if ( pRagDollTC && pRagDollTC->IsEnabled() )
	{
		const RMatrix* pOffsetTM = pRagDollTC->GetAttacherOffsetTM();
		if (pOffsetTM)
		{
			matSrcActorNodeWorld = (*pOffsetTM) * pOwnerActor->GetWorldTransform();
		}
		else
		{
			matSrcActorNodeWorld = pOwnerActor->GetWorldTransform();
		}
	}
	else
	{
		nFrame = pOwnerActor->GetAnimationController().GetFrame();
		pSrcActorNode->GetWorldTransformAt(nFrame, matSrcActorNodeWorld);
	}

	RMatrix matSrcActorWorld = pOwnerActor->GetWorldTransform();		// 플레이어 actor의 월드 매트릭스
	matSrcActorNodeWorld = m_matOffsetTransform * matSrcActorNodeWorld;	// 플레이어 바디 더미의 월드매트릭스

	RMatrix matSrcActorNodeWorldInverse;
	matSrcActorNodeWorld.GetInverse(&matSrcActorNodeWorldInverse);		// 플레이어 바디 더미의 월드인버스

	// Owner Entity 최종좌표 설정
	RMatrix matResult = matSrcActorWorld * matSrcActorNodeWorldInverse * matDestActorNodeWorld;

	// interplation
	m_fElapsed += fDelta;
	if (m_fInterpolation>m_fElapsed)
	{
		RVector3 vSrc(matSrcActorWorld._41, matSrcActorWorld._42, matSrcActorWorld._43);
		RVector3 vDest(matResult._41, matResult._42, matResult._43);
		vDest -= vSrc;
		if (vDest.LengthSq() > 10.f)
			vDest *= (m_fElapsed/m_fInterpolation);
		vDest += vSrc;

		RQuaternion qSrc(matSrcActorWorld);
		RQuaternion qDest(matResult);
		RQuaternion qResult;
		qResult.Slerp(qSrc, qDest, m_fElapsed/m_fInterpolation);

		matResult = qResult.Conv2Matrix();
		matResult.SetTranslation(vDest);
	}

	pOwnerActor->SetTransform(matResult);
	pOwnerActor->UpdateTransform();

	//REngine::GetSceneManager().GetPassRenderer<RDebugPassRenderer>()->AddDebugBox(matResult, 10, 50, D3DCOLOR_XRGB(0, 255, 255));
}

void MPxAttacherTC::EndAttacher()
{
	m_pActorDest = NULL;
	m_fInterpolation = -1.f;
	m_fElapsed = 0.f;
	m_strDestBoneName.clear();
	m_strSrcBoneName.clear();

	m_bEnabled = false;
}

}
