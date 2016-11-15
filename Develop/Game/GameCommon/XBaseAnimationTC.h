#pragma once

#include "XTransformController.h"

enum BASE_ANIMATION_INDEX
{
	BAI_BASE_1 = 0,
	BAI_BASE_2,

	BAI_MAX
};

class XBaseAnimationTC : public XTransformController
{
protected:
	XRAnimationController *				m_pCurrentRAnimationController;
	XRAnimationController *				m_pPrevRAnimationController;
	XRAnimationController *				m_pRAnimationController[BAI_MAX];

	RANIMATIONTRANSITIONTYPE			m_eAnimationBlendType;
	float								m_fRequestedBlendTime;
	float								m_fElapsedBlendTime;

	vector<tstring>						m_vecActiveNodeName;

	tstring								m_strLastAni;
	tstring								m_strReservedAni;	///< 로딩이 끝났을때 복구해주기 위한 것

private:
	bool								CheckDuplicationActiveNodeName(tstring& strNodeName);
	bool								CheckUseVertexAnimation(XRAnimationController * pAnimationController);
	bool								CheckDonotPushActiveNodeName(tstring& strNodeName);
	
	void								UpdateWeaponBlend(XPlayerPartsSlotType slotType);
	void								CheckWeaponBlendVisibility(tstring strParentName);

	bool								PushActiveActorNode(tstring& strNodeName);

	tstring								GetWeaponParentName(tstring& strParentName, string strPostfix);

protected:

public:
	XBaseAnimationTC(XCharacter* pCharacter);
	virtual ~XBaseAnimationTC(void);

	virtual void						PreUpdate(float fDelta);
	virtual void						Update(float fDelta);
	virtual void						PostUpdate(float fDelta);
	virtual void						Stop(){}

	virtual bool 						SetAnimation(tstring& strAniName, bool bForce=false, bool bSameFrame = false);
	void								SetAniName(tstring& strAniName) { m_strLastAni = strAniName; }

	void								CheckActiveActorNode(tstring& strNodeName);
	void								CheckActiveActorNode(RActorNode* pActorNode, bool bForceUpdate = false);

	virtual float						GetAnimationSpeed();
	virtual void						SetAnimationSpeed(float fSpeed);
	virtual void						SetAnimationFrame(unsigned int nFrame);
	virtual float						GetMaxFrameToSecond();
	virtual float						GetCurFrameToSecond();

	virtual void						ResetAniamtionSpeed();
	virtual float						GetOrgAniamtionSpeed();

	virtual void						PauseAnimation();
	virtual void						StopAnimation();
	virtual void						PlayAnimation();

	virtual tstring						GetAnimationName();
	virtual int							GetAniFrame();
	virtual int							GetAniFrameWithUpdateTime();
	virtual int							GetAniMaxFrame();
	RAnimation *						GetAnimation();

	tstring								GetPreAnimaionName();
	int									GetPreAniFrame();
	float								GetPreAniFrameToSec(unsigned int nFrame);

	virtual void						GetActiveRActorNodeList(vector<tstring>& vecActorNodeList);

	vector<ANIMATION_EVENT>*			GetAnimationEvent();

	virtual bool						IsAnimationLoop();
	virtual bool						IsPlayDone();
	virtual bool						IsOncePlayDone();
	bool								IsNodeController(tstring& strDummyName);
	bool								IsActiveNodeController(tstring& strNodeName);
	bool								IsAnimationUsable();

	void								CloseEvent();

	void								CheckReservedAnimation();

	XRAnimationController *				GetCurrentAnimationController() { return m_pCurrentRAnimationController; }

	//////////////////////////////////////////////////////////////////////////
	virtual RActorNode*					GetAnimatedActorNodeTransform(tstring& strNodeName, RMatrix& _rOut, E_TRANSFORMSPACE eTransformSpace);
	bool								GetActorNodePosByFrame(tstring& strNodeName, unsigned int nFrame, vec3& vOut);


	static TRANSFORM_CONTROLLER_LIST	GetID() { return TCL_BASE; }
};
