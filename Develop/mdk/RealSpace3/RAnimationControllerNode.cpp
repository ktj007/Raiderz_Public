#include "StdAfx.h"
#include "RAnimationControllerNode.h"
#include "RActorNode.h"
#include "RAnimationNode.h"
#include "RAnimationController.h"
#include "RMeshNode.h"

namespace rs3 {

//RAnimationControllerNode::RAnimationControllerNode(RSceneNode* pTargetNode) : m_pAniNode(NULL), m_pParent(NULL)
//{
//	m_pSceneNode = pTargetNode;
//}
//
RAnimationControllerNode::RAnimationControllerNode()
: m_pAniNode(NULL), m_fVisibility(1.f), m_pParent(NULL), m_bUpdateAnimation(true)
//														,m_bAnimationParentMatch(true)
{
//	m_pSceneNode = NULL;
}

RAnimationControllerNode::~RAnimationControllerNode(void)
{
}

void RAnimationControllerNode::Disconnect()
{
	m_pActorNode = NULL;
	SetAnimationNode(NULL);
}

void RAnimationControllerNode::SetAnimationNode(RAnimationNode* pNode)
{
	m_szName =  pNode ? pNode->m_Name.c_str() : "";
	m_pAniNode = pNode;
	m_nRotKeyIterator = 0;
	m_nPosKeyIterator = 0;
	m_nScaleKeyIterator = 0;
	m_nVisKeyIterator = 0;

	m_fVisibility = 1;
}

void RAnimationControllerNode::UpdateVertexAnimation(unsigned int nFrame)
{
	if( m_pAniNode->m_nVertexVCnt &&
		m_pActorNode->m_pMeshNode->m_nPointCnt == m_pAniNode->m_nVertexVCnt ) // vertex animation
	{
		// vertex animation 시 bounding_box 갱신
		// 애니매이션 마다 바운딩 박스가 다를 수 있는데 여기서 셋팅해버려도 되나? - charmke
		m_pActorNode->m_pMeshNode->m_boundingBox = m_pAniNode->m_bboxVertexAni;
		m_pActorNode->ReserveUpdateVertexBuffer(m_pAniNode, nFrame);
	}
	else
	{
		m_pActorNode->ReserveUpdateVertexBuffer(NULL, 0);
	}
}

void RAnimationControllerNode::Update(unsigned int nFrame)
{
//	RPFC_THISFUNC;

//	if(!m_pAniNode) return;

	bool bChanged = false;

	bChanged |= m_pAniNode->GetRotValue(&m_currentRotation,nFrame,m_nRotKeyIterator);
	bChanged |= m_pAniNode->GetPosValue(&m_currentTranslation, nFrame,m_nPosKeyIterator);
	bChanged |= m_pAniNode->GetScaleValue(&m_currentScale,nFrame,m_nScaleKeyIterator);

	m_pAniNode->GetVisValue(&m_fVisibility,nFrame,m_nVisKeyIterator );

	UpdateVertexAnimation(nFrame); // strategy 로 분리 ?
}

RQuaternion RAnimationControllerNode::GetRotationAt(DWORD nFrame)
{
	RQuaternion rotation;
	if(!m_pAniNode->GetRotValue(&rotation,nFrame,m_nRotKeyIterator))
		rotation = m_currentRotation;
	return rotation;
}

RVector RAnimationControllerNode::GetTranslationAt(DWORD nFrame)
{
	RVector translation;
	if(!m_pAniNode->GetPosValue(&translation, nFrame,m_nPosKeyIterator))
		translation = m_currentTranslation;

	return translation;
}

RVector RAnimationControllerNode::GetScaleAt(DWORD nFrame)
{
	RVector scale;
	if(!m_pAniNode->GetScaleValue(&scale,nFrame,m_nScaleKeyIterator))
		scale = RVector(1,1,1);
	return scale;
}

void RAnimationControllerNode::GetLocalTransformAt(DWORD nFrame, RMatrix& outMatrix)
{
	RQuaternion rotation = GetRotationAt(nFrame);
	RVector translation = GetTranslationAt(nFrame);
	RVector scale = GetScaleAt(nFrame);

	RMatrix matLocalScale;
	matLocalScale.SetScaleMatrix(scale);

	RMatrix matRotateTranslation;
	rotation.SetMatrixRotation(matRotateTranslation);
	matRotateTranslation._41 = translation.x;
	matRotateTranslation._42 = translation.y;
	matRotateTranslation._43 = translation.z;

	matRotateTranslation._14 = 0;
	matRotateTranslation._24 = 0;
	matRotateTranslation._34 = 0;
	matRotateTranslation._44 = 1;

	matLocalScale.MultiplyTo(matRotateTranslation, outMatrix);	// out = scale * rotTranslation
}

RActorNode* RAnimationControllerNode::GetNode()
{
	return m_pActorNode;
}
}
