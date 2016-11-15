#include "StdAfx.h"
#include "RSimpleRotationController.h"
#include "RActorNode.h"

namespace rs3 {

RRotationController::RRotationController(void)
{
	m_bEnable = false;
	AddAttribute( RCA_STICKY );
}

RRotationController::~RRotationController(void)
{
}

void RRotationController::Update(RCameraSceneNode* pCamera /*= NULL*/)
{
	RPFC_THISFUNC;
	RSceneNode* pTarget = m_pSceneNode;

	bool bModelAxis = true;
	if(bModelAxis)
	{
		RVector trAxis;
		
		RMatrix matInv;
		pTarget->GetWorldTransform().GetInverse(&matInv);
		matInv.TransformNormal(m_axis,trAxis);	// model รเ -> local รเ
//		pTarget->m_matResult.TransformNormal(m_axis,trAxis);

		RQuaternion quatRotation(trAxis,m_fAngle);

		RQuaternion targetRotation = pTarget->GetLocalTransform();
		pTarget->SetRotation( quatRotation * targetRotation );

		m_matAffectedTransform = matInv * quatRotation.Conv2Matrix() * pTarget->GetWorldTransform();
	}else
	{
		RQuaternion quatRotation(m_axis,m_fAngle);

		RQuaternion targetRotation = pTarget->GetLocalTransform();
		pTarget->SetRotation( quatRotation * targetRotation );
	}
}

void RRotationController::SetRotation(const RVector& axis, float fAngle)
{
	m_axis = axis;
	m_fAngle = fAngle;
}


}