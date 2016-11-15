#include "StdAfx.h"
#include "RActorAlignController.h"
#include "RCameraSceneNode.h"
#include "RActor.h"

namespace rs3 {

//////////////////////////////////////////////////////////////////////////
// RActorAlignController
// align controller 들, scale 에 대한 처리는 되어있지 않다

RActorAlignController::RActorAlignController(RActor* pActor) : RSceneNodeController(RSNCTRL_PRERENDER), m_pActor(pActor)
{
	AddAttribute( RCA_STICKY );
}

void RActorAlignController::GetResultTransform( RMatrix& outResult, const RMatrix& matWorld, const RCameraSceneNode* pCamera )
{
	// 최종목표변환 = 방향이 Target을 향하고 up이 (0,0,1)인 변환

	RVector worldPos(matWorld._41,matWorld._42,matWorld._43);
	RVector cameraDir(pCamera->GetWorldDirection());

	outResult.SetLocalMatrix(worldPos, cameraDir, RVector::AXISZ);
}

void RActorAlignController::Update(RCameraSceneNode* pCamera)
{
	RPFC_THISFUNC;
	// 이때 이미 애니메이션이 있다면 UpdateForRender 어야한다.
	_ASSERT( m_pActor->GetAnimation()==NULL || m_pActor->GetUpdatedFrame()==REngine::GetDevice().GetFrameCount() );

	if( pCamera )
	{
		const RMatrix& matWorld = m_pSceneNode->GetWorldTransform();
		RVector scale;
		matWorld.ExtractScale( scale );

		RMatrix matResult;
		GetResultTransform( matResult, matWorld, pCamera );

 		RMatrix matScale;
 		matScale.SetScaleMatrix( scale );

		// Target ActorNode 의 local tm 은 변화시키지 않고, world tm 만 설정해준다
		SetWorldTransform( matScale * matResult );

		// update children
		RSceneNodeUpdateTransformVisitor updateChildrenVisitor;
		m_pSceneNode->Traverse( &updateChildrenVisitor);
	}
}

//////////////////////////////////////////////////////////////////////////
// RActorZFixedAlignController
RActorZFixedAlignController::RActorZFixedAlignController(RActor* pActor) : RActorAlignController(pActor)
{
	AddAttribute( RCA_STICKY );
}

void RActorZFixedAlignController::GetResultTransform( RMatrix& outResult, const RMatrix& matWorld, const RCameraSceneNode* pCamera )
{
	// 최종목표변환 = z축이 고정된 상태에서 y 방향이 Target을 향하는 변환
	RVector worldPos(matWorld._41,matWorld._42,matWorld._43);
	RVector cameraDir(pCamera->GetWorldDirection());

	// 기본축의 up (z)에 대한 새 방향을 구한다
	RVector up = Normalize(RVector(matWorld._31,matWorld._32,matWorld._33));
	RVector right = Normalize(CrossProduct(cameraDir,up));
	RVector newDir = Normalize(CrossProduct(up,right));
		
	outResult.SetLocalMatrix(worldPos, newDir, up);
}

}