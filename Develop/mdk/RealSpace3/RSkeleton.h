#pragma once
#include "RSceneNode.h"

// 본애니메이션의 instance

namespace rs3 {

enum RBIPID
{
	RBIPID_UNKNOWN = -1,
	RBIPID_ROOT,
	RBIPID_PELVIS,
	  RBIPID_SPINE ,
		RBIPID_LTHIGH,
		  RBIPID_LCALF,
			RBIPID_LFOOT ,
			  RBIPID_LTOE0 ,
		RBIPID_RTHIGH,
		  RBIPID_RCALF,
			RBIPID_RFOOT ,
			  RBIPID_RTOE0 ,
		RBIPID_SPINE1 ,
		  RBIPID_SPINE2 ,
			RBIPID_NECK ,
			  RBIPID_HEAD,
			  RBIPID_LCLAVICLE,
				RBIPID_LUPPERARM ,
				  RBIPID_LFOREARM ,
					RBIPID_LHAND,
			  RBIPID_RCLAVICLE,
				RBIPID_RUPPERARM ,
				  RBIPID_RFOREARM ,
					RBIPID_RHAND,
	// 왼손가락 parent = RBIPID_LHAND
					RBIPID_LFINGER0,
					  RBIPID_LFINGER01,
						RBIPID_LFINGER02,
					RBIPID_LFINGER1,
					  RBIPID_LFINGER11,
						RBIPID_LFINGER12,
					RBIPID_LFINGER2,
					  RBIPID_LFINGER21,
						RBIPID_LFINGER22,
	// 오른손가락 parent = RBIPID_RHAND
					RBIPID_RFINGER0,
					  RBIPID_RFINGER01,
						RBIPID_RFINGER02,
					RBIPID_RFINGER1,
					  RBIPID_RFINGER11,
						RBIPID_RFINGER12,
					RBIPID_RFINGER2,
					  RBIPID_RFINGER21,
						RBIPID_RFINGER22,
	
	// Ponytail parent = RBIPID_HEAD
	RBIPID_PONYTAIL1,
	  RBIPID_PONYTAIL11,
	RBIPID_PONYTAIL2,
	  RBIPID_PONYTAIL21,

	// 꼬리
	RBIPID_TAIL,
	RBIPID_TAIL1,
	RBIPID_TAIL2,
	RBIPID_TAIL3,
	
	// 얼굴, 규칙 약속이 필요
	RBIPID_EYE,
	RBIPID_LIP,

	// 무기
	RBIPID_PROP1,

	RBIPID_END
};

class RMesh;
class RAnimation;
class RAnimationNode;
class RSkeletonController;
class RBlendAnimationController;
class RAnimationFrameInfo;

typedef list<RSkeletonController*> RSKELETONCONTROLLERLIST;

//	class RSkeletonBone 이 있는게 나을까 ?
class RS_API RSkeleton : public RSceneNode
{
	/*
	friend RSkeletonController;

protected:
	RANIMATIONTRANSITIONTYPE	m_transitionType;
//	RBlendAnimationController*	m_pTransitionController;	// 애니메이션의 전환을 위해 블렌드 콘트롤러는 항상 하나 가지고있다.

	RAnimation*	m_pAnimation;
	RAnimationNode*	m_pAnimationNodeTable[RBIPID_END];
	RSKELETONCONTROLLERLIST m_controllers;

	int	m_nFrame;
	int m_nMaxFrame;

	bool		m_bUse[RBIPID_END];				// 사용하는지 여부
	RMatrix		m_matLocal[RBIPID_END];			// 부모뼈의 좌표계에서 자기뼈로 좌표계로의 변환. 뼈의 기본 위치및 방향을 담고있다
	RQuaternion	m_localRotation[RBIPID_END];	// m_matLocal 의 회전성분
	RVector		m_localPosition[RBIPID_END];	// m_matLocal 의 이동성분
	RMatrix		m_matBase[RBIPID_END];			// local좌표계에서 max static frame으로 가는 변환 ( = 부모로부터 곱해져온 local )
	RMatrix		m_matInvBase[RBIPID_END];		// base의 inverse

	// 시간마다 변하는 실제 뼈대 변환행렬들
	RQuaternion	m_currentRotation[RBIPID_END];	// 현재의 회전값
	RVector		m_currentPosition[RBIPID_END];	// 현재의 이동값
	RMatrix		m_matCurrent[RBIPID_END];		// 현재 렌더링할 변환	= m_quatRotation,m_vecPosition 이 적용된 행렬
	RMatrix		m_matResult[RBIPID_END];		// parent로부터 곱해져 내려온 최종변환행렬

public:
	RSkeleton(void);
	~RSkeleton(void);

	void OnUpdate();
	void OnRender();

	void CreateBone(RMesh* pMesh);
	void SetAnimation(RAnimation* pAnimation);
	void SetAnimation(RAnimation* pAnimation, RAnimationFrameInfo& frameInfo, RANIMATIONTRANSITIONTYPE transType = RAT_NONE);

	void UpdateBone();
	void UpdateBoneFinal();
*/
	RSkeleton(void) {}
public:
	static RBIPID GetBipID(const char* szName);
	static RBIPID GetBipParent(RBIPID id);
	static const char* GetBipName(RBIPID id);
	static float GetBaseBoneImportance(RBIPID id);

/*

	void AddController(RSkeletonController* pController);
	bool RemoveController(RSkeletonController* pController);

	const RMatrix& GetBaseTransform(RBIPID bipID);
	const RMatrix& GetResultTransform(RBIPID bipID);

	const RMatrix& GetCurrentTransform(RBIPID bipID);
	const RQuaternion& GetCurrentRotation(RBIPID bipID);
	const RVector&	GetCurrentPosition(RBIPID bipID);

	const RMatrix& GetLocalTransform(RBIPID bipID);
	const RQuaternion& GetLocalRotation(RBIPID bipID);
	const RVector&	GetLocalPosition(RBIPID bipID);

// 임시변수들
	DWORD		m_dwBoneColor[RBIPID_END];	// 디버그용 렌더링할때 색깔

	RANIMATIONTRANSITIONTYPE GetAnimationTransitionStatus();
*/
};

/*
//////////////////////////////////////////////////////

inline const RMatrix& RSkeleton::GetBaseTransform(RBIPID bipID) { return m_matBase[bipID]; }
inline const RMatrix& RSkeleton::GetResultTransform(RBIPID bipID) { return m_matResult[bipID]; }

inline const RMatrix& RSkeleton::GetCurrentTransform(RBIPID bipID) { return m_matCurrent[bipID]; }
inline const RQuaternion& RSkeleton::GetCurrentRotation(RBIPID bipID) { return m_currentRotation[bipID]; }
inline const RVector& RSkeleton::GetCurrentPosition(RBIPID bipID) { return m_currentPosition[bipID]; }

inline const RMatrix& RSkeleton::GetLocalTransform(RBIPID bipID) { return m_matLocal[bipID]; }
inline const RQuaternion& RSkeleton::GetLocalRotation(RBIPID bipID) { return m_localRotation[bipID]; }
inline const RVector& RSkeleton::GetLocalPosition(RBIPID bipID) { return m_localPosition[bipID]; }
*/

}
