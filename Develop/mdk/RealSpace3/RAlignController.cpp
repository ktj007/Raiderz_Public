#include "StdAfx.h"

#include "RAlignController.h"
#include "RActorNode.h"
#include "RActor.h"

namespace rs3 {

RAlignController::RAlignController(void) : RSceneNodeController(RSNCTRL_PRERENDER)
{
	AddAttribute( RCA_STICKY );
}

RAlignController::~RAlignController(void)
{
	m_bEnable = false;
}

void RAlignController::Update(RCameraSceneNode* pCamera /*= NULL*/)
{
	RPFC_THISFUNC;
	RSceneNode *pTarget = m_pSceneNode;
	_ASSERT(pTarget);

	if (pCamera == NULL)
		return;

	// 일반적인 빌보드처럼 보이게 만든다. 2D 사각형처럼 ...
	const RMatrix& matWorld = pTarget->GetWorldTransform();
	RVector wP(matWorld._41,matWorld._42,matWorld._43);

	const RVector &vU = pCamera->GetUp();
	const RVector &vD = pCamera->GetDirection();

	RMatrix matResult;
	matResult.SetLocalMatrix(wP, vD, vU);

	RMatrix matInvParent;
	pTarget->GetParent()->GetWorldTransform().GetInverse(&matInvParent);
	RMatrix matLocal = matResult * matInvParent;

	// 뼈의 up vector 가 local의 x 축이라 이렇게 되었다. -> Z축으로?
	RVector localDir = RVector(matLocal._21,matLocal._22,matLocal._23);
	RVector localUp = RVector(matLocal._31,matLocal._32,matLocal._33);
	pTarget->SetDirection(localDir,localUp);

	// update children
	RSceneNodeUpdateTransformVisitor updateChildrenVisitor;
	pTarget->Traverse( &updateChildrenVisitor);
}

}