#pragma once

#include "RAnimationNode.h"
#include "RActorNode.h"

namespace rs3 {

class RActorNode;
class RAnimationNode;
class RAnimationController;

class RS_API RAnimationControllerNode
{
	friend RAnimationController;

	const char*		m_szName;	// for debug

//	RAnimationController*		m_pBase;		///< base animation controller
	RAnimationControllerNode*	m_pParent;		///< 애니메이션의 부모

	RAnimationNode* m_pAniNode;					///< 연결된 animation node
	RActorNode*		m_pActorNode;				///< 연결된 actornode
	float			m_fBoneImportance;			///< 뼈의 중요도

	unsigned int	m_nPosKeyIterator;
	unsigned int	m_nRotKeyIterator;
	unsigned int	m_nScaleKeyIterator;
	unsigned int	m_nVisKeyIterator;


	bool			m_bUpdateAnimation;		// 애니메이션 업데이트 여부

	float			m_fVisibility;			// 애니메이션의 투명도 값
	RVector			m_currentScale;			// 애니메이션의 스케일값
	RQuaternion		m_currentRotation;		// 애니메이션의 회전값
	RVector			m_currentTranslation;	// 애니메이션의 이동값
	RMatrix			m_matCurrent;			// 에니메이션에서 수행한 local Matrix 값 ( = 회전 + 이동 )

	RMatrix			m_matResult;			// 에니메이션에서 수행한 최종 Matrix 값. world matrix 는 곱하지 않은상태.

	RQuaternion		m_baseRotation;			// 기본회전값
 	RVector			m_baseTranslation;		// 기본이동값
 	
	RVector			m_baseScale;		// 기본이동값
	RMatrix			m_matBase;				// 에니메이션이 없을 경우 사용하게 자신의 첫 프레임 로컬 메트릭스,

public:
	RAnimationControllerNode();
	virtual ~RAnimationControllerNode(void);

	RAnimationNode* GetAnimationNode();
	void SetAnimationNode(RAnimationNode* pNode);

	void Disconnect();

	void SetEnableAnimationUpdate(bool bEnable)
	{
		m_bUpdateAnimation = bEnable;
	}

	void Update(unsigned int nFrame);
	void UpdateVertexAnimation(unsigned int nFrame);
	void UpdateActorNode();

	const char* GetNodeName();
	RActorNode* GetNode();

	const RQuaternion& GetCurrentRotation();
	const RVector& GetCurrentTranslation();
	const RVector& GetCurrentScale();

	const RQuaternion& GetResultRotation();
	const RVector& GetResultTranslation();

	float GetVisibility();

	bool HasVisibilityAnimation();

	RQuaternion	GetRotationAt(DWORD nFrame);
	RVector		GetTranslationAt(DWORD nFrame);
	RVector		GetScaleAt(DWORD nFrame);

	void		GetLocalTransformAt(DWORD nFrame, RMatrix& outMatrix);
};

//////////////////////////////////////////////////////////////////////////
// 인라인 함수들
inline const char* RAnimationControllerNode::GetNodeName()				{ return m_pAniNode->GetName(); }
inline const RQuaternion& RAnimationControllerNode::GetCurrentRotation()	{ return m_currentRotation; }
inline const RVector& RAnimationControllerNode::GetCurrentTranslation() { return m_currentTranslation; }
inline const RVector& RAnimationControllerNode::GetCurrentScale()		{ return m_currentScale; }
inline float RAnimationControllerNode::GetVisibility()					{ return m_fVisibility; }
inline RAnimationNode* RAnimationControllerNode::GetAnimationNode()		{ return m_pAniNode; }
inline void RAnimationControllerNode::UpdateActorNode()
{
	if(m_bUpdateAnimation && m_pActorNode)
	{
		m_pActorNode->SetVisibility( GetVisibility() );

		m_pActorNode->SetRotation( GetCurrentRotation() );
		m_pActorNode->SetScale( GetCurrentScale() );
		m_pActorNode->SetPosition( GetCurrentTranslation() );

		m_pActorNode->UpdateAllParentDependentValues();
		m_pActorNode->UpdateFinalResultTransforms();
	}
}
}