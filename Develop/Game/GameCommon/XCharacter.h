#pragma once

#include "RActor.h"
#include "RActorController.h"
#include "CSItemData.h"
#include "XCharacterParts.h"
#include "XAnimationInfoMgr.h"

using namespace rs3;

/// Raiderz 의 캐릭터 메쉬, RActor의 래퍼클래스
// item장착및 무기들의 장착등의 캐릭터의 규칙은 여기에 모두 정의된다.
// Character_Net 툴에서 같이 써야 하므로 *** 절대*** 아무거나 include 하지 말것

class XCharacter;
class XCharacterParts;
class XBaseAnimationTC;

//////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------
// XCharacterSpineController
class XCharacterSpineController : public rs3::RActorController
{
	RActorNode* m_pSpine1;
	RActorNode* m_pSpine2;

	float m_fSpineVertical;	// 허리 구부리기
	float m_fSpineHorizonal;

public:
	XCharacterSpineController()
		: rs3::RActorController(RSNCTRL_POSTANIMATION)
		, m_pSpine1(NULL), m_pSpine2(NULL), m_fSpineVertical(0), m_fSpineHorizonal(0)
	{}

	bool Create();
	void SetAngle(float fSpineAngleVertical, float fSpineAngleHorizonal);
	virtual void Update();
};


enum E_TRANSFORMSPACE
{
	E_TS_WORLD = 0,
	E_TS_LOCAL,

	E_TS_LOCAL_RESULT
};

class XCharacter : public RActor
{
protected:
	MDeclareRTTI;

	XCharacterSpineController*	m_pSpineController;

	float						m_fRadius;

	bool						m_bFakeResourceLoading;

	XCharacterParts*			m_pParts;
	RSceneNodeUpdateVisitor		m_SceneUpdator;

	RBoundingBox				m_InteractionAABB;

	XBaseAnimationTC*			m_pAnimationController;
	bool						m_bAnimationUpdated;
	bool						m_bDoNotAnimationUpdate;

	XAnimationInfoMgr*			m_pAnimationInfoMgr;

	bool						m_bLoadingCompleted;
protected:
	virtual void OnUpdate();
	virtual void OnActorLoadComplete();
	bool CheckLoadingCompleted();
public:
	XCharacter(void);
	virtual ~XCharacter(void);

	virtual bool Create(const TCHAR* szMeshName, bool bBackgroundCreation = false);
	virtual void Destroy();

	virtual bool IsLoadingCompleted();	///< 속도가 느리므로 단순히 로딩 완료체크할 때에는 CheckLoadingCompletedFastly 함수 사용하도록 한다.
	bool CheckLoadingCompletedFastly() { return m_bLoadingCompleted; }

	// 아이템 조합/ 파츠 관련

	void CreatePartsNodeAsync( XPartsParam* pPartsParam );
	void DestroyPartsNode( int nSlotID );
	void UpdatePartsNode(){ m_pParts->Update(); }
	void UpdatePartsNodeForNoneBackgroundIfAllMeshLoaded(){ m_pParts->UpdateForNoneBackgroundIfAllMeshLoaded(); }

	virtual void SetColor(unsigned int nColor);
	virtual void SetColor(unsigned int nColor, const TCHAR* _szNodeName);

	void SetActorSpineTarget(float fAngle1=0.f,float fAngle2=0.f);	// 좌우, 상하
	void RemoveActorSpineController();

	bool	IsEnableGlowEffect();
	void	EnableGlowEffect(const RVector3& color);
	void	DisableGlowEffect();
	void	EnableAddColorEffect(const RVector3& color);
	void	DisableAddColorEffect();
	void	EnableAddColorEffect(const TCHAR* _szNodeName, const RVector3& color);
	void	DisableAddColorEffect(const TCHAR* _szNodeName);

	float	GetMaxFrameToSec();					/// 현재애니메이션의 총 플레이 시간.
	float	GetCurFrameToSec();
	int		GetSecToFrame(float fTime);
	float	GetFrameToSec(int nFrame);
	int		GetCurFrame();
	int		GetMaxFrame();
	void	SetScale(float scale)				{ SetScale(vec3(scale, scale, scale));	}
	void	SetScale(const RVector& scale)		{ RActor::SetScale(scale);	}
	float	GetRadius() { return m_fRadius; }

	virtual bool	IsExistActorNode(tstring strDummyName);

	void	GetAnimationEventData(vector<ANIMATION_EVENT>& vecAnimaitonEvent);

	RActorNode* GetAnimatedActorNodeDir( const TCHAR* szNodeName, RVector& dir, E_TRANSFORMSPACE eTransformSpace);
	RActorNode* GetAnimatedActorNodeTransform( const TCHAR* szNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace);

	RActorNode* GetActorNodePos(const TCHAR* szNodeName, RVector& pos, E_TRANSFORMSPACE eTransformSpace, bool bForce = false);
	RActorNode* GetActorNodeTransform(const TCHAR* szNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace, bool bForce = false);

	bool	GetNodePosition(const TCHAR* szNodeName, RVector& vPos);

	
	virtual bool GetMovingDummyPosition(RVector& pos);					// 유닛 테스트 때문에 Virtual로 합니다.
	virtual bool GetMovingDummyDirection(RVector& dir);					// 유닛 테스트 때문에 Virtual로 합니다.

	void		UpdateAllSceneNodes();
	void		RefreshInteractionAABB();

	RBoundingBox	GetInteractionAABB()	{ return m_InteractionAABB; }

	void	GetPartsActorNode(int nSlotID, vector<RActorNode*>& vecPartsActorNode, vector<tstring>& vecPartsActorNodeName);

	void	SetActorListener(RListener* pListener);		///< event listener

	void	SetAnimationConroller(XBaseAnimationTC* pAnimationController)	{ m_pAnimationController = pAnimationController; }
	void	SetAnimationUpdated(bool bSet) { m_bAnimationUpdated = bSet; }
	bool	GetAnimationUpdated() { return m_bAnimationUpdated; }
	void	SetDoNotAnimationUpdated(bool bSet) { m_bDoNotAnimationUpdate = bSet; }
	bool	GetDoNotAnimationUpdated() { return m_bDoNotAnimationUpdate; }

	void	AddActiveActorNode(tstring strActorNodeName);
	void	AddActiveActorNode(RActorNode* pActorNode, bool bForceUpdate = false);

	void	SetAnimationInfoMgr(XAnimationInfoMgr* pMgr) { m_pAnimationInfoMgr = pMgr; }
	virtual XOverlayAnimationInfo* GetOverlayAnimaionInfo();
	virtual BLEND_DATA GetAnimationBlendData(tstring strSource, tstring strTarget);
	virtual XAnimationHitSoundInfo* GetAnimationHitSoundInfo();
	virtual XReActionAnimationInfo* GetReActionAnimationInfo();

private:
	unsigned int	m_dummy_loc_keyIteratorPosition;	///< dummy_loc 의 애니메이션 키 데이터에 접근하기 위한 책갈피
	unsigned int	m_dummy_loc_keyIteratorRotation;	///< dummy_loc 의 애니메이션 키 데이터에 접근하기 위한 책갈피
	RMatrix			m_matAnimatedTemp;					///< AnimatedTranform을 구하기 위한 temp

private:

	// 사용하지 않음 ----
	void SetAngleVertical(float fSpineAngleVertical);
	// 사용하지 않음 ----
public:
	// for test -----
	void SetAnimationForMock();
	tstring GetReservedSlotMeshName(int nSlotID);		// for test
	// for test -----
	void DumpSceneNode(const TCHAR* szFileName);
};

